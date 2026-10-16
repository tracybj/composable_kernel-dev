// SPDX-License-Identifier: MIT
#pragma once

#include <hip/hip_runtime.h>
#define CK_TILE_FLOAT_TO_BFLOAT16_DEFAULT 0
#include "ck_tile/core.hpp"
#include "ck_tile/ops/gemm/warp/warp_gemm_attribute_mmac_impl.hpp"
#include "ck_tile/ops/gemm/pipeline/tile_gemm_shape.hpp"

namespace ck_tile::h3 {

// Public operation: Y[tokens, features] = X[tokens, K] * W[features, K]^T.
// Internally A is the shared weight tile; B is the register-resident activation.
struct H3GemmProblem
{
    using ADataType    = bf16_t;
    using BDataType    = bf16_t;
    using CDataType    = bf16_t;
    using CAccDataType = float;
    using Vector      = ext_vector_t<bf16_t, 8>;
    using WarpGemm    = WarpGemmAttributeMmacImplBf16Bf16F32M16N16K16;
    using AccVector   = WarpGemm::CVecType;

    // CK's native two-group shape describes this asymmetric pipeline. A
    // producer wave transports 64x64 weight elements (not an MMAC tile);
    // consumer waves use the actual 16x16x16 MMAC primitive.
    using BlockGemmShape = TileGemmShape2Groups<sequence<256, 256, 64>,
                                               sequence<4, 1, 1>, sequence<64, 1, 64>,
                                               sequence<1, 8, 1>, sequence<16, 16, 16>>;

    static constexpr index_t FeatureTile   = BlockGemmShape::kM;
    static constexpr index_t TokenTile     = BlockGemmShape::kN;
    static constexpr index_t WeightK       = BlockGemmShape::kK;
    static constexpr index_t PhaseK        = 32;
    static constexpr index_t ComputeWaves  = BlockGemmShape::NumConsumerWarps;
    static constexpr index_t ProducerWaves = BlockGemmShape::NumProducerWarps;
    static constexpr index_t WaveSize      = 64;
    static constexpr index_t BlockSize     = (ComputeWaves + ProducerWaves) * WaveSize;
    static constexpr index_t FeatureWaves  = BlockGemmShape::ConsumerBlockWarps::at(number<0>{});
    static constexpr index_t TokenWaves    = ComputeWaves / FeatureWaves;
    static constexpr index_t FeatureFrags  = (FeatureTile / 16) / FeatureWaves;
    static constexpr index_t TokenFrags    = 16 / TokenWaves;
    static constexpr index_t PrefetchPoint = 8;
    static constexpr index_t EarlyWaves    = 4;
    static constexpr index_t TokenGroup    = 8;
    static constexpr index_t LdsElements   = FeatureTile * WeightK * 2;

    static_assert(FeatureFrags == 16 && TokenFrags == 2);
    static_assert(LdsElements * sizeof(ADataType) == 65536);
};

} // namespace ck_tile::h3
