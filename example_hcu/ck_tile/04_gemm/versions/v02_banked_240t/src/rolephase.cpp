// Isolated gfx936 BF16 experiment: 8 compute + 4 producer waves.
// Phase-sized B register prefetch; validated wrap layout from role64 probe.
#include <hip/hip_runtime.h>
#define CK_TILE_FLOAT_TO_BFLOAT16_DEFAULT 0
#include "ck_tile/core.hpp"
#include "ck_tile/ops/gemm/warp/warp_gemm_attribute_mmac_impl.hpp"
#include "ck_tile/core/arch/hcu_buffer_addressing.hpp"
#ifndef H3_PHASE_M_WAVES
#define H3_PHASE_M_WAVES 4
#endif
using BF=ck_tile::bf16_t;
using V8=ck_tile::ext_vector_t<BF,8>;
using MM=ck_tile::WarpGemmAttributeMmacImplBf16Bf16F32M16N16K16;
using Acc=MM::CVecType;
CK_TILE_DEVICE_EXTERN ck_tile::int32x4_t h3_phase_struct_load(ck_tile::int32x4_t res,
    int index,int offset,int soffset,int flags) __asm("llvm.amdgcn.struct.buffer.load.v4i32");
constexpr int MW=H3_PHASE_M_WAVES,NW=8/MW,MT=16/MW,NT=16/NW;
static_assert(MW>0 && 8%MW==0 && 16%MW==0,"Complete wave tile coverage");
__device__ __forceinline__ void ready_ack(){
    asm volatile("s_waitcnt lgkmcnt(0)\n\ts_barrier" ::: "memory");
}
__device__ __forceinline__ int phys(int r,int c){
    const int p=r%4;
    return (r/32)*2048+p*512+(((r%32)/4)*64+c+p*16)%512;
}
__device__ __forceinline__ void stage_weight(BF* shared,ck_tile::int32x4_t res,
                                            int loader,int tile,int k){
    const int wave=loader/64,lane=loader%64;
    #pragma unroll
    for(int j=0;j<8;++j){
        const int row=wave+(lane/8)*4+j*32,col=(lane%8)*8;
        const uintptr_t addr=reinterpret_cast<uintptr_t>(shared)+(tile%2)*32768+wave*1024+j*4096;
        const uintptr_t m0=addr|(uintptr_t(2*wave)<<16);
#if H3_PHASE_CACHE_A
        ck_tile::hcu_async_struct_buffer_load_asm_impl<BF,8>(m0,res,row,(tile*64+col)*2,0);
#else
        ck_tile::hcu_async_buffer_load_asm_impl<BF,8>(m0,res,(row*k+tile*64+col)*2,0);
#endif
    }
}
__device__ __forceinline__ V8 activation(ck_tile::int32x4_t res,int row,int kk,int k){
#if H3_PHASE_ASM_B
    // Async raw load. Only the banked path may use this helper: it explicitly
    // waits for current-bank completion before MMAC consumes any registers.
    static_assert(H3_PHASE_BANK, "Async asm requires explicit banked waits");
    return ck_tile::bit_cast<V8>(ck_tile::amd_buffer_load_impl_inline_asm<BF,8>(res,(row*k+kk)*2,0));
#elif H3_PHASE_CACHE_B
    return ck_tile::bit_cast<V8>(h3_phase_struct_load(res,row,kk*2,0,0));
#else
    return ck_tile::bit_cast<V8>(ck_tile::llvm_amdgcn_raw_buffer_load_i32x4(res,(row*k+kk)*2,0,0));
#endif
}
__global__ __launch_bounds__(768,1)
void h3_ck_rolephase(const BF* weight,const BF* input,BF* output,int features,int tokens,int k){
    __shared__ BF shared[32768];
    const int id=threadIdx.x,mt=(features+255)/256,nt=(tokens+255)/256;
    const int flat=blockIdx.x,span=8*mt,first=(flat/span)*8;
    const int size=min(8,nt-first),local=flat%span;
    const int fm=(local/size)*256,tn=(first+local%size)*256,count=k/64;
    if(id>=512){
        const auto res=ck_tile::make_wave_buffer_resource(weight+fm*k,(features-fm)*k*2
#if H3_PHASE_CACHE_A
                                                        ,k*2
#endif
                                                        );
        stage_weight(shared,res,id-512,0,k);
        if(count>1)stage_weight(shared,res,id-512,1,k);
        asm volatile("s_waitcnt vmcnt(0)" ::: "memory");
        for(int tile=0;tile<count;++tile){
            ready_ack();
            asm volatile("s_waitcnt vmcnt(0)" ::: "memory");
            ready_ack();
            if(tile+2<count)stage_weight(shared,res,id-512,tile+2,k);
        }
    }else{
        const int wave=id/64,lane=id%64,row=lane%16,klane=(lane/16)*8;
        const int wave_m=wave/NW,wave_n=wave%NW;
        const auto res=ck_tile::make_wave_buffer_resource(input+tn*k,(tokens-tn)*k*2
#if H3_PHASE_CACHE_B
                                                        ,k*2
#endif
                                                        );
        Acc accum[NT][MT]={};
#if H3_PHASE_BANK
        V8 bank[2][NT],av[MT];
#else
        V8 current[NT],future[NT],av[MT];
#endif
        #pragma unroll
        for(int j=0;j<NT;++j){
#if H3_PHASE_BANK
            bank[0][j]=activation(res,wave_n*NT*16+row+j*16,klane,k);
#else
            current[j]=activation(res,wave_n*NT*16+row+j*16,klane,k);
#endif
        }
        for(int tile=0;tile<count;++tile){
            ready_ack();
            const int offset=(tile%2)*16384;
            #pragma unroll
            for(int phase=0;phase<2;++phase){
                const int next_phase=tile*2+phase+1;
                if(next_phase<count*2){
                    #pragma unroll
                    for(int j=0;j<NT;++j){
#if H3_PHASE_BANK
                        bank[1-phase][j]=activation(res,wave_n*NT*16+row+j*16,next_phase*32+klane,k);
#else
                        future[j]=activation(res,wave_n*NT*16+row+j*16,next_phase*32+klane,k);
#endif
                    }
                }
                #pragma unroll
                for(int i=0;i<MT;++i)
                    av[i]=*reinterpret_cast<const V8*>(shared+offset+phys(wave_m*MT*16+row+i*16,phase*32+klane));
                if(phase==1)ready_ack(); // last A reads completed before producer reuse
#if H3_PHASE_ASM_B
                // Only this compute wave issues its B loads (NT instructions per
                // phase). Future-bank loads are newer than all current-bank loads.
                // Allow NT outstanding future loads, but wait for all current data;
                // at the terminal phase there are no future loads, so drain to 0.
                if(next_phase<count*2) asm volatile("s_waitcnt vmcnt(%0)" :: "n"(NT) : "memory");
                else asm volatile("s_waitcnt vmcnt(0)" ::: "memory");
                __builtin_amdgcn_sched_barrier(0);
#endif
#if H3_PHASE_PRIORITY
                asm volatile("s_setprio 1" ::: "memory");
#endif
                #pragma unroll
                for(int j=0;j<NT;++j){
                    #pragma unroll
                    for(int i=0;i<MT;++i)MM{}(accum[j][i],av[i].lo,
#if H3_PHASE_BANK
                                             bank[phase][j].lo);
#else
                                             current[j].lo);
#endif
                }
                #pragma unroll
                for(int j=0;j<NT;++j){
                    #pragma unroll
                    for(int i=0;i<MT;++i)MM{}(accum[j][i],av[i].hi,
#if H3_PHASE_BANK
                                             bank[phase][j].hi);
#else
                                             current[j].hi);
#endif
                }
#if H3_PHASE_PRIORITY
                asm volatile("s_setprio 0" ::: "memory");
#endif
#if !H3_PHASE_BANK
                if(next_phase<count*2){
                    #pragma unroll
                    for(int j=0;j<NT;++j)current[j]=future[j];
                }
#endif
            }
        }
        #pragma unroll
        for(int j=0;j<NT;++j){
            #pragma unroll
            for(int i=0;i<MT;++i){
                const int feature=fm+wave_m*MT*16+row+i*16;
                #pragma unroll
                for(int x=0;x<4;++x){
                    const int token=tn+wave_n*NT*16+j*16+lane/16+x*4;
                    if(feature<features && token<tokens)
                        output[token*features+feature]=ck_tile::type_convert<BF>(accum[j][i][x]);
                }
            }
        }
    }
}
extern "C" int h3_gemm(const void* a,const void* w,void* c,int m,int n,int k,hipStream_t stream){
    if(m<=0||n<=0||k<=0||n%256||k%64||
       (reinterpret_cast<uintptr_t>(a)&15)||(reinterpret_cast<uintptr_t>(w)&15))
        return int(hipErrorInvalidValue);
#if H3_PHASE_CACHE_A || H3_PHASE_CACHE_B
    if(k*2>=0x4000)return int(hipErrorInvalidValue);
#endif
    hipLaunchKernelGGL(h3_ck_rolephase,dim3((m+255)/256*(n/256)),dim3(768),0,stream,
        static_cast<const BF*>(w),static_cast<const BF*>(a),static_cast<BF*>(c),n,m,k);
    return int(hipGetLastError());
}
