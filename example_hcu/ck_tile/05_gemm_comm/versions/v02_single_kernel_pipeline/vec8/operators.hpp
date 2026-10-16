// Isolated v08 GEMM + device-side TP2 reduction. No shared CK/model edits.
#pragma once
#include "gemm_kernel.hpp"
#include <dushmem.h>
#include <dushmemx.h>
using namespace ck_tile::h3;
using T=ck_tile::bf16_t;
using P=H3GemmProblem;
using Policy=ProducerPolicy;
using Pipe=H3GemmPipeline<P,Policy>;
struct Args {
    const T *input,*weight;
    T *partial,*peer_partial,*output;
    uint64_t *produced,*finished,*remote_ready,*trace;
    int m,n,k,compute,comm,mode;
    uint64_t epoch;
};
// 0=serial80, 1=phase-separated roles, 2=pipelined roles, 3=compute-only roles.
struct Coord {int feature,token;};
__device__ __attribute__((always_inline)) Coord coord(const Args&a,int tile){
    int ft=a.n/256,nt=(a.m+255)/256,span=P::TokenGroup*ft;
    int first=tile/span*P::TokenGroup,size=min(P::TokenGroup,nt-first),rem=tile%span;
    return {(rem/size)*256,(first+rem%size)*256};
}
template<bool Trace>
__device__ __attribute__((always_inline)) void stamp(const Args&a,int tile,int phase){
    if constexpr(Trace) if(threadIdx.x==0)a.trace[size_t(tile)*6+phase]=uint64_t(wall_clock64());
}
__device__ __attribute__((always_inline)) void wait_local(uint64_t*flag,uint64_t epoch){
    if(threadIdx.x==0)
        while(__hip_atomic_load(flag,__ATOMIC_ACQUIRE,__HIP_MEMORY_SCOPE_AGENT)<epoch)
            __builtin_amdgcn_s_sleep(1);
    __syncthreads();
}
__device__ __attribute__((always_inline)) void publish(uint64_t*flag,uint64_t epoch){
    // All data writers drain before lane0 publishes the generation.
    __threadfence_system();__syncthreads();
    if(threadIdx.x==0)__hip_atomic_store(flag,epoch,__ATOMIC_RELEASE,__HIP_MEMORY_SCOPE_AGENT);
    __syncthreads();
}
template<bool Trace>
__device__ __attribute__((always_inline)) void compute_tile(const Args&a,int tile,T*shared){
    stamp<Trace>(a,tile,0);
    auto c=coord(a,tile);int id=threadIdx.x;
    if(id>=P::ComputeWaves*64){
        auto resource=ck_tile::make_wave_buffer_resource(a.weight+c.feature*a.k,(a.n-c.feature)*a.k*2);
        Pipe::RunProducer(shared,resource,id-P::ComputeWaves*64,a.k/P::WeightK,a.k);
    }else{
        int wave=id/64,lane=id%64,row=lane%16,klane=(lane/16)*8;
        int wave_m=wave/P::TokenWaves,wave_n=wave%P::TokenWaves;
        auto resource=Policy::ActivationAccess::MakeResource(a.input+c.token*a.k,a.m-c.token,a.k);
        P::AccVector accum[P::TokenFrags][P::FeatureFrags]={};
        Pipe::RunConsumer(shared,resource,accum,a.k/P::WeightK,a.k,wave,wave_m,wave_n,row,klane);
        auto dst=ck_tile::make_wave_buffer_resource(a.partial+size_t(c.token)*a.n,(a.m-c.token)*a.n*2);
        H3GemmEpilogue<P>{}(accum,dst,a.n,c.feature,wave_n,row,lane);
    }
    __threadfence();__syncthreads();
    stamp<Trace>(a,tile,1);
}
template<bool Trace>
__device__ __attribute__((always_inline)) void communicate_tile(const Args&a,int tile){
    stamp<Trace>(a,tile,3);
    // Produced flag was acquired before this function. The producer's system
    // fence covers every partial-output store before notifying the peer.
    if(threadIdx.x==0){
        dushmemx_signal_op(a.remote_ready+tile,a.epoch,DUSHMEM_SIGNAL_SET,1-dushmem_my_pe());
        dushmem_quiet();
        dushmem_signal_wait_until(a.remote_ready+tile,DUSHMEM_CMP_GE,a.epoch);
    }
    __syncthreads();stamp<Trace>(a,tile,4);
    auto c=coord(a,tile);
    // Eight adjacent BF16 elements per lane: aligned 16-byte local/peer reads
    // and output writes. The BF16 sum rounding is identical to the scalar path.
    using V=ck_tile::ext_vector_t<T,8>;
    for(int v=threadIdx.x;v<256*256/8;v+=blockDim.x){
        int row=c.token+v/32,col=c.feature+(v%32)*8;
        if(row<a.m){
            size_t pos=size_t(row)*a.n+col;
            V local=*reinterpret_cast<const V*>(a.partial+pos);
            V peer=*reinterpret_cast<const V*>(a.peer_partial+pos),out;
            #pragma unroll
            for(int j=0;j<8;++j){
                float sum=ck_tile::type_convert<float>(local[j])+ck_tile::type_convert<float>(peer[j]);
                out[j]=ck_tile::bit_cast<T>(gfx936::ConvertBf16Rne(sum));
            }
            *reinterpret_cast<V*>(a.output+pos)=out;
        }
    }
    __threadfence();__syncthreads();stamp<Trace>(a,tile,5);
}
template<bool Trace>
__global__ __launch_bounds__(P::BlockSize,1) void role_kernel(Args a){
    // Static LDS belongs to every CTA, including communication-only CTAs.
    __shared__ T shared[P::LdsElements];
    int total=((a.m+255)/256)*(a.n/256);
    if(a.mode==0){
        for(int tile=blockIdx.x;tile<total;tile+=80){
            compute_tile<Trace>(a,tile,shared);
            publish(a.produced+tile,a.epoch);stamp<Trace>(a,tile,2);
            communicate_tile<Trace>(a,tile);
        }
    }else if(blockIdx.x<a.compute){
        for(int tile=blockIdx.x;tile<total;tile+=a.compute){
            compute_tile<Trace>(a,tile,shared);
            publish(a.produced+tile,a.epoch);stamp<Trace>(a,tile,2);
        }
        publish(a.finished+blockIdx.x,a.epoch);
    }else if(a.mode!=3){
        if(a.mode==1)
            for(int producer=0;producer<a.compute;++producer)wait_local(a.finished+producer,a.epoch);
        for(int tile=int(blockIdx.x)-a.compute;tile<total;tile+=a.comm){
            wait_local(a.produced+tile,a.epoch);
            communicate_tile<Trace>(a,tile);
        }
    }
}
