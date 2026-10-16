// Isolated TP2 BF16 GEMM + AllReduce benchmark. No H3 model/runtime changes.
#include "gemm_kernel.hpp"
#include <dushmem.h>
#include <dushmemx.h>
#include <rccl/rccl.h>
#include <mpi.h>
#include <dlfcn.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>
#include <string>
using namespace ck_tile::h3;
using T = ck_tile::bf16_t;
using P = H3GemmProblem;
using Policy = ProducerPolicy;
using Pipe = H3GemmPipeline<P, Policy>;
constexpr int E = 256 * 256;
#define HIP_OK(x) do { auto e=(x); if(e!=hipSuccess){fprintf(stderr,"HIP %d %s\n",__LINE__,hipGetErrorString(e));MPI_Abort(MPI_COMM_WORLD,2);} } while(0)
#define NCCL_OK(x) do { auto e=(x); if(e!=ncclSuccess){fprintf(stderr,"RCCL %d %s\n",__LINE__,ncclGetErrorString(e));MPI_Abort(MPI_COMM_WORLD,3);} } while(0)
#define DS_OK(x) do { int e=(x); if(e){fprintf(stderr,"DUSHMEM %d %d\n",__LINE__,e);MPI_Abort(MPI_COMM_WORLD,4);} } while(0)
struct Args {
    const T *input, *weight;
    T *local, *remote, *output, *peer_local;
    uint64_t *ready;
    int m,n,k,workers,rank,mode;
    uint64_t epoch;
};

// Keep v08's compute pipeline unchanged; only tile scheduling and epilogue
// destination differ. Packed tiles permit one contiguous device-side put.
template<bool Direct>
__device__ __attribute__((always_inline)) void compute_tile(const Args& a, int tile, T* shared) {
    int ft=a.n/256, nt=(a.m+255)/256;
    int span=P::TokenGroup*ft, first=(tile/span)*P::TokenGroup;
    int size=min(P::TokenGroup,nt-first), rem=tile%span;
    int fm=(rem/size)*256, tn=(first+rem%size)*256;
    int id=threadIdx.x;
    if(id >= P::ComputeWaves*64) {
        auto resource=ck_tile::make_wave_buffer_resource(a.weight+fm*a.k,(a.n-fm)*a.k*2);
        Pipe::RunProducer(shared,resource,id-P::ComputeWaves*64,a.k/P::WeightK,a.k);
    } else {
        int wave=id/64,lane=id%64,row=lane%16,klane=(lane/16)*8;
        int wave_m=wave/P::TokenWaves,wave_n=wave%P::TokenWaves;
        auto resource=Policy::ActivationAccess::MakeResource(a.input+tn*a.k,a.m-tn,a.k);
        P::AccVector accum[P::TokenFrags][P::FeatureFrags]={};
        Pipe::RunConsumer(shared,resource,accum,a.k/P::WeightK,a.k,wave,wave_m,wave_n,row,klane);
        // All 256 rows are stored, including zero-filled padded input rows.
        if constexpr(Direct) {
            auto dst=ck_tile::make_wave_buffer_resource(a.local+size_t(tn)*a.n,(a.m-tn)*a.n*2);
            H3GemmEpilogue<P>{}(accum,dst,a.n,fm,wave_n,row,lane);
        } else {
            auto dst=ck_tile::make_wave_buffer_resource(a.local+size_t(tile)*E,E*2);
            H3GemmEpilogue<P>{}(accum,dst,256,0,wave_n,row,lane);
        }
    }
    // v08 stores are complete before any communication lane reads this tile.
    __threadfence();
    __syncthreads();
}

// Intra-node P2P path: dushmem_ptr provides the peer mapping. Data remains in
// the original row-major partial output; only readiness is sent through the
// device communication API. This is not RCCL or a general multinode collective.
__device__ __attribute__((always_inline)) void direct_exchange_tile(const Args& a,int tile) {
    __threadfence_system();__syncthreads();
    if(threadIdx.x==0) {
        dushmemx_signal_op(a.ready+tile,a.epoch,DUSHMEM_SIGNAL_SET,1-a.rank);
        dushmem_quiet();
        dushmem_signal_wait_until(a.ready+tile,DUSHMEM_CMP_GE,a.epoch);
    }
    __syncthreads();
    int ft=a.n/256,nt=(a.m+255)/256,span=P::TokenGroup*ft;
    int first=(tile/span)*P::TokenGroup,size=min(P::TokenGroup,nt-first),rem=tile%span;
    int fm=(rem/size)*256,tn=(first+rem%size)*256;
    for(int i=threadIdx.x;i<E;i+=blockDim.x) {
        int row=tn+i/256,col=fm+i%256;
        if(row<a.m) {
            size_t pos=size_t(row)*a.n+col;
            float sum=ck_tile::type_convert<float>(a.local[pos])+ck_tile::type_convert<float>(a.peer_local[pos]);
            a.output[pos]=ck_tile::bit_cast<T>(gfx936::ConvertBf16Rne(sum));
        }
    }
    __threadfence();__syncthreads();
}

__device__ __attribute__((always_inline)) void exchange_tile(const Args& a, int tile) {
    size_t off=size_t(tile)*E;
    dushmemx_putmem_signal_block(a.remote+off,a.local+off,E*sizeof(T),
        a.ready+tile,a.epoch,DUSHMEM_SIGNAL_SET,1-a.rank);
    if(threadIdx.x==0) dushmem_signal_wait_until(a.ready+tile,DUSHMEM_CMP_GE,a.epoch);
    __syncthreads();
    int ft=a.n/256,nt=(a.m+255)/256,span=P::TokenGroup*ft;
    int first=(tile/span)*P::TokenGroup,size=min(P::TokenGroup,nt-first),rem=tile%span;
    int fm=(rem/size)*256,tn=(first+rem%size)*256;
    for(int i=threadIdx.x;i<E;i+=blockDim.x) {
        int row=tn+i/256,col=fm+i%256;
        if(row<a.m) {
            float sum=ck_tile::type_convert<float>(a.local[off+i])+ck_tile::type_convert<float>(a.remote[off+i]);
            a.output[size_t(row)*a.n+col]=ck_tile::bit_cast<T>(gfx936::ConvertBf16Rne(sum));
        }
    }
    __threadfence();
    __syncthreads();
    if(threadIdx.x==0) dushmem_quiet();
    __syncthreads();
}

// Fixed matched worker order on both PEs; cooperative capacity checked before launch.
// mode 0=compute only; 1=communication only; 2=single-kernel tile compute+exchange.
// Across CTAs computation and communication may overlap; within each CTA they
// are sequential. This is NOT a claim of same-CTA GEMM/communication overlap.
template<bool Direct>
__global__ __launch_bounds__(P::BlockSize,1) void tile_kernel(Args a) {
    __shared__ T shared[P::LdsElements];
    int total=((a.m+255)/256)*(a.n/256);
    for(int tile=blockIdx.x;tile<total;tile+=a.workers) {
        if(a.mode!=1) compute_tile<Direct>(a,tile,shared);
        if(a.mode!=0) {
            if constexpr(Direct) direct_exchange_tile(a,tile);
            else exchange_tile(a,tile);
        }
    }
}

__host__ __device__ int av(int rank,int row,int k,int tag) {return ((row+tag)*5+k*3+rank*7)%17-8;}
__host__ __device__ int bv(int rank,int col,int k) {return (col*7+k*5+rank*3)%19-9;}
__global__ void init(T* x,int rows,int k,int rank,int tag,int weight) {
    for(size_t i=size_t(blockIdx.x)*blockDim.x+threadIdx.x;i<size_t(rows)*k;i+=size_t(gridDim.x)*blockDim.x)
        x[i]=ck_tile::type_convert<T>(float(weight?bv(rank,int(i/k),i%k):av(rank,int(i/k),i%k,tag))/16);
}
__global__ void check(const T* out,const T* ref,unsigned long long* errors,int m,int n,int tag) {
    for(size_t i=size_t(blockIdx.x)*blockDim.x+threadIdx.x;i<size_t(m)*n;i+=size_t(gridDim.x)*blockDim.x)
        if(ck_tile::bit_cast<uint16_t>(out[i])!=ck_tile::bit_cast<uint16_t>(ref[((i/n+tag)%17)*19+(i%n)%19]))
            atomicAdd(errors,1ULL);
    for(int i=blockIdx.x*blockDim.x+threadIdx.x;i<256;i+=gridDim.x*blockDim.x)
        if(ck_tile::bit_cast<uint16_t>(out[size_t(m)*n+i])!=0xffff) atomicAdd(errors,1ULL);
}
static T bf16(float f) {
    uint32_t bits;std::memcpy(&bits,&f,4);bits+=0x7fff+((bits>>16)&1);
    uint16_t hi=bits>>16;T out;std::memcpy(&out,&hi,2);return out;
}
static float fp32(T t) {uint16_t lo;std::memcpy(&lo,&t,2);uint32_t b=uint32_t(lo)<<16;float f;std::memcpy(&f,&b,4);return f;}
using GemmFn=int(*)(const void*,const void*,void*,int,int,int,hipStream_t);

static void test(int rank,ncclComm_t comm,GemmFn gemm,int m,int n,int k,int requested,int rounds) {
    int total=((m+255)/256)*(n/256);
    Args a{};a.m=m;a.n=n;a.k=k;a.rank=rank;a.workers=requested;
    T *x,*w,*out,*ref;
    unsigned long long* errors;
    HIP_OK(hipMalloc(&x,size_t(m)*k*2));HIP_OK(hipMalloc(&w,size_t(n)*k*2));
    HIP_OK(hipMalloc(&out,(size_t(m)*n+256)*2));HIP_OK(hipMalloc(&ref,17*19*2));
    HIP_OK(hipMalloc(&errors,8));
    a.input=x;a.weight=w;a.output=out;
    size_t bytes=size_t(total)*E*2;
    a.local=(T*)dushmem_malloc(bytes);a.remote=(T*)dushmem_malloc(bytes);
    a.ready=(uint64_t*)dushmem_malloc(total*sizeof(uint64_t));
    if(!a.local||!a.remote||!a.ready) MPI_Abort(MPI_COMM_WORLD,5);
    a.peer_local=(T*)dushmem_ptr(a.local,1-rank);
    if(!a.peer_local) {fprintf(stderr,"No intra-node peer pointer\n");MPI_Abort(MPI_COMM_WORLD,10);}
    HIP_OK(hipMemset(a.ready,0,total*sizeof(uint64_t)));
    hipLaunchKernelGGL(init,dim3(256),dim3(256),0,0,w,n,k,rank,0,1);
    HIP_OK(hipGetLastError());HIP_OK(hipDeviceSynchronize());dushmem_barrier_all();
    std::vector<T> h_ref(17*19);
    for(int row=0;row<17;++row)for(int col=0;col<19;++col){
        float local[2];for(int r=0;r<2;++r){int acc=0;for(int j=0;j<k;++j)acc+=av(r,row,j,0)*bv(r,col,j);local[r]=fp32(bf16(float(acc)/256));}
        h_ref[row*19+col]=bf16(local[0]+local[1]);
    }
    HIP_OK(hipMemcpy(ref,h_ref.data(),h_ref.size()*2,hipMemcpyHostToDevice));
    void* params[]={&a};int cap=0,mincap=0;
    DS_OK(dushmemx_collective_launch_query_gridsize((const void*)tile_kernel<false>,dim3(P::BlockSize),params,0,&cap));
    int direct_cap=0;
    DS_OK(dushmemx_collective_launch_query_gridsize((const void*)tile_kernel<true>,dim3(P::BlockSize),params,0,&direct_cap));
    cap=std::min(cap,direct_cap);
    MPI_Allreduce(&cap,&mincap,1,MPI_INT,MPI_MIN,MPI_COMM_WORLD);
    a.workers=std::min({requested,mincap,total});
    if(a.workers<1)MPI_Abort(MPI_COMM_WORLD,6);
    if(rank==0){printf("CAPACITY m=%d n=%d k=%d cap=%d workers=%d tiles=%d\n",m,n,k,mincap,a.workers,total);fflush(stdout);}
    const char* names[]={"v08_rccl","recompiled_rccl","tile_split","tile_fused","direct_split","direct_fused"};
    hipEvent_t start,end;HIP_OK(hipEventCreate(&start));HIP_OK(hipEventCreate(&end));
    uint64_t epoch=0;
    for(int round=-1;round<rounds;++round)for(int j=0;j<6;++j){
        int mode=(round<0?j:(round%2?5-j:j));
        for(int rep=0;rep<(round<0?3:5);++rep){
            a.epoch=++epoch;int tag=epoch%17;
            hipLaunchKernelGGL(init,dim3(256),dim3(256),0,0,x,m,k,rank,tag,0);
            HIP_OK(hipGetLastError());HIP_OK(hipMemset(out,0xff,(size_t(m)*n+256)*2));
            HIP_OK(hipMemset(errors,0,8));HIP_OK(hipDeviceSynchronize());MPI_Barrier(MPI_COMM_WORLD);
            HIP_OK(hipEventRecord(start));
            if(mode<2){
                if(mode==0)HIP_OK(hipError_t(gemm(x,w,out,m,n,k,0)));
                else HIP_OK(hipError_t(Launch<Policy>(x,w,out,m,n,k,0)));
                NCCL_OK(ncclAllReduce(out,out,size_t(m)*n,ncclBfloat16,ncclSum,comm,0));
            }else if(mode==2){
                a.mode=0;DS_OK(dushmemx_collective_launch((const void*)tile_kernel<false>,dim3(a.workers),dim3(P::BlockSize),params,0,0));
                a.mode=1;DS_OK(dushmemx_collective_launch((const void*)tile_kernel<false>,dim3(a.workers),dim3(P::BlockSize),params,0,0));
            }else if(mode==3){
                a.mode=2;DS_OK(dushmemx_collective_launch((const void*)tile_kernel<false>,dim3(a.workers),dim3(P::BlockSize),params,0,0));
            }else if(mode==4){
                a.mode=0;DS_OK(dushmemx_collective_launch((const void*)tile_kernel<true>,dim3(a.workers),dim3(P::BlockSize),params,0,0));
                a.mode=1;DS_OK(dushmemx_collective_launch((const void*)tile_kernel<true>,dim3(a.workers),dim3(P::BlockSize),params,0,0));
            }else{
                a.mode=2;DS_OK(dushmemx_collective_launch((const void*)tile_kernel<true>,dim3(a.workers),dim3(P::BlockSize),params,0,0));
            }
            HIP_OK(hipEventRecord(end));HIP_OK(hipEventSynchronize(end));
            float ms,worst;HIP_OK(hipEventElapsedTime(&ms,start,end));
            MPI_Allreduce(&ms,&worst,1,MPI_FLOAT,MPI_MAX,MPI_COMM_WORLD);
            hipLaunchKernelGGL(check,dim3(512),dim3(256),0,0,out,ref,errors,m,n,tag);
            HIP_OK(hipGetLastError());unsigned long long err=0,allerr=0;
            HIP_OK(hipMemcpy(&err,errors,8,hipMemcpyDeviceToHost));
            MPI_Allreduce(&err,&allerr,1,MPI_UNSIGNED_LONG_LONG,MPI_SUM,MPI_COMM_WORLD);
            if(rank==0){printf("{\"m\":%d,\"n\":%d,\"k\":%d,\"mode\":\"%s\",\"workers\":%d,\"round\":%d,\"rep\":%d,\"tag\":%d,\"errors\":%llu,\"maxrank_ms\":%.6f}\n",m,n,k,names[mode],a.workers,round,rep,tag,allerr,worst);fflush(stdout);}
            if(allerr)MPI_Abort(MPI_COMM_WORLD,7);
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);dushmem_barrier_all();
    dushmem_free(a.ready);dushmem_free(a.remote);dushmem_free(a.local);
    HIP_OK(hipFree(x));HIP_OK(hipFree(w));HIP_OK(hipFree(out));HIP_OK(hipFree(ref));HIP_OK(hipFree(errors));
    HIP_OK(hipEventDestroy(start));HIP_OK(hipEventDestroy(end));
}
int main(int argc,char**argv){
    MPI_Init(&argc,&argv);int rank,np;MPI_Comm_rank(MPI_COMM_WORLD,&rank);MPI_Comm_size(MPI_COMM_WORLD,&np);
    if(np!=2||argc<2)MPI_Abort(MPI_COMM_WORLD,1);
    HIP_OK(hipSetDevice(rank));
    MPI_Comm mpi=MPI_COMM_WORLD;dushmemx_init_attr_t attr=DUSHMEMX_INIT_ATTR_INITIALIZER;attr.mpi_comm=&mpi;
    DS_OK(dushmemx_init_attr(DUSHMEMX_INIT_WITH_MPI_COMM,&attr));
    ncclUniqueId id;if(rank==0)NCCL_OK(ncclGetUniqueId(&id));MPI_Bcast(&id,sizeof(id),MPI_BYTE,0,MPI_COMM_WORLD);
    ncclComm_t comm;NCCL_OK(ncclCommInitRank(&comm,np,id,rank));
    void* lib=dlopen(argv[1],RTLD_NOW|RTLD_LOCAL);if(!lib){fprintf(stderr,"%s\n",dlerror());MPI_Abort(mpi,8);}
    GemmFn gemm=(GemmFn)dlsym(lib,"h3_gemm");if(!gemm)MPI_Abort(mpi,9);
    int workers=argc>2?atoi(argv[2]):80,rounds=argc>3?atoi(argv[3]):3;
    bool smoke=argc>4&&std::string(argv[4])=="smoke";
    if(smoke)test(rank,comm,gemm,257,5376,7168,workers,1);
    else {test(rank,comm,gemm,18880,5376,7168,workers,rounds);test(rank,comm,gemm,18880,5376,3584,workers,rounds);}
    NCCL_OK(ncclCommDestroy(comm));dushmem_finalize();dlclose(lib);
    if(rank==0)puts("BENCH_PASS");MPI_Finalize();return 0;
}
