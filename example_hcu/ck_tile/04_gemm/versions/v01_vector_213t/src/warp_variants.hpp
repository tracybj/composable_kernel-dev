// Local BF16 alias using the same hardware 16x16x16 MMAC as 03_gemm.
// A smaller software warp K tile shortens operand register lifetimes.
#pragma once
namespace ck_tile {
using H3BF16Warp32x64x16=WarpGemmImpl<WarpGemmAttributeMmacIterateK<WarpGemmAttributeMmacImplBf16Bf16F32M16N16K16,2,4,1,1,1>>;
using H3BF16Warp32x64x32NI4=WarpGemmImpl<WarpGemmAttributeMmacIterateK<WarpGemmAttributeMmacImplBf16Bf16F32M16N16K16,2,1,1,4,2>>;
using H3BF16Warp64x64x32=WarpGemmImpl<WarpGemmAttributeMmacIterateK<WarpGemmAttributeMmacImplBf16Bf16F32M16N16K16,4,4,1,1,2>>;
using H3BF16Warp64x64x16=WarpGemmImpl<WarpGemmAttributeMmacIterateK<WarpGemmAttributeMmacImplBf16Bf16F32M16N16K16,4,4,1,1,1>>;
namespace impl {
template<> struct WarpGemmMmacDispatcher<bf16_t,bf16_t,float,32,64,16,false,2,4,1,1>{using Type=H3BF16Warp32x64x16;};
template<> struct WarpGemmMmacDispatcher<bf16_t,bf16_t,float,32,64,32,false,2,1,1,4>{using Type=H3BF16Warp32x64x32NI4;};
template<> struct WarpGemmMmacDispatcher<bf16_t,bf16_t,float,64,64,32,false,4,4,1,1>{using Type=H3BF16Warp64x64x32;};
template<> struct WarpGemmMmacDispatcher<bf16_t,bf16_t,float,64,64,16,false,4,4,1,1>{using Type=H3BF16Warp64x64x16;};
}
}
