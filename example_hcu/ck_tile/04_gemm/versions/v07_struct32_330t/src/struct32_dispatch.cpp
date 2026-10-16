// Single optional h3_gemm entry for all four benchmarked H3 local shapes.
// Exact N/K dispatch only; arbitrary original-domain shapes retain the baseline.
#include <hip/hip_runtime.h>
#define CK_TILE_FLOAT_TO_BFLOAT16_DEFAULT 0
#include "ck_tile/core.hpp"
#include "ck_tile/ops/gemm/warp/warp_gemm_attribute_mmac_impl.hpp"
#include "ck_tile/core/arch/hcu_buffer_addressing.hpp"

#undef H3_PHASE_SCALAR_B_BASE
#define H3_PHASE_SCALAR_B_BASE 1
namespace h3_original {
#define h3_gemm h3_original_gemm
#define h3_ck_rolephase h3_original_kernel
#include "rolephase.cpp"
#undef h3_gemm
#undef h3_ck_rolephase
}

#undef H3_PHASE_SCALAR_B_BASE
#define H3_PHASE_SCALAR_B_BASE 0
#undef H3_PHASE_STRUCT_B32
#define H3_PHASE_STRUCT_B32 2
namespace h3_struct32 {
#define h3_gemm h3_struct32_gemm
#define h3_ck_rolephase h3_struct32_kernel
#include "rolephase.cpp"
#undef h3_gemm
#undef h3_ck_rolephase
}

#undef H3_PHASE_PRODUCER_PAIRS
#define H3_PHASE_PRODUCER_PAIRS 1
#undef H3_PHASE_A_SOFFSET
#define H3_PHASE_A_SOFFSET 1
namespace h3_struct32_producer {
#define h3_gemm h3_struct32_producer_gemm
#define h3_ck_rolephase h3_struct32_producer_kernel
#include "rolephase.cpp"
#undef h3_gemm
#undef h3_ck_rolephase
}

extern "C" int h3_gemm(const void* a,const void* w,void* c,int m,int n,int k,hipStream_t stream){
    if(n==5376 && (k==7168 || k==3584))
        return h3_struct32_producer::h3_struct32_producer_gemm(a,w,c,m,n,k,stream);
    if((n==10752 || n==14336) && k==5376)
        return h3_struct32::h3_struct32_gemm(a,w,c,m,n,k,stream);
    return h3_original::h3_original_gemm(a,w,c,m,n,k,stream);
}
