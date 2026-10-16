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
#ifndef H3_PHASE_FEATURE_TILE
#define H3_PHASE_FEATURE_TILE 256
#endif
#ifndef H3_PHASE_COMPUTE_WAVES
#define H3_PHASE_COMPUTE_WAVES 8
#endif
#ifndef H3_PHASE_PRODUCER_WAVES
#define H3_PHASE_PRODUCER_WAVES 4
#endif
#ifndef H3_PHASE_DS_PROGRESS
#define H3_PHASE_DS_PROGRESS 0
#endif
#ifndef H3_PHASE_WEIGHT_K
#define H3_PHASE_WEIGHT_K 64
#endif
#ifndef H3_PHASE_ALWAYS_PREFETCH
#define H3_PHASE_ALWAYS_PREFETCH 0
#endif
#ifndef H3_PHASE_MID_POINT
#define H3_PHASE_MID_POINT 8
#endif
#ifndef H3_PHASE_MID_SPLIT
#define H3_PHASE_MID_SPLIT 0
#endif
#ifndef H3_PHASE_BURST
#define H3_PHASE_BURST 0
#endif
#ifndef H3_PHASE_MATH_FENCE
#define H3_PHASE_MATH_FENCE 0
#endif
#ifndef H3_PHASE_B64
#define H3_PHASE_B64 0
#endif
#ifndef H3_PHASE_B64_PREFETCH_PHASE
#define H3_PHASE_B64_PREFETCH_PHASE 0
#endif
#ifndef H3_PHASE_SWAP
#define H3_PHASE_SWAP 0
#endif
#ifndef H3_PHASE_OUTPUT_RNE
#define H3_PHASE_OUTPUT_RNE 0
#endif
#ifndef H3_PHASE_NATIVE_CADENCE
#define H3_PHASE_NATIVE_CADENCE 0
#endif
#ifndef H3_PHASE_POW2_B
#define H3_PHASE_POW2_B 0
#endif
#ifndef H3_PHASE_B_BIAS
#define H3_PHASE_B_BIAS 0
#endif
#ifndef H3_PHASE_FINAL_VM_WAIT
#define H3_PHASE_FINAL_VM_WAIT 0
#endif
#ifndef H3_PHASE_SCALAR_A_BASE
#define H3_PHASE_SCALAR_A_BASE 0
#endif
#ifndef H3_PHASE_SCALAR_B_BASE
#define H3_PHASE_SCALAR_B_BASE 0
#endif
#ifndef H3_PHASE_B_SOFFSET
#define H3_PHASE_B_SOFFSET 0
#endif
#ifndef H3_PHASE_A_PHASE1_PREFETCH
#define H3_PHASE_A_PHASE1_PREFETCH 0
#endif
#ifndef H3_PHASE_A_PHASE1_BEGIN
#define H3_PHASE_A_PHASE1_BEGIN 0
#endif
#ifndef H3_PHASE_A_PHASE1_COUNT
#define H3_PHASE_A_PHASE1_COUNT 16
#endif
#ifndef H3_PHASE_SCALAR_WAVE
#define H3_PHASE_SCALAR_WAVE 0
#endif
#ifndef H3_PHASE_CACHE_A
#define H3_PHASE_CACHE_A 0
#endif
#ifndef H3_PHASE_CACHE_B
#define H3_PHASE_CACHE_B 0
#endif
#ifndef H3_PHASE_STAGGER
#define H3_PHASE_STAGGER 0
#endif
#ifndef H3_PHASE_STATIC_STAGGER
#define H3_PHASE_STATIC_STAGGER 0
#endif
#ifndef H3_PHASE_STAGGER_FIRST_WAVE
#define H3_PHASE_STAGGER_FIRST_WAVE 4
#endif
#ifndef H3_PHASE_GROUP_PREFETCH
#define H3_PHASE_GROUP_PREFETCH 0
#endif
#ifndef H3_PHASE_EARLY_PREFETCH_POINT
#define H3_PHASE_EARLY_PREFETCH_POINT 8
#endif
#ifndef H3_PHASE_LATE_PREFETCH_POINT
#define H3_PHASE_LATE_PREFETCH_POINT 8
#endif
#ifndef H3_PHASE_BUILTIN_LGKM
#define H3_PHASE_BUILTIN_LGKM 0
#endif
#ifndef H3_PHASE_ACC_PACK
#define H3_PHASE_ACC_PACK 0
#endif
#ifndef H3_PHASE_ACC_WIDTH
#define H3_PHASE_ACC_WIDTH 128
#endif
#ifndef H3_PHASE_POSTPRIO_VALUE
#define H3_PHASE_POSTPRIO_VALUE 1
#endif
#ifndef H3_PHASE_RESERVE_LOW
#define H3_PHASE_RESERVE_LOW 0
#endif
#ifndef H3_PHASE_NATIVE_B64
#define H3_PHASE_NATIVE_B64 0
#endif
#ifndef H3_PHASE_NATIVE_B64_POINT
#define H3_PHASE_NATIVE_B64_POINT 0
#endif
#ifndef H3_PHASE_NATIVE_B64_LATE_WAIT
#define H3_PHASE_NATIVE_B64_LATE_WAIT 0
#endif
#ifndef H3_PHASE_NATIVE_B64_PREFETCH_POINT
#define H3_PHASE_NATIVE_B64_PREFETCH_POINT -1
#endif
#ifndef H3_PHASE_FIXED_SHAPE
#define H3_PHASE_FIXED_SHAPE 0
#endif
#ifndef H3_PHASE_MMAC_GROUP4
#define H3_PHASE_MMAC_GROUP4 0
#endif
#ifndef H3_PHASE_A_TWO_BASE
#define H3_PHASE_A_TWO_BASE 0
#endif
#ifndef H3_PHASE_A_READ_WAIT
#define H3_PHASE_A_READ_WAIT 0
#endif
#ifndef H3_PHASE_VM_AFTER_BARRIER
#define H3_PHASE_VM_AFTER_BARRIER 0
#endif
#ifndef H3_PHASE_NATIVE_B64_END_WAIT
#define H3_PHASE_NATIVE_B64_END_WAIT 0
#endif
#ifndef H3_PHASE_MMAC_CHUNK
#define H3_PHASE_MMAC_CHUNK 0
#endif
#ifndef H3_PHASE_A_ALWAYS_PREFETCH
#define H3_PHASE_A_ALWAYS_PREFETCH 0
#endif
#ifndef H3_PHASE_PRODUCER_PAIRS
#define H3_PHASE_PRODUCER_PAIRS 0
#endif
#ifndef H3_PHASE_A_SOFFSET
#define H3_PHASE_A_SOFFSET 0
#endif
#ifndef H3_PHASE_NATIVE_B64_BASE64
#define H3_PHASE_NATIVE_B64_BASE64 0
#endif
#ifndef H3_PHASE_STRUCT_B64
#define H3_PHASE_STRUCT_B64 0
#endif
#ifndef H3_PHASE_STRUCT_B32
#define H3_PHASE_STRUCT_B32 0
#endif
#ifndef H3_PHASE_NATIVE_B64_HOIST
#define H3_PHASE_NATIVE_B64_HOIST 0
#endif
#ifndef H3_PHASE_NATIVE_B64_PHASE_WAIT
#define H3_PHASE_NATIVE_B64_PHASE_WAIT 0
#endif
#ifndef H3_PHASE_NATIVE_B64_ROW_ORDER
#define H3_PHASE_NATIVE_B64_ROW_ORDER 0
#endif
#ifndef H3_PHASE_PAIRED_OUTPUT
#define H3_PHASE_PAIRED_OUTPUT 0
#endif
#ifndef H3_PHASE_PAIR_TILES
#define H3_PHASE_PAIR_TILES 0
#endif
#ifndef H3_PHASE_A_BURST
#define H3_PHASE_A_BURST 0
#endif
#ifndef H3_PHASE_GRID_2D
#define H3_PHASE_GRID_2D 0
#endif
#ifndef H3_PHASE_TOKEN_GROUP
#define H3_PHASE_TOKEN_GROUP 8
#endif
#if H3_PHASE_POW2_B
#include "structured_b.hpp"
#endif
#if H3_PHASE_OUTPUT_RNE
#include "output_rne.hpp"
#endif
#if H3_PHASE_A_SOFFSET
#include "async_a_soffset.hpp"
#endif
#if H3_PHASE_B_SOFFSET
#include "async_b_soffset.hpp"
#endif
#if H3_PHASE_NATIVE_B64_BASE64
#if H3_PHASE_STRUCT_B64
#if !H3_PHASE_NATIVE_B64 || H3_PHASE_NATIVE_B64_ROW_ORDER != 2 || H3_PHASE_NATIVE_B64_HOIST || H3_PHASE_POW2_B || H3_PHASE_SCALAR_B_BASE || H3_PHASE_CACHE_B
#error "Structured K64 supports only paired-cache private row-major immediate banks"
#endif
#include "structured_b64.hpp"
#else
#include "raw_b_base64.hpp"
#endif
#endif
#if H3_PHASE_STRUCT_B32
#if H3_PHASE_NATIVE_B64 || H3_PHASE_B64 || H3_PHASE_STRUCT_B64 || H3_PHASE_SCALAR_B_BASE || H3_PHASE_POW2_B || H3_PHASE_CACHE_B || H3_PHASE_B_SOFFSET || H3_PHASE_VM_AFTER_BARRIER
#error "Structured K32 is isolated from other B addressing/wait transforms"
#endif
#include "structured_b32.hpp"
#endif
#if H3_PHASE_PAIRED_OUTPUT
#include "paired_output.hpp"
#endif
using BF=ck_tile::bf16_t;
using V8=ck_tile::ext_vector_t<BF,8>;
using MM=ck_tile::WarpGemmAttributeMmacImplBf16Bf16F32M16N16K16;
using Acc=MM::CVecType;
#if H3_PHASE_MMAC_GROUP4
#include "mmac_group4.hpp"
#endif
CK_TILE_DEVICE_EXTERN ck_tile::int32x4_t h3_phase_struct_load(ck_tile::int32x4_t res,
    int index,int offset,int soffset,int flags) __asm("llvm.amdgcn.struct.buffer.load.v4i32");
constexpr int FW=H3_PHASE_FEATURE_TILE;
constexpr int CW=H3_PHASE_COMPUTE_WAVES,PW=H3_PHASE_PRODUCER_WAVES,THREADS=(CW+PW)*64;
constexpr int MW=H3_PHASE_M_WAVES,NW=CW/MW,MT=(FW/16)/MW,NT=16/NW;
static_assert((CW==4 || CW==8) && (PW==2 || PW==4),"Tested wave-count domain only");
#if H3_PHASE_COMPUTE_WAVES != 8 || H3_PHASE_PRODUCER_WAVES != 4
static_assert(((FW==128 && CW==4 && NT==4 && MT==8) ||
               (FW==256 && CW==8 && NT==2 && MT==16)) && PW==2 && MW==1 &&
              H3_PHASE_NATIVE_CADENCE && H3_PHASE_PAIR_TILES && H3_PHASE_STATIC_STAGGER,
              "Reduced producer layout is isolated to paired K32 bank cadence");
#endif
static_assert(FW==128 || FW==256,"Only complete 128/256 feature tiles");
#if H3_PHASE_FEATURE_TILE != 256
static_assert(MW==1 && H3_PHASE_NATIVE_CADENCE && H3_PHASE_PAIR_TILES &&
              H3_PHASE_STATIC_STAGGER && H3_PHASE_BANK && H3_PHASE_ASM_B &&
              !H3_PHASE_NATIVE_B64 && !H3_PHASE_A_BURST && !H3_PHASE_B64 &&
              !H3_PHASE_GROUP_PREFETCH && !H3_PHASE_A_TWO_BASE &&
              !H3_PHASE_MMAC_CHUNK && !H3_PHASE_MMAC_GROUP4 && !H3_PHASE_DS_PROGRESS,
              "Reduced feature tile uses only the audited K32 bank cadence");
static_assert(H3_PHASE_MID_POINT>0 && H3_PHASE_MID_POINT<MT,"Prefetch inside shortened A phase");
#endif
constexpr int WK=H3_PHASE_WEIGHT_K;
#if H3_PHASE_A_PHASE1_PREFETCH
static_assert(H3_PHASE_A_PHASE1_BEGIN>=0 && H3_PHASE_A_PHASE1_COUNT>0 &&
              H3_PHASE_A_PHASE1_BEGIN+H3_PHASE_A_PHASE1_COUNT<=MT &&
              H3_PHASE_A_PHASE1_BEGIN%H3_PHASE_A_PHASE1_PREFETCH==0 &&
              H3_PHASE_A_PHASE1_COUNT%H3_PHASE_A_PHASE1_PREFETCH==0,
              "Phase1 stream window is complete aligned groups within the A tile");
static_assert((H3_PHASE_A_PHASE1_PREFETCH==1 || H3_PHASE_A_PHASE1_PREFETCH==4) &&
              FW==256 && CW==8 && PW==4 && MW==1 && MT==16 && NT==2 && WK==64 &&
              H3_PHASE_NATIVE_CADENCE && H3_PHASE_STATIC_STAGGER && H3_PHASE_PAIR_TILES,
              "Phase1 A streaming reuses the audited original-tile K32-bank cadence");
#if H3_PHASE_A_TWO_BASE || H3_PHASE_DS_ASM || H3_PHASE_MMAC_CHUNK || H3_PHASE_MMAC_GROUP4 || H3_PHASE_NATIVE_B64 || H3_PHASE_B64 || H3_PHASE_SWAP
#error "Phase1 A stream does not compose with other experimental load/math rearrangements"
#endif
#endif
static_assert(WK==32 || WK==64,"Weight layout independently verified only for K32/K64");
static_assert(MW>0 && CW%MW==0 && 16%MW==0,"Complete wave tile coverage");
static_assert(H3_PHASE_MID_POINT>0 && H3_PHASE_MID_POINT<16,"Prefetch must lie within DS16 sequence");
#if H3_PHASE_NATIVE_CADENCE
static_assert(WK==64 && MW==1 && !H3_PHASE_B64 && !H3_PHASE_SWAP,
              "Two-barrier cadence audited only for unremapped K64 A/K32 B");
#endif
__device__ __forceinline__ void ready_ack(){
    asm volatile("s_waitcnt lgkmcnt(0)\n\ts_barrier" ::: "memory");
}
__device__ __forceinline__ int phys(int r,int c){
    const int p=r%4;
    return (r/(2048/WK))*2048+p*512+(((r%(2048/WK))/4)*WK+c+p*16)%512;
}
template<int FixedSlot=-1>
__device__ __forceinline__ void stage_weight_raw(BF* shared,ck_tile::int32x4_t res,
                                            int loader,int tile,int k){
    static_assert(FixedSlot>=-1 && FixedSlot<=1,"Two protected shared-memory slots only");
    const int shared_slot=FixedSlot<0 ? tile%2 : FixedSlot;
#if H3_PHASE_A_SOFFSET
    static_assert(WK==64 && !H3_PHASE_SCALAR_A_BASE && !H3_PHASE_CACHE_A &&
                  !H3_PHASE_A_BURST && !H3_PHASE_A_ALWAYS_PREFETCH,
                  "Scalar offset requires scheduled valid K tiles and full feature tiles");
    const unsigned common=__builtin_amdgcn_readfirstlane(static_cast<unsigned>(tile*WK*2));
#endif
#if H3_PHASE_SCALAR_WAVE
    const int wave=__builtin_amdgcn_readfirstlane(loader)/64,lane=loader%64;
#else
    const int wave=loader/64,lane=loader%64;
#endif
#if H3_PHASE_SCALAR_A_BASE
    static_assert(!H3_PHASE_CACHE_A,"Scalar address shift supports raw A resource only");
    const unsigned delta=tile*WK*2;
    const uint64_t pointer=ck_tile::bit_cast<uint64_t>(res.lo)+delta;
    res.lo=ck_tile::bit_cast<ck_tile::int32x2_t>(pointer);
    res[2]-=delta; // same physical end address, not a widened resource bound
#endif
#if H3_PHASE_A_BURST
    static_assert(WK==64 && !H3_PHASE_CACHE_A,"Burst supports validated raw K64 layout");
    int offsets[8];
    #pragma unroll
    for(int j=0;j<8;++j){
        const int row=wave+(lane/8)*4+j*32,col=(lane%8)*8;
#if H3_PHASE_SCALAR_A_BASE
        offsets[j]=(row*k+col)*2;
#else
        offsets[j]=(row*k+tile*WK+col)*2;
#endif
    }
    const uintptr_t first_addr=reinterpret_cast<uintptr_t>(shared)+shared_slot*(FW*WK*2)+wave*1024;
    const unsigned first_m0=__builtin_amdgcn_readfirstlane(
        static_cast<unsigned>(first_addr|(uintptr_t(2*wave)<<16)));
    // Capture all eight offsets and the resource once. m0 increments stay in
    // the scalar register file; no repeated VGPR->SGPR readfirstlane per load.
    asm volatile(
        "s_mov_b32 m0, %0\n\t"
        "buffer_load_dwordx4 %1, %9, 0 offen lds\n\t"
        "s_add_u32 m0, m0, 0x1000\n\t"
        "buffer_load_dwordx4 %2, %9, 0 offen lds\n\t"
        "s_add_u32 m0, m0, 0x1000\n\t"
        "buffer_load_dwordx4 %3, %9, 0 offen lds\n\t"
        "s_add_u32 m0, m0, 0x1000\n\t"
        "buffer_load_dwordx4 %4, %9, 0 offen lds\n\t"
        "s_add_u32 m0, m0, 0x1000\n\t"
        "buffer_load_dwordx4 %5, %9, 0 offen lds\n\t"
        "s_add_u32 m0, m0, 0x1000\n\t"
        "buffer_load_dwordx4 %6, %9, 0 offen lds\n\t"
        "s_add_u32 m0, m0, 0x1000\n\t"
        "buffer_load_dwordx4 %7, %9, 0 offen lds\n\t"
        "s_add_u32 m0, m0, 0x1000\n\t"
        "buffer_load_dwordx4 %8, %9, 0 offen lds\n\t"
        :: "s"(first_m0),"v"(offsets[0]),"v"(offsets[1]),"v"(offsets[2]),"v"(offsets[3]),
           "v"(offsets[4]),"v"(offsets[5]),"v"(offsets[6]),"v"(offsets[7]),"s"(res)
        : "memory","m0","scc");
#else
    #pragma unroll
    for(int j=0;j<(WK/8)*(FW/128)/2;++j){
        const int row=wave+(lane/(WK/8))*4+j*(2048/WK),col=(lane%(WK/8))*8;
        const uintptr_t addr=reinterpret_cast<uintptr_t>(shared)+shared_slot*(FW*WK*2)+wave*1024+j*4096;
        const uintptr_t m0=addr|(uintptr_t(2*wave)<<16);
#if H3_PHASE_A_SOFFSET
        h3_a_load_soffset(m0,res,(row*k+col)*2,common);
#elif H3_PHASE_CACHE_A
        ck_tile::hcu_async_struct_buffer_load_asm_impl<BF,8>(m0,res,row,(tile*WK+col)*2,0);
#else
#if H3_PHASE_SCALAR_A_BASE
        ck_tile::hcu_async_buffer_load_asm_impl<BF,8>(m0,res,(row*k+col)*2,0);
#else
        ck_tile::hcu_async_buffer_load_asm_impl<BF,8>(m0,res,(row*k+tile*WK+col)*2,0);
#endif
#endif
    }
#endif
}
template<int FixedSlot=-1>
__device__ __forceinline__ void stage_weight(BF* shared,ck_tile::int32x4_t res,
                                            int loader,int tile,int k){
    // Four logical row/swizzle groups are unchanged. With two physical
    // producer waves each wave emits its group and the group two lanesets on.
    // Every async load is protected by the original VM wait + matched barrier.
    stage_weight_raw<FixedSlot>(shared,res,loader,tile,k);
#if H3_PHASE_PRODUCER_WAVES == 2
    stage_weight_raw<FixedSlot>(shared,res,loader+128,tile,k);
#endif
}
__device__ __forceinline__ V8 activation(ck_tile::int32x4_t res,int row,int kk,int k){
#if H3_PHASE_STRUCT_B32
    static_assert(H3_PHASE_BANK && H3_PHASE_ASM_B && H3_PHASE_NATIVE_CADENCE &&
                  H3_PHASE_PAIR_TILES && H3_PHASE_STATIC_STAGGER,
                  "Structured K32 uses only the audited two-bank static cadence");
    return ck_tile::bit_cast<V8>(h3_struct_b32_load(res,row,kk,k));
#elif H3_PHASE_POW2_B
    static_assert(H3_PHASE_ASM_B && H3_PHASE_BANK,"Audited banked async path required");
    return ck_tile::bit_cast<V8>(h3_b_load(res,row,kk,k,H3_PHASE_B_BIAS));
#elif H3_PHASE_ASM_B
    // Async raw load. Only the banked path may use this helper: it explicitly
    // waits for current-bank completion before MMAC consumes any registers.
    static_assert(H3_PHASE_BANK, "Async asm requires explicit banked waits");
#if H3_PHASE_CACHE_B
    return ck_tile::bit_cast<V8>(ck_tile::amd_buffer_load_impl_inline_asm<BF,8>(res,ck_tile::int32x2_t{row,kk*2}));
#elif H3_PHASE_B_SOFFSET
    static_assert(!H3_PHASE_NATIVE_B64 && !H3_PHASE_SCALAR_B_BASE && !H3_PHASE_POW2_B,
                  "B soffset keeps raw token bounds and only K32 bank phases");
    return ck_tile::bit_cast<V8>(h3_b_load_soffset(res,static_cast<unsigned>(row),kk,k));
#else
#if H3_PHASE_SCALAR_B_BASE
    // Common K displacement is uniform over a full compute wave. Rebase the
    // raw resource and shrink its bound by the same amount; the physical end
    // remains unchanged, including the deliberately unused terminal prefetch.
    const unsigned common=__builtin_amdgcn_readfirstlane(kk&~31)*2;
    const uint64_t pointer=ck_tile::bit_cast<uint64_t>(res.lo)+common;
    res.lo=ck_tile::bit_cast<ck_tile::int32x2_t>(pointer);
#if H3_PHASE_NATIVE_B64
    // The unused future K64 bank also includes K+32. For a one-row tail,
    // that displacement can exceed the resource extent; never underflow it.
    const unsigned bound=__builtin_amdgcn_readfirstlane(static_cast<unsigned>(res[2]));
    res[2]=common<bound ? bound-common : 0;
    // CK's generic 'r' operand accepted a vector descriptor after the clamp.
    // Every resource field is CTA-uniform; make that fact explicit and use
    // the instruction's required SGPR resource class.
    #pragma unroll
    for(int field=0;field<4;++field)res[field]=__builtin_amdgcn_readfirstlane(res[field]);
    ck_tile::int32x4_t data;
    const unsigned offset=(row*k+(kk&31))*2;
    asm volatile("buffer_load_dwordx4 %0, %1, %2, 0 offen"
                 : "=v"(data) : "v"(offset),"s"(res) : "memory");
    return ck_tile::bit_cast<V8>(data);
#else
    res[2]-=common;
    return ck_tile::bit_cast<V8>(ck_tile::amd_buffer_load_impl_inline_asm<BF,8>(res,(row*k+(kk&31))*2,0));
#endif
#else
    return ck_tile::bit_cast<V8>(ck_tile::amd_buffer_load_impl_inline_asm<BF,8>(res,(row*k+kk)*2,0));
#endif
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
template<int Offset>
__device__ __forceinline__ V8 weight_fragment_immediate(unsigned address){
    static_assert(Offset>=0 && Offset<=65535,"DS byte offset must fit its immediate");
    ck_tile::int32x4_t data;
    asm volatile("ds_read_b128 %0, %1 offset:%2"
                 : "=&v"(data) : "v"(address),"n"(Offset) : "memory");
    return ck_tile::bit_cast<V8>(data);
}
template<int Offset>
__device__ __forceinline__ void weight_fragment_pair(V8& even,V8& odd,
                                                    unsigned even_address,unsigned odd_address){
    static_assert(Offset>=0 && Offset<=65535,"DS byte offset must fit its immediate");
    ck_tile::int32x4_t x,y;
    asm volatile("ds_read_b128 %0, %2 offset:%4\n\t"
                 "ds_read_b128 %1, %3 offset:%4"
                 : "=&v"(x),"=&v"(y) : "v"(even_address),"v"(odd_address),"n"(Offset) : "memory");
    even=ck_tile::bit_cast<V8>(x);odd=ck_tile::bit_cast<V8>(y);
}
__device__ __forceinline__ void constrain_accumulator_bank(Acc (&accum)[NT][MT]){
    constexpr int width=H3_PHASE_ACC_WIDTH<NT*MT*4 ? H3_PHASE_ACC_WIDTH : NT*MT*4;
    static_assert((NT*MT*4)%width==0,"Complete accumulator coverage");
    using Bank=ck_tile::ext_vector_t<float,width>;
    #pragma unroll
    for(int group=0;group<(NT*MT*4)/width;++group){
        Bank packed;
        #pragma unroll
        for(int x=0;x<width;++x){
            const int flat=group*width+x;
            packed[x]=accum[flat/(MT*4)][(flat/4)%MT][flat%4];
        }
        // A real compiler register constraint; inspect emitted moves and
        // register placement rather than assuming this expresses free packing.
        asm volatile("" : "+v"(packed));
        #pragma unroll
        for(int x=0;x<width;++x){
            const int flat=group*width+x;
            accum[flat/(MT*4)][(flat/4)%MT][flat%4]=packed[x];
        }
    }
}
__device__ __forceinline__ void constrain_low_registers(){
    // Conservative compiler clobbers at live-accumulator boundaries. No
    // machine instruction writes these registers; the allocator must preserve
    // any live value elsewhere. Actual alignment/moves need ISA inspection.
#if H3_PHASE_RESERVE_LOW == 1
    asm volatile("" ::: "v0");
#elif H3_PHASE_RESERVE_LOW == 2
    asm volatile("" ::: "v0","v1");
#elif H3_PHASE_RESERVE_LOW == 3
    asm volatile("" ::: "v0","v1","v2");
#endif
}
__device__ __forceinline__ void output_fragment(BF value,ck_tile::int32x4_t res,
                                               unsigned voffset,unsigned soffset,int immediate){
    const unsigned short data=ck_tile::bit_cast<unsigned short>(value);
    asm volatile("buffer_store_short %0, %1, %2, %3 offen offset:%4"
                 :: "v"(data),"v"(voffset),"s"(res),"s"(soffset),"n"(immediate) : "memory");
}
#if H3_PHASE_FIXED_SHAPE
template<int FixedFeatures,int FixedK>
__global__ __launch_bounds__(THREADS,1)
void h3_ck_rolephase(const BF* weight,const BF* input,BF* output,int runtime_features,int tokens,int runtime_k){
    const int features=FixedFeatures ? FixedFeatures : runtime_features;
    const int k=FixedK ? FixedK : runtime_k;
#else
__global__ __launch_bounds__(THREADS,1)
void h3_ck_rolephase(const BF* weight,const BF* input,BF* output,int features,int tokens,int k){
#endif
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
    __shared__ BF shared[FW*WK*2];
    const int id=threadIdx.x,mt=(features+FW-1)/FW,nt=(tokens+255)/256;
#if H3_PHASE_GRID_2D
    const int flat=blockIdx.x+blockIdx.y*mt;
#else
    const int flat=blockIdx.x;
#endif
    const int span=H3_PHASE_TOKEN_GROUP*mt,first=(flat/span)*H3_PHASE_TOKEN_GROUP;
    const int size=min(H3_PHASE_TOKEN_GROUP,nt-first),local=flat%span;
    const int fm=(local/size)*FW,tn=(first+local%size)*256,count=k/64;
#if H3_PHASE_SCALAR_WAVE
    const int uniform_wave=__builtin_amdgcn_readfirstlane(id)/64;
    if(uniform_wave>=8){
#else
    if(id>=CW*64){
#endif
        const auto res=ck_tile::make_wave_buffer_resource(weight+fm*k,(features-fm)*k*2
#if H3_PHASE_CACHE_A
                                                        ,k*2
#endif
                                                        );
        stage_weight(shared,res,id-CW*64,0,k);
        const int weight_count=k/WK;
#if H3_PHASE_NATIVE_CADENCE
        // Startup: producer A0 and consumer B0 complete independently before
        // the second matched barrier. Subsequent A prefetch spans both phases.
        asm volatile("s_barrier\n\ts_waitcnt vmcnt(0)\n\ts_barrier" ::: "memory");
#if H3_PHASE_PRODUCER_PAIRS
        static_assert(WK==64 && H3_PHASE_PAIR_TILES && !H3_PHASE_A_ALWAYS_PREFETCH,
                      "Producer pairs must retain the matching paired consumer/barriers");
        for(int tile=0;tile<weight_count;tile+=2){
            if(tile+1<weight_count){
#if H3_PHASE_PRODUCER_PAIRS == 1
                stage_weight<1>(shared,res,id-CW*64,tile+1,k);
#else
                stage_weight(shared,res,id-CW*64,tile+1,k);
#endif
            }
            asm volatile("s_barrier\n\ts_waitcnt vmcnt(0)\n\ts_barrier" ::: "memory");
            // The odd terminal tile has exactly one consume call and one
            // producer handshake. Never issue an unmatched second handshake.
            if(tile+1<weight_count){
                if(tile+2<weight_count){
#if H3_PHASE_PRODUCER_PAIRS == 1
                    stage_weight<0>(shared,res,id-CW*64,tile+2,k);
#else
                    stage_weight(shared,res,id-CW*64,tile+2,k);
#endif
                }
                asm volatile("s_barrier\n\ts_waitcnt vmcnt(0)\n\ts_barrier" ::: "memory");
            }
        }
#else
#if H3_PHASE_FIXED_SHAPE
        #pragma unroll 1
#endif
        for(int tile=0;tile<weight_count;++tile){
#if H3_PHASE_A_ALWAYS_PREFETCH
            static_assert(WK==64 && H3_PHASE_NATIVE_CADENCE,
                          "Producer branch experiment keeps the audited two-barrier cadence");
            auto future_res=res;
#if H3_PHASE_A_ALWAYS_PREFETCH == 2
            // For the terminal unused A tile, a zero bound suppresses physical
            // reads while retaining the identical instruction/barrier schedule.
            future_res[2]=tile+1<weight_count ? res[2] : 0;
#endif
            stage_weight(shared,future_res,id-CW*64,tile+1,k);
#else
            if(tile+1<weight_count)stage_weight(shared,res,id-CW*64,tile+1,k);
#endif
            asm volatile("s_barrier\n\ts_waitcnt vmcnt(0)\n\ts_barrier" ::: "memory");
        }
#endif
#elif H3_PHASE_READY_WAIT
        // Only current A is required at entry. Issue the next slot after READY;
        // ACK protects all current LDS reads without waiting for future VMEM.
        // Wait for that next slot before the following READY makes it visible.
        asm volatile("s_waitcnt vmcnt(0)" ::: "memory");
        for(int tile=0;tile<weight_count;++tile){
            ready_ack();
            if(tile+1<weight_count)stage_weight(shared,res,id-CW*64,tile+1,k);
            ready_ack();
            asm volatile("s_waitcnt vmcnt(0)" ::: "memory");
        }
#else
        if(weight_count>1)stage_weight(shared,res,id-CW*64,1,k);
        asm volatile("s_waitcnt vmcnt(0)" ::: "memory");
        for(int tile=0;tile<weight_count;++tile){
            ready_ack();
            asm volatile("s_waitcnt vmcnt(0)" ::: "memory");
            ready_ack();
            if(tile+2<weight_count)stage_weight(shared,res,id-CW*64,tile+2,k);
        }
#endif
    }else{
#if H3_PHASE_SCALAR_WAVE
        const int wave=uniform_wave,lane=id%64,row=lane%16,klane=(lane/16)*8;
#else
        const int wave=id/64,lane=id%64,row=lane%16,klane=(lane/16)*8;
#endif
        const int wave_m=wave/NW,wave_n=wave%NW;
#if H3_PHASE_STAGGER
        static_assert(H3_PHASE_NATIVE_CADENCE && WK==64 && MW==1 && !H3_PHASE_B64,
                      "Wave stagger requires matched K64 native cadence");
        const bool stagger=__builtin_amdgcn_readfirstlane(wave)>=4;
#endif
#if H3_PHASE_MID_SPLIT
        static_assert(MT==16,"Split point audited for MW1 only");
        const int prefetch_point=wave>=4 ? 12 : 4;
#else
        constexpr int prefetch_point=MW==1 ? H3_PHASE_MID_POINT : MT/2;
#endif
#if H3_PHASE_SWAP
        static_assert(WK==64,"Swapped phases must share one protected K64 A slot");
        const int swap=wave>=4;
#else
        constexpr int swap=0;
#endif
#if H3_PHASE_STRUCT_B32
        const auto res=h3_struct_b32_resource<H3_PHASE_STRUCT_B32-1>(input+tn*k,tokens-tn,k);
#elif H3_PHASE_STRUCT_B64
        const auto res=h3_struct_b64_resource<H3_PHASE_STRUCT_B64-1>(input+tn*k,tokens-tn,k);
#elif H3_PHASE_POW2_B
        const auto res=h3_b_resource(input+tn*k,tokens-tn,k,H3_PHASE_B_BIAS);
#else
        const auto res=ck_tile::make_wave_buffer_resource(input+tn*k,(tokens-tn)*k*2
#if H3_PHASE_CACHE_B
                                                        ,k*2
#endif
                                                        );
#endif
        Acc accum[NT][MT]={};
#if H3_PHASE_RESERVE_LOW
        constrain_low_registers();
#endif
#if H3_PHASE_ACC_PACK == 1 || H3_PHASE_ACC_PACK == 3
        constrain_accumulator_bank(accum);
#endif
#if H3_PHASE_BUFFER_EPILOGUE
        const auto output_res=ck_tile::make_wave_buffer_resource(output+tn*features,(tokens-tn)*features*2);
#endif
#if H3_PHASE_NATIVE_B64_HOIST
        static_assert(H3_PHASE_NATIVE_B64 && H3_PHASE_STATIC_STAGGER,
                      "Shared B preparation requires the audited two static B64 wave groups");
#include "phase_k64_prepare.hpp"
#endif
#if H3_PHASE_STATIC_STAGGER
        static_assert(H3_PHASE_NATIVE_CADENCE && H3_PHASE_PAIR_TILES && !H3_PHASE_B64 && !H3_PHASE_STAGGER,
                      "Static wave groups specialize the validated paired cadence only");
        auto consume_wave_group = [&](auto stagger_tag) __attribute__((always_inline)) {
        constexpr bool stagger=decltype(stagger_tag)::value;
#if H3_PHASE_GROUP_PREFETCH
        // Shadow the common point with a compile-time group-specific point.
        // Loading order, VM completion waits and both matched barriers remain.
        constexpr int prefetch_point=stagger ? H3_PHASE_EARLY_PREFETCH_POINT : H3_PHASE_LATE_PREFETCH_POINT;
        static_assert(MT==16 && prefetch_point>0 && prefetch_point<16,
                      "Group prefetch must stay within the same audited DS16 phase");
#endif
#endif
#if H3_PHASE_NATIVE_B64
#include "phase_k64_native.hpp"
#elif H3_PHASE_B64
#include "phase_k64_body.hpp"
#else
#if H3_PHASE_BANK
        V8 bank[2][NT],av[MT];
#else
        V8 current[NT],future[NT],av[MT];
#endif
        #pragma unroll
        for(int j=0;j<NT;++j){
#if H3_PHASE_BANK
            bank[0][j]=activation(res,wave_n*NT*16+row+j*16,swap*32+klane,k);
#else
            current[j]=activation(res,wave_n*NT*16+row+j*16,swap*32+klane,k);
#endif
        }
#if H3_PHASE_NATIVE_CADENCE
        asm volatile("s_barrier\n\ts_waitcnt vmcnt(0)\n\ts_barrier" ::: "memory");
        __builtin_amdgcn_sched_barrier(0);
#endif
#if H3_PHASE_UNROLL2
        #pragma unroll 2
#endif
#if H3_PHASE_PAIR_TILES
        static_assert(H3_PHASE_NATIVE_CADENCE && WK==64 && MW==1 && !H3_PHASE_B64,
                      "Explicit pair expansion audited for native K64 A/K32 B");
        auto consume_pair_tile = [&](int tile,auto slot_tag) __attribute__((always_inline)) {
#else
        for(int tile=0;tile<count;++tile){
#endif
            if constexpr(WK==64 && !H3_PHASE_NATIVE_CADENCE)ready_ack();
            #pragma unroll
            for(int phase=0;phase<2;++phase){
                if constexpr(WK==32)ready_ack();
                const int weight_tile=WK==32 ? tile*2+phase : tile;
#if H3_PHASE_PAIR_TILES
                constexpr int offset=decltype(slot_tag)::value*(FW*WK);
#else
                const int offset=(weight_tile%2)*(FW*WK);
#endif
                const int next_phase=tile*2+phase+1;
                const int next_k=(next_phase/2)*64+((next_phase%2)^swap)*32+klane;
                constexpr bool force_future=H3_PHASE_ALWAYS_PREFETCH;
                const bool has_future=force_future || next_phase<count*2;
#if H3_PHASE_A_TWO_BASE
                static_assert(WK==64 && MW==1 && MT==16 && H3_PHASE_STATIC_STAGGER,
                              "Two LDS bases require the audited row<16 K64 static-group layout");
                const BF* a_even=shared+offset+phys(row,(phase^swap)*32+klane);
                const BF* a_odd=shared+offset+phys(row+16,(phase^swap)*32+klane);
#endif
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
#if H3_PHASE_A_TWO_BASE >= 2
                ck_tile::static_for<0,MT,1>{}([&](auto a_index){
                    constexpr int i=decltype(a_index)::value;
#else
                #pragma unroll
                for(int i=0;i<MT;++i){
#endif
#if H3_PHASE_MID_PREFETCH
                    if(i==prefetch_point && has_future && (H3_PHASE_MID_GROUP ? wave>=4 : true)){
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
#if H3_PHASE_A_PHASE1_PREFETCH
                    // Phase1 A was loaded after each old fragment's last
                    // phase0 MMAC use. Current K64 shared slot remains protected
                    // through the phase1 ACK, so producer cannot overwrite it.
                    if(phase==0 || i<H3_PHASE_A_PHASE1_BEGIN ||
                       i>=H3_PHASE_A_PHASE1_BEGIN+H3_PHASE_A_PHASE1_COUNT)
                        av[i]=weight_fragment(shared+offset+phys(row+i*16,phase*32+klane));
#elif H3_PHASE_A_TWO_BASE == 1
                    av[i]=weight_fragment((i%2 ? a_odd : a_even)+(i/2)*2048);
#elif H3_PHASE_A_TWO_BASE == 2
                    av[i]=weight_fragment_immediate<(i/2)*4096>(
                        static_cast<unsigned>(reinterpret_cast<uintptr_t>(i%2 ? a_odd : a_even)));
#elif H3_PHASE_A_TWO_BASE == 3
                    static_assert(H3_PHASE_MID_POINT%2==0 && !H3_PHASE_GROUP_PREFETCH && !H3_PHASE_MID_SPLIT,
                                  "Pair loads must not hide a prefetch scheduled on an odd element");
                    if(i%2==0)weight_fragment_pair<(i/2)*4096>(av[i],av[i+1],
                        static_cast<unsigned>(reinterpret_cast<uintptr_t>(a_even)),
                        static_cast<unsigned>(reinterpret_cast<uintptr_t>(a_odd)));
#else
                    av[i]=weight_fragment(shared+offset+phys(wave_m*MT*16+row+i*16,(WK==64?(phase^swap)*32:0)+klane));
#endif
#if H3_PHASE_A_TWO_BASE >= 2
                });
#else
                }
#endif
#if H3_PHASE_A_TWO_BASE >= 2 || H3_PHASE_A_READ_WAIT
                // Inline DS outputs are asynchronous: do not rely on the
                // compiler inferring the LGKM dependency from the asm operands.
                // The early-barrier group already drains LGKM before math below.
                if constexpr(!stagger){
                    asm volatile("s_waitcnt lgkmcnt(0)" ::: "memory");
                    __builtin_amdgcn_sched_barrier(0);
                }
#endif
                if(!H3_PHASE_NATIVE_CADENCE && (WK==32 || phase==1))ready_ack();
#if H3_PHASE_DS_ASM && !H3_PHASE_DS_PROGRESS
                asm volatile("s_waitcnt lgkmcnt(0)" ::: "memory");
                __builtin_amdgcn_sched_barrier(0);
#endif
#if H3_PHASE_BURST
                // Hypothesis: avoid interleaving future DS issue into a MMAC
                // burst. All A fragments are ready before entering this burst.
                // This is not removal of READY/ACK or any data dependency wait.
                asm volatile("s_waitcnt lgkmcnt(0)" ::: "memory");
                __builtin_amdgcn_sched_barrier(0);
#endif
#if H3_PHASE_ASM_B
#if H3_PHASE_VM_AFTER_BARRIER
                if constexpr(!stagger){
#endif
                // Only this compute wave issues its B loads (NT instructions per
                // phase). Future-bank loads are newer than all current-bank loads.
                // Allow NT outstanding future loads, but wait for all current data;
                // at the terminal phase there are no future loads, so drain to 0.
#if H3_PHASE_POW2_B || H3_PHASE_FINAL_VM_WAIT || H3_PHASE_STRUCT_B32
                if(next_phase>=count*2) asm volatile("s_waitcnt vmcnt(0)" ::: "memory");
                else
#endif
                if(has_future) asm volatile("s_waitcnt vmcnt(%0)" :: "n"(NT) : "memory");
                else asm volatile("s_waitcnt vmcnt(0)" ::: "memory");
                __builtin_amdgcn_sched_barrier(0);
#if H3_PHASE_VM_AFTER_BARRIER
                }
#endif
#endif
#if H3_PHASE_STAGGER || H3_PHASE_STATIC_STAGGER
                if(stagger){
                    // This group reaches each barrier before math. All current
                    // A reads must finish before producer can reuse this slot;
                    // later math consumes private VGPRs, not live shared memory.
                    __builtin_amdgcn_sched_barrier(0);
#if H3_PHASE_BUILTIN_LGKM
                    // LGKM=0; VM=63 and EXP=7 are left unconstrained. The
                    // compiler can track completion of ordinary C++ LDS loads.
                    __builtin_amdgcn_s_waitcnt(0xc07f);
                    asm volatile("s_barrier" ::: "memory");
#else
                    asm volatile("s_waitcnt lgkmcnt(0)\n\ts_barrier" ::: "memory");
#endif
                    __builtin_amdgcn_sched_barrier(0);
                }
#endif
#if H3_PHASE_VM_AFTER_BARRIER
                static_assert(H3_PHASE_STATIC_STAGGER && H3_PHASE_BANK && H3_PHASE_ASM_B &&
                              !H3_PHASE_POW2_B && !H3_PHASE_FINAL_VM_WAIT,
                              "Deferred B wait is restricted to the audited raw K32 bank path");
                if constexpr(stagger){
                    // Current A has been read into private registers and the
                    // shared-slot ACK is safe regardless of private B latency.
                    // Still complete current B before the first MMAC consumes it.
                    if(has_future)asm volatile("s_waitcnt vmcnt(%0)" :: "n"(NT) : "memory");
                    else asm volatile("s_waitcnt vmcnt(0)" ::: "memory");
                    __builtin_amdgcn_sched_barrier(0);
                }
#endif
#if H3_PHASE_A_PHASE1_PREFETCH
                if constexpr(!stagger){
                    if(phase==1){
                        // Complete all streamed phase1 A before math. Early
                        // group already drains LGKM before its phase1 ACK.
                        asm volatile("s_waitcnt lgkmcnt(0)" ::: "memory");
                        __builtin_amdgcn_sched_barrier(0);
                    }
                }
#endif
#if H3_PHASE_PRIORITY
                asm volatile("s_setprio 1" ::: "memory");
#endif
#if H3_PHASE_MMAC_GROUP4
                static_assert(MT==16 && NT==2 && H3_PHASE_BANK &&
                              H3_PHASE_POSTPRIO && H3_PHASE_POSTPRIO_VALUE==1,
                              "Four-MMAC sequence preserves the audited bank/priority path");
                #pragma unroll
                for(int j=0;j<NT;++j){
                    #pragma unroll
                    for(int i=0;i<MT;i+=4){
                        if(j==0 && i==0)
                            h3_mmac_group4<false,true>(accum[j][i],accum[j][i+1],accum[j][i+2],accum[j][i+3],
                                av[i],av[i+1],av[i+2],av[i+3],bank[phase][j]);
                        else
                            h3_mmac_group4<false,false>(accum[j][i],accum[j][i+1],accum[j][i+2],accum[j][i+3],
                                av[i],av[i+1],av[i+2],av[i+3],bank[phase][j]);
                    }
                }
                #pragma unroll
                for(int j=0;j<NT;++j){
                    #pragma unroll
                    for(int i=0;i<MT;i+=4)
                        h3_mmac_group4<true,false>(accum[j][i],accum[j][i+1],accum[j][i+2],accum[j][i+3],
                            av[i],av[i+1],av[i+2],av[i+3],bank[phase][j]);
                }
#elif H3_PHASE_MMAC_CHUNK
                static_assert(MT==16 && NT==2 && MT%H3_PHASE_MMAC_CHUNK==0 &&
                              H3_PHASE_MMAC_CHUNK>=4 && H3_PHASE_MMAC_CHUNK<=16 &&
                              H3_PHASE_BANK && H3_PHASE_POSTPRIO && H3_PHASE_POSTPRIO_VALUE==1,
                              "Chunking may reorder independent C tiles, not their K accumulation order");
                #pragma unroll
                for(int first_i=0;first_i<MT;first_i+=H3_PHASE_MMAC_CHUNK){
                    #pragma unroll
                    for(int j=0;j<NT;++j){
                        #pragma unroll
                        for(int part=0;part<H3_PHASE_MMAC_CHUNK;++part){
                            const int i=first_i+part;
                            MM{}(accum[j][i],av[i].lo,bank[phase][j].lo);
                            if(first_i==0 && j==0 && part==0)
                                asm volatile("s_setprio 1" ::: "memory");
                        }
                    }
                    #pragma unroll
                    for(int j=0;j<NT;++j){
                        #pragma unroll
                        for(int part=0;part<H3_PHASE_MMAC_CHUNK;++part){
                            const int i=first_i+part;
                            MM{}(accum[j][i],av[i].hi,bank[phase][j].hi);
                        }
                    }
                }
#else
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
                        if(j==0 && i==0){
#if H3_PHASE_POSTPRIO_VALUE == 1
                            asm volatile("s_setprio 1" ::: "memory");
#else
                            static_assert(H3_PHASE_POSTPRIO_VALUE>=0 && H3_PHASE_POSTPRIO_VALUE<=3,
                                          "Two-bit immediate wave priority");
                            asm volatile("s_setprio %0" :: "n"(H3_PHASE_POSTPRIO_VALUE) : "memory");
#endif
                        }
#endif
                    }
                }
                #pragma unroll
                for(int j=0;j<NT;++j){
                    #pragma unroll
#if H3_PHASE_A_PHASE1_PREFETCH
                    for(int i=0;i<MT;++i){
                        MM{}(accum[j][i],av[i].hi,bank[phase][j].hi);
                        if(phase==0 && j==NT-1 && i>=H3_PHASE_A_PHASE1_BEGIN &&
                           i<H3_PHASE_A_PHASE1_BEGIN+H3_PHASE_A_PHASE1_COUNT &&
                           (i+1-H3_PHASE_A_PHASE1_BEGIN)%H3_PHASE_A_PHASE1_PREFETCH==0){
                            #pragma unroll
                            for(int p=0;p<H3_PHASE_A_PHASE1_PREFETCH;++p){
                                const int next_i=i+1-H3_PHASE_A_PHASE1_PREFETCH+p;
                                av[next_i]=weight_fragment(shared+offset+phys(row+next_i*16,32+klane));
                            }
                        }
                    }
#else
                    for(int i=0;i<MT;++i)MM{}(accum[j][i],av[i].hi,
#if H3_PHASE_BANK
                                             bank[phase][j].hi);
#else
                                             current[j].hi);
#endif
#endif
                }
#endif
#if H3_PHASE_PRIORITY || H3_PHASE_POSTPRIO
                asm volatile("s_setprio 0" ::: "memory");
#endif
#if H3_PHASE_ACC_PACK == 2 || H3_PHASE_ACC_PACK == 3
                constrain_accumulator_bank(accum);
#endif
#if H3_PHASE_RESERVE_LOW
                constrain_low_registers();
#endif
#if H3_PHASE_BURST || H3_PHASE_MATH_FENCE
                __builtin_amdgcn_sched_barrier(0);
#endif
#if H3_PHASE_NATIVE_CADENCE
                // phase0: producer has issued A(tile+1), in the other slot.
                // phase1: producer has waited for it; current A is consumed,
                // so producer may overwrite current slot for tile+2.
#if H3_PHASE_STAGGER || H3_PHASE_STATIC_STAGGER
                if(!stagger)
#endif
                {
                    __builtin_amdgcn_sched_barrier(0);
                    asm volatile("s_barrier" ::: "memory");
                }
#endif
#if !H3_PHASE_BANK
                if(next_phase<count*2){
                    #pragma unroll
                    for(int j=0;j<NT;++j)current[j]=future[j];
                }
#endif
            }
        }
#if H3_PHASE_PAIR_TILES
        ; // End lambda definition: both inlined calls have fixed LDS slot IDs.
#if H3_PHASE_FIXED_SHAPE
        #pragma unroll 1
#endif
        for(int tile=0;tile<count;tile+=2){
            consume_pair_tile(tile,ck_tile::number<0>{});
            if(tile+1<count)consume_pair_tile(tile+1,ck_tile::number<1>{});
        }
#endif
#if H3_PHASE_ALWAYS_PREFETCH
        static_assert(H3_PHASE_BANK && H3_PHASE_ASM_B && (NT==2 || NT==4),"Audited banked B path only");
        // The terminal prefetch is not part of the GEMM. Keep both its vectors
        // live so the allocator cannot reuse their still-pending destinations
        // for A/accumulator registers. Drain before any output/epilogue reuse.
        asm volatile("s_waitcnt vmcnt(0)" ::: "memory");
        __builtin_amdgcn_sched_barrier(0);
        if constexpr(NT==2){
            asm volatile("" :: "v"(bank[0][0]),"v"(bank[0][1]) : "memory");
        }else{
            asm volatile("" :: "v"(bank[0][0]),"v"(bank[0][1]),
                               "v"(bank[0][2]),"v"(bank[0][3]) : "memory");
        }
#endif
#endif // H3_PHASE_B64
#if H3_PHASE_STATIC_STAGGER
        };
        // Choose once, outside the entire K loop. The two instantiated bodies
        // keep the same K order and matched barrier count; current A is in
        // private registers before the early-barrier group permits LDS reuse.
        static_assert(H3_PHASE_STAGGER_FIRST_WAVE>0 && H3_PHASE_STAGGER_FIRST_WAVE<CW,
                      "Both compute groups must participate in every barrier");
        if(__builtin_amdgcn_readfirstlane(wave)>=H3_PHASE_STAGGER_FIRST_WAVE)
            consume_wave_group(ck_tile::number<1>{});
        else
            consume_wave_group(ck_tile::number<0>{});
#endif
#if H3_PHASE_ACC_PACK == 1 || H3_PHASE_ACC_PACK == 3
        constrain_accumulator_bank(accum);
#endif
#if H3_PHASE_PAIRED_OUTPUT
        static_assert(MW==1 && H3_PHASE_BUFFER_EPILOGUE && H3_PHASE_OUTPUT_RNE==1,
                      "Paired output retains the audited MW1 CK mask RNE and bounded stores");
        const bool pair_aligned=(reinterpret_cast<uintptr_t>(output)&3)==0;
#if H3_PHASE_PAIRED_OUTPUT >= 3
        auto emit_pairs = [&](auto aligned_tag) __attribute__((always_inline)) {
        constexpr bool pair_aligned=decltype(aligned_tag)::value;
#endif
        #pragma unroll
        for(int j=0;j<NT;++j){
            #pragma unroll
            for(int i=0;i<MT;++i){
                #pragma unroll
                for(int x=0;x<4;x+=2){
                    const unsigned first=h3_rne_mask(accum[j][i][x]);
                    const unsigned second=h3_rne_mask(accum[j][i][x+1]);
                    const unsigned word=h3_feature_pair<(H3_PHASE_PAIRED_OUTPUT==2 || H3_PHASE_PAIRED_OUTPUT==4)>(first,second,lane);
                    const unsigned token=wave_n*NT*16+j*16+lane/16+x*4+(lane&1)*4;
                    const unsigned voffset=(token*features+fm+(row&~1))*2;
                    h3_store_feature_pair(word,output_res,voffset,i*32,pair_aligned);
                }
            }
        }
#if H3_PHASE_PAIRED_OUTPUT >= 3
        };
        if(pair_aligned)emit_pairs(ck_tile::number<1>{});
        else emit_pairs(ck_tile::number<0>{});
#endif
#else
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
#if H3_PHASE_OUTPUT_RNE
                    const auto converted=ck_tile::bit_cast<BF>(H3_PHASE_OUTPUT_RNE==1 ?
                        h3_rne_mask(accum[j][i][x]) : (H3_PHASE_OUTPUT_RNE==3 ?
                        h3_rne_fpclass(accum[j][i][x]) : h3_rne_select(accum[j][i][x])));
#else
                    const auto converted=ck_tile::type_convert<BF>(accum[j][i][x]);
#endif
                    output_fragment(converted,output_res,voffset,0,i*32);
#else
                    if(feature<features && token<tokens)
                        output[token*features+feature]=ck_tile::type_convert<BF>(accum[j][i][x]);
#endif
                }
            }
        }
#endif
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
#if H3_PHASE_STRUCT_B32
    // Descriptor stride must fit its 14-bit field. Check without k*2 overflow.
    if(k>=8192)return int(hipErrorInvalidValue);
#endif
#if H3_PHASE_CACHE_A || H3_PHASE_CACHE_B
    if(k*2>=0x4000)return int(hipErrorInvalidValue);
#endif
#if H3_PHASE_POW2_B
    if(k*2>=0x4000)return int(hipErrorInvalidValue);
#endif
#if H3_PHASE_GRID_2D
    const dim3 grid(n/FW,(m+255)/256);
#else
    const dim3 grid((m+255)/256*(n/FW));
#endif
#if H3_PHASE_FIXED_SHAPE
    // Exact N/K dispatch; other supported sizes retain the generic instance.
    // Mode 1 fixes N+K, mode 2 only K, mode 3 only N. M remains runtime.
    #define H3_FIXED_LAUNCH(NV,KV) \
        hipLaunchKernelGGL((h3_ck_rolephase<(H3_PHASE_FIXED_SHAPE==2 ? 0 : NV), \
                                            (H3_PHASE_FIXED_SHAPE==3 ? 0 : KV)>), \
                          grid,dim3(THREADS),0,stream,static_cast<const BF*>(w), \
                          static_cast<const BF*>(a),static_cast<BF*>(c),n,m,k)
    if(n==5376 && k==7168){ H3_FIXED_LAUNCH(5376,7168); }
    else if(n==10752 && k==5376){ H3_FIXED_LAUNCH(10752,5376); }
    else if(n==5376 && k==3584){ H3_FIXED_LAUNCH(5376,3584); }
    else if(n==14336 && k==5376){ H3_FIXED_LAUNCH(14336,5376); }
    else{
        hipLaunchKernelGGL((h3_ck_rolephase<0,0>),grid,dim3(THREADS),0,stream,
            static_cast<const BF*>(w),static_cast<const BF*>(a),static_cast<BF*>(c),n,m,k);
    }
    #undef H3_FIXED_LAUNCH
#else
    hipLaunchKernelGGL(h3_ck_rolephase,grid,dim3(THREADS),0,stream,
        static_cast<const BF*>(w),static_cast<const BF*>(a),static_cast<BF*>(c),n,m,k);
#endif
    return int(hipGetLastError());
}
