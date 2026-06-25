// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2026, Advanced Micro Devices, Inc. All rights reserved.

#pragma once

#include "ck_tile/core.hpp"
#include "ck_tile/ops/gemm/block/block_gemm_problem.hpp"
#include "ck_tile/ops/gemm/block/mmac_block_gemm_asmem_bsmem_creg_v1.hpp"
#include "ck_tile/ops/gemm/pipeline/tile_gemm_shape.hpp"
#include "ck_tile/ops/gemm/warp/warp_mmac_gemm.hpp"
#include "jenga_bwd_dkdv_config.hpp"

namespace ck_tile {
namespace example {
namespace jenga {

using JengaWarpGemmMmacBF16BF16F32_WT16x16x64_MR1NR1MI1NI1 = WarpGemmImpl<
    WarpGemmAttributeMmacIterateK<WarpGemmAttributeMmacImplBf16Bf16F32M16N16K16, 1, 1, 1, 1, 4>>;

struct JengaMmacQKConfig
{
    static constexpr ck_tile::index_t kN         = 64;
    static constexpr ck_tile::index_t kK         = 128;
    static constexpr ck_tile::index_t kBlockSize = 256;

    using BlockWarps = ck_tile::sequence<2, 2, 1>;
    using WarpTile   = ck_tile::sequence<16, 16, 128>;
    using WarpGemm   = ck_tile::WarpGemmMmacBF16BF16F32_WT16x16x128_MR1NR1MI1NI1;
};

struct JengaMmacQKPolicy
{
    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr auto GetWarpGemmMWarpNWarp()
    {
        return ck_tile::make_tuple(typename JengaMmacQKConfig::WarpGemm{},
                                   JengaMmacQKConfig::BlockWarps::at(ck_tile::number<0>{}),
                                   JengaMmacQKConfig::BlockWarps::at(ck_tile::number<1>{}));
    }
};

template <typename Problem>
using JengaMmacQKGemmShape =
    ck_tile::TileGemmShape<ck_tile::sequence<Problem::BlockM,
                                             JengaMmacQKConfig::kN,
                                             JengaMmacQKConfig::kK>,
                           typename JengaMmacQKConfig::BlockWarps,
                           typename JengaMmacQKConfig::WarpTile>;

template <typename Problem>
using JengaMmacQKGemmProblem =
    ck_tile::BlockGemmProblem<typename Problem::QDataType,
                              typename Problem::KDataType,
                              typename Problem::AccDataType,
                              256,
                              JengaMmacQKGemmShape<Problem>>;

template <typename Problem>
using JengaMmacQKBlockGemm =
    ck_tile::MmacBlockGemmASmemBSmemCRegV1<JengaMmacQKGemmProblem<Problem>, JengaMmacQKPolicy>;

struct JengaMmacDVConfig
{
    static constexpr ck_tile::index_t kM         = 64;
    static constexpr ck_tile::index_t kN         = 128;
    static constexpr ck_tile::index_t kBlockSize = 256;

    using BlockWarps = ck_tile::sequence<2, 2, 1>;
    using WarpTile   = ck_tile::sequence<16, 16, 64>;
    using WarpGemm   = JengaWarpGemmMmacBF16BF16F32_WT16x16x64_MR1NR1MI1NI1;
};

struct JengaMmacDVPolicy
{
    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr auto GetWarpGemmMWarpNWarp()
    {
        return ck_tile::make_tuple(typename JengaMmacDVConfig::WarpGemm{},
                                   JengaMmacDVConfig::BlockWarps::at(ck_tile::number<0>{}),
                                   JengaMmacDVConfig::BlockWarps::at(ck_tile::number<1>{}));
    }
};

template <typename Problem>
using JengaMmacDVGemmShape =
    ck_tile::TileGemmShape<ck_tile::sequence<JengaMmacDVConfig::kM,
                                             JengaMmacDVConfig::kN,
                                             Problem::BlockM>,
                           typename JengaMmacDVConfig::BlockWarps,
                           typename JengaMmacDVConfig::WarpTile>;

template <typename Problem>
using JengaMmacDVGemmProblem =
    ck_tile::BlockGemmProblem<typename Problem::QDataType,
                              typename Problem::OGradDataType,
                              typename Problem::AccDataType,
                              256,
                              JengaMmacDVGemmShape<Problem>>;

template <typename Problem>
using JengaMmacDVBlockGemm =
    ck_tile::MmacBlockGemmASmemBSmemCRegV1<JengaMmacDVGemmProblem<Problem>, JengaMmacDVPolicy>;

struct JengaMmacDPConfig
{
    static constexpr ck_tile::index_t kN         = 64;
    static constexpr ck_tile::index_t kK         = 128;
    static constexpr ck_tile::index_t kBlockSize = 256;

    using BlockWarps = ck_tile::sequence<2, 2, 1>;
    using WarpTile   = ck_tile::sequence<16, 16, 128>;
    using WarpGemm   = ck_tile::WarpGemmMmacBF16BF16F32_WT16x16x128_MR1NR1MI1NI1;
};

struct JengaMmacDPPolicy
{
    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr auto GetWarpGemmMWarpNWarp()
    {
        return ck_tile::make_tuple(typename JengaMmacDPConfig::WarpGemm{},
                                   JengaMmacDPConfig::BlockWarps::at(ck_tile::number<0>{}),
                                   JengaMmacDPConfig::BlockWarps::at(ck_tile::number<1>{}));
    }
};

template <typename Problem>
using JengaMmacDPGemmShape =
    ck_tile::TileGemmShape<ck_tile::sequence<Problem::BlockM,
                                             JengaMmacDPConfig::kN,
                                             JengaMmacDPConfig::kK>,
                           typename JengaMmacDPConfig::BlockWarps,
                           typename JengaMmacDPConfig::WarpTile>;

template <typename Problem>
using JengaMmacDPGemmProblem =
    ck_tile::BlockGemmProblem<typename Problem::OGradDataType,
                              typename Problem::VDataType,
                              typename Problem::AccDataType,
                              256,
                              JengaMmacDPGemmShape<Problem>>;

template <typename Problem>
using JengaMmacDPBlockGemm =
    ck_tile::MmacBlockGemmASmemBSmemCRegV1<JengaMmacDPGemmProblem<Problem>, JengaMmacDPPolicy>;

struct JengaMmacDKConfig
{
    static constexpr ck_tile::index_t kM         = 64;
    static constexpr ck_tile::index_t kN         = 128;
    static constexpr ck_tile::index_t kBlockSize = 256;

    using BlockWarps = ck_tile::sequence<2, 2, 1>;
    using WarpTile   = ck_tile::sequence<16, 16, 64>;
    using WarpGemm   = JengaWarpGemmMmacBF16BF16F32_WT16x16x64_MR1NR1MI1NI1;
};

struct JengaMmacDKPolicy
{
    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr auto GetWarpGemmMWarpNWarp()
    {
        return ck_tile::make_tuple(typename JengaMmacDKConfig::WarpGemm{},
                                   JengaMmacDKConfig::BlockWarps::at(ck_tile::number<0>{}),
                                   JengaMmacDKConfig::BlockWarps::at(ck_tile::number<1>{}));
    }
};

template <typename Problem>
using JengaMmacDKGemmShape =
    ck_tile::TileGemmShape<ck_tile::sequence<JengaMmacDKConfig::kM,
                                             JengaMmacDKConfig::kN,
                                             Problem::BlockM>,
                           typename JengaMmacDKConfig::BlockWarps,
                           typename JengaMmacDKConfig::WarpTile>;

template <typename Problem>
using JengaMmacDKGemmProblem =
    ck_tile::BlockGemmProblem<typename Problem::QDataType,
                              typename Problem::QDataType,
                              typename Problem::AccDataType,
                              256,
                              JengaMmacDKGemmShape<Problem>>;

template <typename Problem>
using JengaMmacDKBlockGemm =
    ck_tile::MmacBlockGemmASmemBSmemCRegV1<JengaMmacDKGemmProblem<Problem>, JengaMmacDKPolicy>;

template <ck_tile::index_t M, ck_tile::index_t K>
CK_TILE_HOST_DEVICE constexpr auto MakeSimpleLdsDescriptor()
{
    return ck_tile::make_naive_tensor_descriptor(
        ck_tile::make_tuple(ck_tile::number<M>{}, ck_tile::number<K>{}),
        ck_tile::make_tuple(ck_tile::number<K>{}, ck_tile::number<1>{}),
        ck_tile::number<8>{},
        ck_tile::number<1>{});
}

template <ck_tile::index_t K, ck_tile::index_t M>
CK_TILE_HOST_DEVICE constexpr auto MakeTransposedLdsDescriptor()
{
    const auto desc_raw = ck_tile::make_naive_tensor_descriptor(
        ck_tile::make_tuple(ck_tile::number<M>{}, ck_tile::number<K>{}),
        ck_tile::make_tuple(ck_tile::number<K>{}, ck_tile::number<1>{}),
        ck_tile::number<8>{},
        ck_tile::number<1>{});

    return ck_tile::transform_tensor_descriptor(
        desc_raw,
        ck_tile::make_tuple(ck_tile::make_pass_through_transform(ck_tile::number<M>{}),
                            ck_tile::make_pass_through_transform(ck_tile::number<K>{})),
        ck_tile::make_tuple(ck_tile::sequence<0>{}, ck_tile::sequence<1>{}),
        ck_tile::make_tuple(ck_tile::sequence<1>{}, ck_tile::sequence<0>{}));
}


template <ck_tile::index_t M, ck_tile::index_t N, typename BlockGemm, typename AccTile, typename AccDataType>
CK_TILE_DEVICE void StoreMmacOutputTileToLdsRowMajor(const BlockGemm& block_gemm,
                                                     const AccTile& acc_tile,
                                                     AccDataType* out_smem)
{
    const auto out_tile = block_gemm.MakeOuputLayout(acc_tile);
    constexpr auto spans = decltype(out_tile)::get_distributed_spans();

    ck_tile::sweep_tile_span(spans[ck_tile::number<0>{}], [&](auto idx0) {
        ck_tile::sweep_tile_span(spans[ck_tile::number<1>{}], [&](auto idx1) {
            constexpr auto tile_idx = ck_tile::make_tuple(idx0, idx1);
            const auto x_idx = ck_tile::get_x_indices_from_distributed_indices(
                out_tile.get_tile_distribution(), tile_idx);
            const ck_tile::index_t m = x_idx.at(ck_tile::number<0>{});
            const ck_tile::index_t n = x_idx.at(ck_tile::number<1>{});

            if(m < M && n < N)
            {
                out_smem[static_cast<ck_tile::long_index_t>(m) * N + n] = out_tile[tile_idx];
            }
        });
    });
}

template <typename Problem>
struct JengaBwdDkdvDefaultPolicy
{
    using QKBlockGemm = JengaMmacQKBlockGemm<Problem>;
    using DVBlockGemm = JengaMmacDVBlockGemm<Problem>;
    using DPBlockGemm = JengaMmacDPBlockGemm<Problem>;
    using DKBlockGemm = JengaMmacDKBlockGemm<Problem>;

    union LdsStorage
    {
        struct
        {
            typename Problem::QDataType q[Problem::BlockM * Problem::HeadDim];
            typename Problem::KDataType k[Problem::BlockN * Problem::HeadDim];
        } qk_input;
        typename Problem::AccDataType qk[Problem::BlockM * Problem::BlockN];
        struct
        {
            typename Problem::OGradDataType do_t[Problem::HeadDim * Problem::BlockM];
            union
            {
                typename Problem::QDataType p_t[Problem::BlockN * Problem::BlockM];
                typename Problem::VDataType v[Problem::BlockN * Problem::HeadDim];
            } rhs;
        } dv_dp_input;
        struct
        {
            typename Problem::QDataType ds_t[Problem::BlockN * Problem::BlockM];
            typename Problem::QDataType q_t[Problem::HeadDim * Problem::BlockM];
        } dk_input;
    };
};

} // namespace jenga
} // namespace example
} // namespace ck_tile
