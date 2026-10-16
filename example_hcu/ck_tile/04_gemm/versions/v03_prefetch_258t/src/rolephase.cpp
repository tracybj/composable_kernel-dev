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
#ifndef H3_PHASE_WEIGHT_K
#define H3_PHASE_WEIGHT_K 64
#endif
#ifndef H3_PHASE_ALWAYS_PREFETCH
#define H3_PHASE_ALWAYS_PREFETCH 0
#endif
using BF=ck_tile::bf16_t;
using V8=ck_tile::ext_vector_t<BF,8>;
using MM=ck_tile::WarpGemmAttributeMmacImplBf16Bf16F32M16N16K16;
using Acc=MM::CVecType;
CK_TILE_DEVICE_EXTERN ck_tile::int32x4_t h3_phase_struct_load(ck_tile::int32x4_t res,
    int index,int offset,int soffset,int flags) __asm("llvm.amdgcn.struct.buffer.load.v4i32");
constexpr int MW=H3_PHASE_M_WAVES,NW=8/MW,MT=16/MW,NT=16/NW;
constexpr int WK=H3_PHASE_WEIGHT_K;
static_assert(WK==32 || WK==64,"Weight layout independently verified only for K32/K64");
static_assert(MW>0 && 8%MW==0 && 16%MW==0,"Complete wave tile coverage");
__device__ __forceinline__ void ready_ack(){
    asm volatile("s_waitcnt lgkmcnt(0)\n\ts_barrier" ::: "memory");
}
__device__ __forceinline__ int phys(int r,int c){
    const int p=r%4;
    return (r/(2048/WK))*2048+p*512+(((r%(2048/WK))/4)*WK+c+p*16)%512;
}
__device__ __forceinline__ void stage_weight(BF* shared,ck_tile::int32x4_t res,
                                            int loader,int tile,int k){
    const int wave=loader/64,lane=loader%64;
    #pragma unroll
    for(int j=0;j<WK/8;++j){
        const int row=wave+(lane/(WK/8))*4+j*(2048/WK),col=(lane%(WK/8))*8;
        const uintptr_t addr=reinterpret_cast<uintptr_t>(shared)+(tile%2)*(256*WK*2)+wave*1024+j*4096;
        const uintptr_t m0=addr|(uintptr_t(2*wave)<<16);
#if H3_PHASE_CACHE_A
        ck_tile::hcu_async_struct_buffer_load_asm_impl<BF,8>(m0,res,row,(tile*WK+col)*2,0);
#else
        ck_tile::hcu_async_buffer_load_asm_impl<BF,8>(m0,res,(row*k+tile*WK+col)*2,0);
#endif
    }
}
__device__ __forceinline__ V8 activation(ck_tile::int32x4_t res,int row,int kk,int k){
#if H3_PHASE_ASM_B
    // Async raw load. Only the banked path may use this helper: it explicitly
    // waits for current-bank completion before MMAC consumes any registers.
    static_assert(H3_PHASE_BANK, "Async asm requires explicit banked waits");
#if H3_PHASE_CACHE_B
    return ck_tile::bit_cast<V8>(ck_tile::amd_buffer_load_impl_inline_asm<BF,8>(res,ck_tile::int32x2_t{row,kk*2}));
#else
    return ck_tile::bit_cast<V8>(ck_tile::amd_buffer_load_impl_inline_asm<BF,8>(res,(row*k+kk)*2,0));
#endif
#elif H3_PHASE_CACHE_B
    return ck_tile::bit_cast<V8>(h3_phase_struct_load(res,row,kk*2,0,0));
#else
    return ck_tile::bit_cast<V8>(ck_tile::llvm_amdgcn_raw_buffer_load_i32x4(res,(row*k+kk)*2,0,0));
#endif
}
__device__ __forceinline__ V8 weight_fragment(const BF* address){
#if H3_PHASE_DS_ASM
    ck_tile::int32x4_t data;
    const unsigned lds_address=static_cast<unsigned>(reinterpret_cast<uintptr_t>(address));
    asm volatile("ds_read_b128 %0, %1" : "=v"(data) : "v"(lds_address) : "memory");
    return ck_tile::bit_cast<V8>(data);
#else
    return *reinterpret_cast<const V8*>(address);
#endif
}
__device__ __forceinline__ void output_fragment(BF value,ck_tile::int32x4_t res,
                                               unsigned voffset,unsigned soffset,int immediate){
    const unsigned short data=ck_tile::bit_cast<unsigned short>(value);
    asm volatile("buffer_store_short %0, %1, %2, %3 offen offset:%4"
                 :: "v"(data),"v"(voffset),"s"(res),"s"(soffset),"n"(immediate) : "memory");
}
__global__ __launch_bounds__(768,1)
void h3_ck_rolephase(const BF* weight,const BF* input,BF* output,int features,int tokens,int k){
#if H3_LIVE_PAD > 0
    unsigned pad0=threadIdx.x^0x123401u;
    asm volatile("" : "+v"(pad0));
#endif
#if H3_LIVE_PAD > 1
    unsigned pad1=threadIdx.x^0x123402u;
    asm volatile("" : "+v"(pad1));
#endif
#if H3_LIVE_PAD > 2
    unsigned pad2=threadIdx.x^0x123403u;
    asm volatile("" : "+v"(pad2));
#endif
    __shared__ BF shared[256*WK*2];
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
        const int weight_count=k/WK;
#if H3_PHASE_READY_WAIT
        // Only current A is required at entry. Issue the next slot after READY;
        // ACK protects all current LDS reads without waiting for future VMEM.
        // Wait for that next slot before the following READY makes it visible.
        asm volatile("s_waitcnt vmcnt(0)" ::: "memory");
        for(int tile=0;tile<weight_count;++tile){
            ready_ack();
            if(tile+1<weight_count)stage_weight(shared,res,id-512,tile+1,k);
            ready_ack();
            asm volatile("s_waitcnt vmcnt(0)" ::: "memory");
        }
#else
        if(weight_count>1)stage_weight(shared,res,id-512,1,k);
        asm volatile("s_waitcnt vmcnt(0)" ::: "memory");
        for(int tile=0;tile<weight_count;++tile){
            ready_ack();
            asm volatile("s_waitcnt vmcnt(0)" ::: "memory");
            ready_ack();
            if(tile+2<weight_count)stage_weight(shared,res,id-512,tile+2,k);
        }
#endif
    }else{
        const int wave=id/64,lane=id%64,row=lane%16,klane=(lane/16)*8;
        const int wave_m=wave/NW,wave_n=wave%NW;
#if H3_PHASE_SWAP
        static_assert(WK==64,"Swapped phases must share one protected K64 A slot");
        const int swap=wave>=4;
#else
        constexpr int swap=0;
#endif
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
#if H3_PHASE_BUFFER_EPILOGUE
        // API requires feature count divisible by 256, so all features in this
        // CTA are valid. A bounded buffer view discards stores for tail tokens.
        const auto output_res=ck_tile::make_wave_buffer_resource(output+tn*features,(tokens-tn)*features*2);
#endif
        #pragma unroll
        for(int j=0;j<NT;++j){
#if H3_PHASE_BANK
            bank[0][j]=activation(res,wave_n*NT*16+row+j*16,swap*32+klane,k);
#else
            current[j]=activation(res,wave_n*NT*16+row+j*16,swap*32+klane,k);
#endif
        }
#if H3_PHASE_UNROLL2
        #pragma unroll 2
#endif
        for(int tile=0;tile<count;++tile){
            if constexpr(WK==64)ready_ack();
            #pragma unroll
            for(int phase=0;phase<2;++phase){
                if constexpr(WK==32)ready_ack();
                const int weight_tile=WK==32 ? tile*2+phase : tile;
                const int offset=(weight_tile%2)*(256*WK);
                const int next_phase=tile*2+phase+1;
                const int next_k=(next_phase/2)*64+((next_phase%2)^swap)*32+klane;
                constexpr bool force_future=H3_PHASE_ALWAYS_PREFETCH;
                const bool has_future=force_future || next_phase<count*2;
                if(has_future
#if H3_PHASE_MID_PREFETCH
                   && !(H3_PHASE_MID_GROUP ? wave>=4 : true)
#endif
                   ){
                    #pragma unroll
                    for(int j=0;j<NT;++j){
#if H3_PHASE_BANK
                        bank[1-phase][j]=activation(res,wave_n*NT*16+row+j*16,next_k,k);
#else
                        future[j]=activation(res,wave_n*NT*16+row+j*16,next_k,k);
#endif
                    }
                }
                #pragma unroll
                for(int i=0;i<MT;++i){
#if H3_PHASE_MID_PREFETCH
                    if(i==MT/2 && has_future && (H3_PHASE_MID_GROUP ? wave>=4 : true)){
                        #pragma unroll
                        for(int j=0;j<NT;++j){
#if H3_PHASE_BANK
                            bank[1-phase][j]=activation(res,wave_n*NT*16+row+j*16,next_k,k);
#else
                            future[j]=activation(res,wave_n*NT*16+row+j*16,next_k,k);
#endif
                        }
                    }
#endif
                    av[i]=weight_fragment(shared+offset+phys(wave_m*MT*16+row+i*16,(WK==64?(phase^swap)*32:0)+klane));
                }
                if(WK==32 || phase==1)ready_ack(); // last A reads completed before producer reuse
#if H3_PHASE_DS_ASM && !H3_PHASE_DS_PROGRESS
                asm volatile("s_waitcnt lgkmcnt(0)" ::: "memory");
                __builtin_amdgcn_sched_barrier(0);
#endif
#if H3_PHASE_ASM_B
                // Only this compute wave issues its B loads (NT instructions per
                // phase). Future-bank loads are newer than all current-bank loads.
                // Allow NT outstanding future loads, but wait for all current data;
                // at the terminal phase there are no future loads, so drain to 0.
                if(has_future) asm volatile("s_waitcnt vmcnt(%0)" :: "n"(NT) : "memory");
                else asm volatile("s_waitcnt vmcnt(0)" ::: "memory");
                __builtin_amdgcn_sched_barrier(0);
#endif
#if H3_PHASE_PRIORITY
                asm volatile("s_setprio 1" ::: "memory");
#endif
                #pragma unroll
                for(int j=0;j<NT;++j){
                    #pragma unroll
                    for(int i=0;i<MT;++i){
#if H3_PHASE_DS_PROGRESS
                        static_assert(MT==16 && H3_PHASE_DS_ASM,"Ordered DS16 required");
                        if(j==0) {
                            asm volatile("s_waitcnt lgkmcnt(%0)" :: "n"(15-i) : "memory");
                            __builtin_amdgcn_sched_barrier(0);
                        }
#endif
                        MM{}(accum[j][i],av[i].lo,
#if H3_PHASE_BANK
                                             bank[phase][j].lo);
#else
                                             current[j].lo);
#endif
#if H3_PHASE_POSTPRIO
                        if(j==0 && i==0)asm volatile("s_setprio 1" ::: "memory");
#endif
                    }
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
#if H3_PHASE_PRIORITY || H3_PHASE_POSTPRIO
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
#if H3_PHASE_ALWAYS_PREFETCH
        static_assert(H3_PHASE_BANK && H3_PHASE_ASM_B && NT==2,"Audited banked B path only");
        // The terminal prefetch is not part of the GEMM. Keep both its vectors
        // live so the allocator cannot reuse their still-pending destinations
        // for A/accumulator registers. Drain before any output/epilogue reuse.
        asm volatile("s_waitcnt vmcnt(0)" ::: "memory");
        __builtin_amdgcn_sched_barrier(0);
        asm volatile("" :: "v"(bank[0][0]),"v"(bank[0][1]) : "memory");
#endif
        #pragma unroll
        for(int j=0;j<NT;++j){
            #pragma unroll
            for(int i=0;i<MT;++i){
                const int feature=fm+wave_m*MT*16+row+i*16;
                #pragma unroll
                for(int x=0;x<4;++x){
                    const int token=tn+wave_n*NT*16+j*16+lane/16+x*4;
#if H3_PHASE_BUFFER_EPILOGUE
                    // Scalar soffset is excluded from buffer bounds checking.
                    // All token displacement must be in the checked voffset,
                    // including x, to protect M=1/63/... partial-token tails.
                    const unsigned voffset=((wave_n*NT*16+j*16+lane/16+x*4)*features+fm+row)*2;
                    output_fragment(ck_tile::type_convert<BF>(accum[j][i][x]),output_res,voffset,0,i*32);
#else
                    if(feature<features && token<tokens)
                        output[token*features+feature]=ck_tile::type_convert<BF>(accum[j][i][x]);
#endif
                }
            }
        }
    }
#if H3_LIVE_PAD == 1
    asm volatile("" :: "v"(pad0) : "memory");
#elif H3_LIVE_PAD == 2
    asm volatile("" :: "v"(pad0),"v"(pad1) : "memory");
#elif H3_LIVE_PAD > 2
    asm volatile("" :: "v"(pad0),"v"(pad1),"v"(pad2) : "memory");
#endif
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
