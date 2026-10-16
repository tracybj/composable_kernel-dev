#include "two_kernel.hpp"
#include <rccl/rccl.h>
#include <mpi.h>
#include <dlfcn.h>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>
#define HIP_OK(x) do{auto e=(x);if(e!=hipSuccess){fprintf(stderr,"HIP %d %s\n",__LINE__,hipGetErrorString(e));MPI_Abort(MPI_COMM_WORLD,2);}}while(0)
#define NC_OK(x) do{auto e=(x);if(e!=ncclSuccess){fprintf(stderr,"RCCL %d %s\n",__LINE__,ncclGetErrorString(e));MPI_Abort(MPI_COMM_WORLD,3);}}while(0)
#define DS_OK(x) do{int e=(x);if(e){fprintf(stderr,"DUSHMEM %d %d\n",__LINE__,e);MPI_Abort(MPI_COMM_WORLD,4);}}while(0)
using GemmFn=int(*)(const void*,const void*,void*,int,int,int,hipStream_t);
__host__ __device__ int av(int rank,int row,int k,int tag){return ((row+tag)*5+k*3+rank*7)%17-8;}
__host__ __device__ int bv(int rank,int col,int k){return (col*7+k*5+rank*3)%19-9;}
__global__ void init(T*x,int rows,int k,int rank,int tag,int weight){
    for(size_t i=size_t(blockIdx.x)*blockDim.x+threadIdx.x;i<size_t(rows)*k;i+=size_t(gridDim.x)*blockDim.x)
        x[i]=ck_tile::type_convert<T>(float(weight?bv(rank,i/k,i%k):av(rank,i/k,i%k,tag))/16);
}
__global__ void init_random(T*x,size_t count,uint32_t seed){
    for(size_t i=size_t(blockIdx.x)*blockDim.x+threadIdx.x;i<count;i+=size_t(gridDim.x)*blockDim.x){
        uint32_t h=uint32_t(i)^seed;h^=h>>16;h*=0x7feb352d;h^=h>>15;h*=0x846ca68b;h^=h>>16;
        x[i]=ck_tile::bit_cast<T>(gfx936::ConvertBf16Rne((int(h&65535)-32768)/32768.0f));
    }
}
__global__ void check(const T*out,const T*ref,unsigned long long*errors,int m,int n,int tag,bool exact){
    for(size_t i=size_t(blockIdx.x)*blockDim.x+threadIdx.x;i<size_t(m)*n;i+=size_t(gridDim.x)*blockDim.x){
        size_t r=exact?i:((i/n+tag)%17)*19+(i%n)%19;
        if(ck_tile::bit_cast<uint16_t>(out[i])!=ck_tile::bit_cast<uint16_t>(ref[r]))atomicAdd(errors,1ULL);
    }
    for(int i=blockIdx.x*blockDim.x+threadIdx.x;i<256;i+=gridDim.x*blockDim.x)
        if(ck_tile::bit_cast<uint16_t>(out[size_t(m)*n+i])!=0xffff)atomicAdd(errors,1ULL);
}
static T bf16(float f){uint32_t b;std::memcpy(&b,&f,4);b+=0x7fff+((b>>16)&1);uint16_t h=b>>16;T t;std::memcpy(&t,&h,2);return t;}
static float fp32(T t){uint16_t h;std::memcpy(&h,&t,2);uint32_t b=uint32_t(h)<<16;float f;std::memcpy(&f,&b,4);return f;}
static void launch_single(Args a,bool trace,hipStream_t stream){
    void*params[]={&a};
    if(trace)DS_OK(dushmemx_collective_launch((const void*)role_kernel<true>,dim3(80),dim3(P::BlockSize),params,0,stream));
    else DS_OK(dushmemx_collective_launch((const void*)role_kernel<false>,dim3(80),dim3(P::BlockSize),params,0,stream));
}
template<bool Trace>
static void launch_compute(Args a,hipStream_t stream){
    hipLaunchKernelGGL((compute_kernel<Trace>),dim3(a.compute),dim3(P::BlockSize),0,stream,a);
    HIP_OK(hipGetLastError());
}
template<bool Trace,int Threads>
static void launch_communication(Args a,hipStream_t stream){
    hipLaunchKernelGGL((communication_kernel<Trace,Threads>),dim3(a.comm),dim3(Threads),0,stream,a);
    HIP_OK(hipGetLastError());
}
template<bool Trace>
static void dispatch_communication(Args a,int threads,hipStream_t stream){
    if(threads==128)launch_communication<Trace,128>(a,stream);
    else if(threads==256)launch_communication<Trace,256>(a,stream);
    else launch_communication<Trace,512>(a,stream);
}
static void resources(){
    hipFuncAttributes c{},r128{},r256{},r512{};
    HIP_OK(hipFuncGetAttributes(&c,(const void*)compute_kernel<false>));
    HIP_OK(hipFuncGetAttributes(&r128,(const void*)communication_kernel<false,128>));
    HIP_OK(hipFuncGetAttributes(&r256,(const void*)communication_kernel<false,256>));
    HIP_OK(hipFuncGetAttributes(&r512,(const void*)communication_kernel<false,512>));
    printf("RESOURCES compute regs=%d LDS=%zu private=%zu; comm128 regs=%d LDS=%zu private=%zu; comm256 regs=%d LDS=%zu private=%zu; comm512 regs=%d LDS=%zu private=%zu\n",
       c.numRegs,c.sharedSizeBytes,c.localSizeBytes,r128.numRegs,r128.sharedSizeBytes,r128.localSizeBytes,
       r256.numRegs,r256.sharedSizeBytes,r256.localSizeBytes,r512.numRegs,r512.sharedSizeBytes,r512.localSizeBytes);fflush(stdout);
}
static void test(int rank,ncclComm_t nc,GemmFn gemm,int m,int n,int k,int compute,int comm,int threads,int rounds,const std::string&kind){
    int total=((m+255)/256)*(n/256);
    Args a{};a.m=m;a.n=n;a.k=k;a.compute=compute;a.comm=comm;
    size_t count=size_t(m)*n,bytes=(count+256)*2;
    T *x,*w,*out,*refsum,*reflocal,*expectsum=nullptr,*expectlocal=nullptr;
    unsigned long long*errors;
    HIP_OK(hipMalloc(&x,size_t(m)*k*2));HIP_OK(hipMalloc(&w,size_t(n)*k*2));HIP_OK(hipMalloc(&out,bytes));
    HIP_OK(hipMalloc(&refsum,17*19*2));HIP_OK(hipMalloc(&reflocal,17*19*2));HIP_OK(hipMalloc(&errors,8));
    HIP_OK(hipMalloc(&a.produced,total*8));HIP_OK(hipMalloc(&a.finished,80*8));HIP_OK(hipMalloc(&a.trace,total*6*8));
    if(kind=="gate"){HIP_OK(hipMalloc(&expectsum,count*2));HIP_OK(hipMalloc(&expectlocal,count*2));}
    a.partial=(T*)dushmem_malloc(bytes);a.remote_ready=(uint64_t*)dushmem_malloc(total*8);
    if(!a.partial||!a.remote_ready)MPI_Abort(MPI_COMM_WORLD,5);
    a.peer_partial=(T*)dushmem_ptr(a.partial,1-rank);
    if(!a.peer_partial)MPI_Abort(MPI_COMM_WORLD,6);
    a.input=x;a.weight=w;a.output=out;
    HIP_OK(hipMemset(a.produced,0,total*8));HIP_OK(hipMemset(a.finished,0,80*8));HIP_OK(hipMemset(a.remote_ready,0,total*8));
    hipLaunchKernelGGL(init,dim3(256),dim3(256),0,0,w,n,k,rank,0,1);HIP_OK(hipGetLastError());
    HIP_OK(hipDeviceSynchronize());dushmem_barrier_all();
    std::vector<T> hs(17*19),hl(17*19);
    for(int row=0;row<17;++row)for(int col=0;col<19;++col){
        float loc[2];for(int r=0;r<2;++r){int acc=0;for(int j=0;j<k;++j)acc+=av(r,row,j,0)*bv(r,col,j);loc[r]=fp32(bf16(float(acc)/256));}
        hs[row*19+col]=bf16(loc[0]+loc[1]);hl[row*19+col]=bf16(loc[rank]);
    }
    HIP_OK(hipMemcpy(refsum,hs.data(),hs.size()*2,hipMemcpyHostToDevice));HIP_OK(hipMemcpy(reflocal,hl.data(),hl.size()*2,hipMemcpyHostToDevice));
    void*params[]={&a};int cap=0,minimum=0;
    DS_OK(dushmemx_collective_launch_query_gridsize((const void*)role_kernel<false>,dim3(P::BlockSize),params,0,&cap));
    MPI_Allreduce(&cap,&minimum,1,MPI_INT,MPI_MIN,MPI_COMM_WORLD);
    if(minimum<80){fprintf(stderr,"Need 80 resident CTAs; got %d\n",minimum);MPI_Abort(MPI_COMM_WORLD,7);}
    if(kind=="trace"){
        DS_OK(dushmemx_collective_launch_query_gridsize((const void*)role_kernel<true>,dim3(P::BlockSize),params,0,&cap));
        MPI_Allreduce(&cap,&minimum,1,MPI_INT,MPI_MIN,MPI_COMM_WORLD);if(minimum<80)MPI_Abort(MPI_COMM_WORLD,8);
    }
    if(rank==0){printf("CAPACITY m=%d n=%d k=%d compute=%d comm=%d comm_threads=%d single_cap=%d tiles=%d\n",m,n,k,a.compute,a.comm,threads,minimum,total);fflush(stdout);resources();}
    const char*names[]={"v08_rccl","single_serial80","single_pipeline72_8","two_serial","two_overlap","compute_only"};
    hipStream_t gs,rs,join;HIP_OK(hipStreamCreateWithFlags(&gs,hipStreamNonBlocking));
    HIP_OK(hipStreamCreateWithFlags(&rs,hipStreamNonBlocking));HIP_OK(hipStreamCreateWithFlags(&join,hipStreamNonBlocking));
    hipEvent_t start,end,gdone,rdone;
    HIP_OK(hipEventCreate(&start));HIP_OK(hipEventCreate(&end));HIP_OK(hipEventCreate(&gdone));HIP_OK(hipEventCreate(&rdone));
    uint64_t epoch=0;
    int endround=kind=="trace"?1:rounds;
    for(int round=-1;round<endround;++round)for(int j=0;j<6;++j){
        int mode=round<0?j:(j+round)%6;
        int reps=round<0?3:(kind=="trace"?1:5);
        for(int rep=0;rep<reps;++rep){
            a.epoch=++epoch;int tag=epoch%17;
            bool trace=kind=="trace"&&round>=0&&mode!=0;
            if(kind=="gate"){
                hipLaunchKernelGGL(init_random,dim3(256),dim3(256),0,0,x,size_t(m)*k,12345+rank*1000+epoch);
                hipLaunchKernelGGL(init_random,dim3(256),dim3(256),0,0,w,size_t(n)*k,56789+rank*1000+epoch);
                HIP_OK(hipGetLastError());HIP_OK(hipError_t(gemm(x,w,expectlocal,m,n,k,0)));
                HIP_OK(hipMemcpyAsync(expectsum,expectlocal,count*2,hipMemcpyDeviceToDevice,0));
                NC_OK(ncclAllReduce(expectsum,expectsum,count,ncclBfloat16,ncclSum,nc,0));
            }else{
                hipLaunchKernelGGL(init,dim3(256),dim3(256),0,0,x,m,k,rank,tag,0);HIP_OK(hipGetLastError());
            }
            HIP_OK(hipMemset(out,0xff,bytes));HIP_OK(hipMemset(a.partial,0xff,bytes));HIP_OK(hipMemset(errors,0,8));
            if(trace)HIP_OK(hipMemset(a.trace,0,total*6*8));
            HIP_OK(hipDeviceSynchronize());MPI_Barrier(MPI_COMM_WORLD);
            HIP_OK(hipEventRecord(start,join));
            HIP_OK(hipStreamWaitEvent(gs,start,0));HIP_OK(hipStreamWaitEvent(rs,start,0));
            if(mode==0){
                HIP_OK(hipError_t(gemm(x,w,out,m,n,k,gs)));NC_OK(ncclAllReduce(out,out,count,ncclBfloat16,ncclSum,nc,gs));
                HIP_OK(hipEventRecord(gdone,gs));HIP_OK(hipEventRecord(rdone,rs));
            }else if(mode==1||mode==2){
                Args b=a;b.compute=72;b.comm=8;b.mode=mode==1?0:2;
                launch_single(b,trace,gs);HIP_OK(hipEventRecord(gdone,gs));HIP_OK(hipEventRecord(rdone,rs));
            }else{
                if(trace)launch_compute<true>(a,gs);else launch_compute<false>(a,gs);
                HIP_OK(hipEventRecord(gdone,gs));
                // Only the auxiliary serial control waits for whole-GEMM completion.
                // two_overlap has NO gdone dependency: it waits on per-tile flags.
                if(mode==3)HIP_OK(hipStreamWaitEvent(rs,gdone,0));
                if(mode!=5){
                    if(trace)dispatch_communication<true>(a,threads,rs);
                    else dispatch_communication<false>(a,threads,rs);
                }
                HIP_OK(hipEventRecord(rdone,rs));
            }
            HIP_OK(hipStreamWaitEvent(join,gdone,0));HIP_OK(hipStreamWaitEvent(join,rdone,0));
            HIP_OK(hipEventRecord(end,join));HIP_OK(hipEventSynchronize(end));
            float ms,worst,gt,rt;HIP_OK(hipEventElapsedTime(&ms,start,end));
            HIP_OK(hipEventElapsedTime(&gt,start,gdone));HIP_OK(hipEventElapsedTime(&rt,start,rdone));
            MPI_Allreduce(&ms,&worst,1,MPI_FLOAT,MPI_MAX,MPI_COMM_WORLD);
            const T*actual=mode==5?a.partial:out;
            const T*reference=kind=="gate"?(mode==5?expectlocal:expectsum):(mode==5?reflocal:refsum);
            hipLaunchKernelGGL(check,dim3(512),dim3(256),0,0,actual,reference,errors,m,n,tag,kind=="gate");HIP_OK(hipGetLastError());
            unsigned long long err=0,allerr=0;HIP_OK(hipMemcpy(&err,errors,8,hipMemcpyDeviceToHost));MPI_Allreduce(&err,&allerr,1,MPI_UNSIGNED_LONG_LONG,MPI_SUM,MPI_COMM_WORLD);
            if(rank==0){printf("{\"m\":%d,\"n\":%d,\"k\":%d,\"compute\":%d,\"comm\":%d,\"comm_threads\":%d,\"mode\":\"%s\",\"round\":%d,\"rep\":%d,\"epoch\":%llu,\"kind\":\"%s\",\"trace\":%s,\"errors\":%llu,\"maxrank_ms\":%.6f,\"rank0_compute_done_ms\":%.6f,\"rank0_comm_done_ms\":%.6f}\n",m,n,k,a.compute,a.comm,threads,names[mode],round,rep,(unsigned long long)epoch,kind.c_str(),trace?"true":"false",allerr,worst,gt,rt);fflush(stdout);}
            if(allerr)MPI_Abort(MPI_COMM_WORLD,9);
            if(trace){
                std::vector<uint64_t>ts(total*6);HIP_OK(hipMemcpy(ts.data(),a.trace,ts.size()*8,hipMemcpyDeviceToHost));
                std::string path="trace_"+std::to_string(k)+"_c"+std::to_string(a.compute)+"_r"+std::to_string(a.comm)+"_b"+std::to_string(threads)+"_"+names[mode]+"_rank"+std::to_string(rank)+".csv";
                std::ofstream f(path);f<<"tile,producer,consumer,g_start,g_end,published,r_start,peer_ready,r_end\n";
                int pc=mode==1?80:(mode==2?72:a.compute),rc=mode==1?80:(mode==2?8:a.comm);
                for(int tile=0;tile<total;++tile){f<<tile<<','<<tile%pc<<','<<tile%rc;for(int z=0;z<6;++z)f<<','<<ts[tile*6+z];f<<'\n';}
            }
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);dushmem_barrier_all();dushmem_free(a.remote_ready);dushmem_free(a.partial);
    HIP_OK(hipFree(x));HIP_OK(hipFree(w));HIP_OK(hipFree(out));HIP_OK(hipFree(refsum));HIP_OK(hipFree(reflocal));HIP_OK(hipFree(errors));
    HIP_OK(hipFree(a.produced));HIP_OK(hipFree(a.finished));HIP_OK(hipFree(a.trace));
    if(expectsum)HIP_OK(hipFree(expectsum));if(expectlocal)HIP_OK(hipFree(expectlocal));
    HIP_OK(hipEventDestroy(start));HIP_OK(hipEventDestroy(end));HIP_OK(hipEventDestroy(gdone));HIP_OK(hipEventDestroy(rdone));
    HIP_OK(hipStreamDestroy(gs));HIP_OK(hipStreamDestroy(rs));HIP_OK(hipStreamDestroy(join));
}
int main(int argc,char**argv){
    MPI_Init(&argc,&argv);int rank,np;MPI_Comm_rank(MPI_COMM_WORLD,&rank);MPI_Comm_size(MPI_COMM_WORLD,&np);
    if(np!=2||argc<2)MPI_Abort(MPI_COMM_WORLD,1);
    HIP_OK(hipSetDevice(rank));MPI_Comm mpi=MPI_COMM_WORLD;
    dushmemx_init_attr_t attr=DUSHMEMX_INIT_ATTR_INITIALIZER;attr.mpi_comm=&mpi;DS_OK(dushmemx_init_attr(DUSHMEMX_INIT_WITH_MPI_COMM,&attr));
    ncclUniqueId id;if(rank==0)NC_OK(ncclGetUniqueId(&id));MPI_Bcast(&id,sizeof(id),MPI_BYTE,0,MPI_COMM_WORLD);
    ncclComm_t nc;NC_OK(ncclCommInitRank(&nc,2,id,rank));
    void*lib=dlopen(argv[1],RTLD_NOW|RTLD_LOCAL);if(!lib){fprintf(stderr,"%s\n",dlerror());MPI_Abort(mpi,10);}
    auto gemm=(GemmFn)dlsym(lib,"h3_gemm");if(!gemm)MPI_Abort(mpi,11);
    int compute=argc>2?atoi(argv[2]):72,comm=argc>3?atoi(argv[3]):8,threads=argc>4?atoi(argv[4]):256,rounds=argc>5?atoi(argv[5]):3;
    std::string kind=argc>6?argv[6]:"full",shape=argc>7?argv[7]:"fc2";
    if(compute<1||compute>80||comm<1||comm>32||(threads!=128&&threads!=256&&threads!=512))MPI_Abort(mpi,12);
    if(kind=="smoke")test(rank,nc,gemm,257,5376,7168,compute,comm,threads,1,kind);
    else{
        if(shape!="out")test(rank,nc,gemm,18880,5376,7168,compute,comm,threads,rounds,kind);
        if(shape!="fc2")test(rank,nc,gemm,18880,5376,3584,compute,comm,threads,rounds,kind);
    }
    NC_OK(ncclCommDestroy(nc));dushmem_finalize();dlclose(lib);if(rank==0)puts("PIPELINE_PASS");MPI_Finalize();return 0;
}
