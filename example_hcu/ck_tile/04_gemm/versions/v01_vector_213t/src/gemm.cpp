// BF16 optimization fork of example_hcu/ck_tile/03_gemm.
// Shared CK includes and 03_gemm remain unchanged.
#include <hip/hip_runtime.h>
#define CK_TILE_FLOAT_TO_BFLOAT16_DEFAULT 0
#include "ck_tile/core.hpp"
#include "ck_tile/ops/epilogue.hpp"
#include "ck_tile/ops/gemm/block/block_gemm_problem.hpp"
#include "ck_tile/ops/gemm/block/block_gemm_asmem_bsmem_creg_v1_custom_policy.hpp"
#include "ck_tile/ops/gemm/block/mmac_block_gemm_asmem_bsmem_creg_v1.hpp"
#include "ck_tile/ops/gemm/pipeline/tile_gemm_shape.hpp"
#include "ck_tile/ops/gemm/pipeline/tile_gemm_traits.hpp"
#include "ck_tile/ops/gemm/pipeline/gemm_pipeline_problem.hpp"
#if H3_RAW_BARRIER
namespace ck_tile {
CK_TILE_DEVICE void h3_lds_barrier(){
    // This mainloop shares only LDS. Future VMEM results are private registers.
    // Keep LDS writes/reads ordered without draining unrelated global prefetch.
    asm volatile("s_waitcnt lgkmcnt(0)\n\ts_barrier" ::: "memory");
}
}
#define block_sync_lds h3_lds_barrier
#endif
#include "ck_tile/ops/gemm/pipeline/mmac_gemm_pipeline_agmem_bgmem_creg_v1.hpp"
#if H3_RAW_BARRIER
#undef block_sync_lds
#endif
#include "ck_tile/ops/gemm/kernel/gemm_kernel.hpp"
#include "ck_tile/ops/gemm/kernel/gemm_tile_partitioner.hpp"
#if H3_GROUP_M
#include "grouped_partitioner.hpp"
#endif
#if H3_GROUP_N
#include "grouped_n_partitioner.hpp"
#endif
#include "policy.hpp"
#include "warp_variants.hpp"
#if H3_LDS_PAD
#include "padded_policy.hpp"
#endif
#if H3_LDS_XOR
#include "xor_policy.hpp"
#endif
#if H3_LDS_KPACK
#include "kpack_policy.hpp"
#endif
#if H3_DOUBLE_BUFFER
#include "double_buffer.hpp"
#include "double_buffer_dynamic.hpp"
#endif
#ifndef H3_BM
#define H3_BM 128
#define H3_BN 128
#define H3_BK 32
#define H3_WM 4
#define H3_WN 1
#endif
#ifndef H3_SWIZZLE
#define H3_SWIZZLE 0
#endif
#ifndef H3_WK
#define H3_WK 32
#endif
#ifndef H3_WARP_M
#define H3_WARP_M 32
#endif
#ifndef H3_PAD_M
#define H3_PAD_M 0
#endif
#ifndef H3_PAD_N
#define H3_PAD_N 0
#endif
using Row=ck_tile::tensor_layout::gemm::RowMajor;
using Col=ck_tile::tensor_layout::gemm::ColumnMajor;
#if H3_GEMM_TRANSPOSE
using OutLayout=Col;
#else
using OutLayout=Row;
#endif
using Shape=ck_tile::TileGemmShape<ck_tile::sequence<H3_BM,H3_BN,H3_BK>,ck_tile::sequence<H3_WM,H3_WN,1>,ck_tile::sequence<H3_WARP_M,64,H3_WK>>;
#if H3_MR4
using Traits=ck_tile::MmacTileGemmTraits<H3_PAD_M,H3_PAD_N,false,Row,Col,OutLayout,4,4,1,1>;
#elif H3_NI4
using Traits=ck_tile::MmacTileGemmTraits<H3_PAD_M,H3_PAD_N,false,Row,Col,OutLayout,2,1,1,4>;
#else
using Traits=ck_tile::MmacTileGemmTraits<H3_PAD_M,H3_PAD_N,false,Row,Col,OutLayout,2,4,1,1>;
#endif
using BaseProblem=ck_tile::MmacUniversalGemmPipelineProblem<ck_tile::bf16_t,ck_tile::bf16_t,float,Shape,Traits>;
#if H3_CONTIGUOUS_B_VECTOR
// This experiment uses contiguous [N,K] B, a 16-byte aligned base and K % BK == 0.
// Padding N does not break K-vector alignment. Preserve row-tail bounds in the
// tensor view; only override the generic kPadN -> scalar-vector heuristic.
struct Problem : BaseProblem {
    static constexpr ck_tile::index_t VectorSizeB=8;
};
#else
using Problem=BaseProblem;
#endif
#if H3_LDS_KPACK
using Policy=ck_tile::H3KPackPolicy;
#elif H3_LDS_XOR
using Policy=ck_tile::H3XorPolicy;
#elif H3_LDS_PAD
using Policy=ck_tile::H3PaddedPolicy;
#else
using Policy=ck_tile::H3GemmPolicy;
#endif
#if H3_DYNAMIC_DOUBLE
using Pipeline=ck_tile::H3DynamicDoublePipeline<Problem,Policy>;
#elif H3_DOUBLE_BUFFER
using Pipeline=ck_tile::H3DoubleBufferPipeline<Problem,Policy>;
#else
using Pipeline=ck_tile::MmacGemmPipelineAGmemBGmemCRegV1<Problem,Policy>;
#endif
using Epi=ck_tile::Default2DEpilogue<ck_tile::Default2DEpilogueProblem<float,ck_tile::bf16_t,H3_PAD_M,H3_PAD_N>>;
#if H3_GROUP_N
using Partition=ck_tile::H3GroupedNPartitioner<Shape>;
#elif H3_GROUP_M
using Partition=ck_tile::H3GroupedPartitioner<Shape>;
#else
using Partition=ck_tile::GemmTilePartitioner<Shape>;
#endif
using G=ck_tile::GemmKernel<Partition,Pipeline,Epi>;
#if H3_RELAX_BOUNDS
__global__ __launch_bounds__(H3_WM*H3_WN*64)
#else
__global__ __launch_bounds__(H3_WM*H3_WN*64,1)
#endif
void h3_ck_bf16(G::GemmCommonKargs args){
#if H3_LIVE_PAD > 0
    unsigned p0=threadIdx.x^0x123401u;
    asm volatile("" : "+v"(p0));
#endif
#if H3_LIVE_PAD > 1
    unsigned p1=threadIdx.x^0x123402u;
    asm volatile("" : "+v"(p1));
#endif
#if H3_LIVE_PAD > 2
    unsigned p2=threadIdx.x^0x123403u;
    asm volatile("" : "+v"(p2));
#endif
    G{}(args);
#if H3_LIVE_PAD == 1
    asm volatile("" : : "v"(p0) : "memory");
#elif H3_LIVE_PAD == 2
    asm volatile("" : : "v"(p0),"v"(p1) : "memory");
#elif H3_LIVE_PAD > 2
    asm volatile("" : : "v"(p0),"v"(p1),"v"(p2) : "memory");
#endif
}
extern "C" int h3_gemm(const void* a,const void* w,void* c,int m,int n,int k,hipStream_t s){
#if H3_CONTIGUOUS_B_VECTOR
    static_assert(H3_BK % 8 == 0, "K tile must preserve BF16 vec8 alignment");
    if((reinterpret_cast<uintptr_t>(a) & 15) || k % 8)
        return int(hipErrorInvalidValue);
#endif
#if H3_GEMM_TRANSPOSE
    if((!H3_PAD_N && m%H3_BN) || n%H3_BM || k%H3_BK)return int(hipErrorInvalidValue);
    auto args=G::MakeKargs(w,a,c,n,m,k,k,k,n);
    hipLaunchKernelGGL(h3_ck_bf16,G::GridSize(n,m,1),G::BlockSize(),0,s,args);
#else
    if((!H3_PAD_M && m%H3_BM) || n%H3_BN || k%H3_BK)return int(hipErrorInvalidValue);
    auto args=G::MakeKargs(a,w,c,m,n,k,k,k,n);
    hipLaunchKernelGGL(h3_ck_bf16,G::GridSize(m,n,1),G::BlockSize(),0,s,args);
#endif
    return int(hipGetLastError());
}
