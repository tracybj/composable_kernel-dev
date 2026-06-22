// SPDX-License-Identifier: MIT
// Copyright (c) 2026, Advanced Micro Devices, Inc. All rights reserved.

#pragma once

#include "jenga_bwd_dq_config.hpp"
#include "ck_tile/ops/gemm/block/block_gemm_problem.hpp"
#include "ck_tile/ops/gemm/block/block_gemm_areg_breg_creg_v1_new.hpp"
#include "ck_tile/ops/gemm/block/block_gemm_areg_breg_creg_v1_custom_policy.hpp"
#include "ck_tile/ops/gemm/block/mmac_block_gemm_asmem_bsmem_creg_v1.hpp"
#include "ck_tile/ops/gemm/pipeline/tile_gemm_shape.hpp"
#include "jenga_block_gemm_areg_bsmem_creg_v2r1.hpp"

namespace ck_tile {
namespace example {
namespace jenga {

template <typename BlockWarps, typename WarpGemm>
struct JengaBwdDqMmacBlockGemmPolicy
{
    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr auto GetWarpGemmMWarpNWarp()
    {
        constexpr index_t m_warps = BlockWarps::at(number<0>{});
        constexpr index_t n_warps = BlockWarps::at(number<1>{});
        return make_tuple(WarpGemm{}, m_warps, n_warps);
    }
};

template <typename BlockWarps, typename WarpGemm>
struct JengaBwdDqARegBSmemBlockGemmPolicy
{
    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr auto GetWarpGemmMWarpNWarp()
    {
        constexpr index_t m_warps = BlockWarps::at(number<0>{});
        constexpr index_t n_warps = BlockWarps::at(number<1>{});
        return make_tuple(WarpGemm{}, m_warps, n_warps);
    }
};

template <typename ADataType_,
          typename BDataType_,
          typename CDataType_,
          index_t BlockSize_,
          typename GemmShape_>
struct JengaBwdDqRegBlockGemmProblem
    : public BlockGemmProblem<ADataType_, BDataType_, CDataType_, BlockSize_, GemmShape_>
{
    static constexpr index_t NumWaveGroups = 1;
};

struct JengaBwdDqDefaultPolicy
{
    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr index_t GetBlockSize()
    {
        return Problem::ThreadsPerBlock;
    }

    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr index_t GetRowsPerBlock()
    {
        return Problem::BlockM;
    }

    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr index_t GetColsPerBlock()
    {
        return Problem::HeadDim;
    }

    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr index_t GetThreadLinearSpan()
    {
        return Problem::BlockM * Problem::HeadDim;
    }

    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr index_t GetVectorSize()
    {
        return 8;
    }

    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr auto GetMmacBlockWarps()
    {
        return std::conditional_t<Problem::ThreadsPerBlock >= 512,
                                  sequence<4, 2, 1>,
                                  sequence<2, 2, 1>>{};
    }

    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr auto GetQKBlockGemm()
    {
        using QDataType = remove_cvref_t<typename Problem::QDataType>;
        using KDataType = remove_cvref_t<typename Problem::KDataType>;
        using BlockWarps = remove_cvref_t<decltype(GetMmacBlockWarps<Problem>())>;
        using GemmShape =
            TileGemmShape<sequence<Problem::BlockM, Problem::BlockN, Problem::HeadDim>,
                          BlockWarps,
                          sequence<16, 32, 128>>;
        using WarpGemm = std::conditional_t<
            std::is_same_v<QDataType, bf16_t> && std::is_same_v<KDataType, bf16_t>,
            WarpGemmMmacBF16BF16F32_WT16x32x128_MR1NR1MI1NI2,
            WarpGemmMmacF16F16F32_WT16x32x128_MR1NR1MI1NI2>;
        using GemmProblem =
            BlockGemmProblem<QDataType, KDataType, float, Problem::ThreadsPerBlock, GemmShape>;
        using BlockGemm =
            MmacBlockGemmASmemBSmemCRegV1<GemmProblem,
                                          JengaBwdDqMmacBlockGemmPolicy<BlockWarps, WarpGemm>>;
        return BlockGemm{};
    }

    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr auto GetQKRegBlockGemm()
    {
        using QDataType  = remove_cvref_t<typename Problem::QDataType>;
        using KDataType  = remove_cvref_t<typename Problem::KDataType>;
        using BlockWarps = remove_cvref_t<decltype(GetMmacBlockWarps<Problem>())>;
        using GemmShape =
            TileGemmShape<sequence<Problem::BlockM, Problem::BlockN, Problem::HeadDim>,
                          BlockWarps,
                          sequence<16, 32, 128>>;
        using WarpGemm = std::conditional_t<
            std::is_same_v<QDataType, bf16_t> && std::is_same_v<KDataType, bf16_t>,
            WarpGemmMmacBF16BF16F32_WT16x32x128_MR1NR1MI1NI2,
            WarpGemmMmacF16F16F32_WT16x32x128_MR1NR1MI1NI2>;
        using GemmProblem = JengaBwdDqRegBlockGemmProblem<QDataType,
                                                          KDataType,
                                                          float,
                                                          Problem::ThreadsPerBlock,
                                                          GemmShape>;
        using BlockGemm = JengaBlockGemmARegBSmemCRegV2R1<
            GemmProblem,
            JengaBwdDqARegBSmemBlockGemmPolicy<BlockWarps, WarpGemm>>;
        return BlockGemm{};
    }

    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr auto GetDPBlockGemm()
    {
        using OGradDataType = remove_cvref_t<typename Problem::OGradDataType>;
        using VDataType     = remove_cvref_t<typename Problem::VDataType>;
        using BlockWarps    = remove_cvref_t<decltype(GetMmacBlockWarps<Problem>())>;
        using GemmShape =
            TileGemmShape<sequence<Problem::BlockM, Problem::BlockN, Problem::HeadDim>,
                          BlockWarps,
                          sequence<16, 32, 128>>;
        using WarpGemm = std::conditional_t<
            std::is_same_v<OGradDataType, bf16_t> && std::is_same_v<VDataType, bf16_t>,
            WarpGemmMmacBF16BF16F32_WT16x32x128_MR1NR1MI1NI2,
            WarpGemmMmacF16F16F32_WT16x32x128_MR1NR1MI1NI2>;
        using GemmProblem = JengaBwdDqRegBlockGemmProblem<OGradDataType,
                                                          VDataType,
                                                          float,
                                                          Problem::ThreadsPerBlock,
                                                          GemmShape>;
        using BlockGemmPolicy =
            BlockGemmARegBRegCRegV1CustomPolicy<OGradDataType, VDataType, float, BlockWarps, WarpGemm>;
        using BlockGemm = BlockGemmARegBRegCRegV1<GemmProblem, BlockGemmPolicy>;
        return BlockGemm{};
    }

    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr auto GetDQBlockGemm()
    {
        using QDataType  = remove_cvref_t<typename Problem::QDataType>;
        using KDataType  = remove_cvref_t<typename Problem::KDataType>;
        using BlockWarps = remove_cvref_t<decltype(GetMmacBlockWarps<Problem>())>;
        using GemmShape =
            TileGemmShape<sequence<Problem::BlockM, Problem::HeadDim, Problem::BlockN>,
                          BlockWarps,
                          sequence<16, 32, 64>>;
        using WarpGemm = std::conditional_t<
            std::is_same_v<QDataType, bf16_t> && std::is_same_v<KDataType, bf16_t>,
            WarpGemmMmacBF16BF16F32_WT16x32x64_MR1NR2MI1NI1_TRANSC,
            WarpGemmMmacF16F16F32_WT16x32x64_MR1NR2MI1NI1_TRANSC>;
        using GemmProblem =
            BlockGemmProblem<QDataType, KDataType, float, Problem::ThreadsPerBlock, GemmShape>;
        using BlockGemm =
            MmacBlockGemmASmemBSmemCRegV1<GemmProblem,
                                          JengaBwdDqMmacBlockGemmPolicy<BlockWarps, WarpGemm>>;
        return BlockGemm{};
    }

    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr auto GetDQRegBlockGemm()
    {
        using QDataType  = remove_cvref_t<typename Problem::QDataType>;
        using KDataType  = remove_cvref_t<typename Problem::KDataType>;
        using BlockWarps = remove_cvref_t<decltype(GetMmacBlockWarps<Problem>())>;
        using GemmShape =
            TileGemmShape<sequence<Problem::BlockM, Problem::HeadDim, Problem::BlockN>,
                          BlockWarps,
                          sequence<16, 32, 64>>;
        using WarpGemm = std::conditional_t<
            std::is_same_v<QDataType, bf16_t> && std::is_same_v<KDataType, bf16_t>,
            WarpGemmMmacBF16BF16F32_WT16x32x64_MR1NR2MI1NI1_TRANSC,
            WarpGemmMmacF16F16F32_WT16x32x64_MR1NR2MI1NI1_TRANSC>;
        using GemmProblem = JengaBwdDqRegBlockGemmProblem<QDataType,
                                                          KDataType,
                                                          float,
                                                          Problem::ThreadsPerBlock,
                                                          GemmShape>;
        using BlockGemm = JengaBlockGemmARegBSmemCRegV2R1<
            GemmProblem,
            JengaBwdDqARegBSmemBlockGemmPolicy<BlockWarps, WarpGemm>>;
        return BlockGemm{};
    }

    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr auto MakeQOGradDQBlockTileDistribution()
    {
        using QDataType = remove_cvref_t<typename Problem::QDataType>;
        constexpr index_t kBlockSize = Problem::ThreadsPerBlock;
        constexpr index_t kMPerBlock = Problem::BlockM;
        constexpr index_t kKPerBlock = Problem::HeadDim;
        constexpr index_t K1         = 16 / sizeof(QDataType);
        constexpr index_t K0         = kKPerBlock / K1;
        constexpr index_t M1         = get_warp_size() / K0;
        constexpr index_t M0         = kBlockSize / get_warp_size();
        constexpr index_t M2         = kMPerBlock / (M1 * M0);

        return make_static_tile_distribution(
            tile_distribution_encoding<sequence<>,
                                       tuple<sequence<M0, M1, M2>, sequence<K0, K1>>,
                                       tuple<sequence<1>, sequence<1, 2>>,
                                       tuple<sequence<0>, sequence<1, 0>>,
                                       sequence<1, 2>,
                                       sequence<2, 1>>{});
    }

    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr auto MakeDsBlockTileDistribution()
    {
        using QDataType = remove_cvref_t<typename Problem::QDataType>;
        constexpr index_t kBlockSize = Problem::ThreadsPerBlock;
        constexpr index_t kMPerBlock = Problem::BlockM;
        constexpr index_t kNPerBlock = Problem::BlockN;
        constexpr index_t N1         = 16 / sizeof(QDataType);
        constexpr index_t N0         = kNPerBlock / N1;
        constexpr index_t M1         = get_warp_size() / N0;
        constexpr index_t M0         = kBlockSize / get_warp_size();
        constexpr index_t M2         = kMPerBlock / (M1 * M0);

        return make_static_tile_distribution(
            tile_distribution_encoding<sequence<>,
                                       tuple<sequence<M0, M1, M2>, sequence<N0, N1>>,
                                       tuple<sequence<1>, sequence<1, 2>>,
                                       tuple<sequence<0>, sequence<1, 0>>,
                                       sequence<1, 2>,
                                       sequence<2, 1>>{});
    }

    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr auto MakeOGradRegBlockDistribution()
    {
        using BlockGemm       = remove_cvref_t<decltype(GetDPBlockGemm<Problem>())>;
        constexpr auto config = BlockGemm::Policy::template GetWarpGemmMWarpNWarp<Problem>();
        using WarpGemm        = remove_cvref_t<decltype(config.template at<0>())>;

        constexpr index_t MWarp = config.template at<1>();
        constexpr index_t NWarp = config.template at<2>();

        constexpr index_t MIterPerWarp = Problem::BlockM / (MWarp * WarpGemm::kM);
        constexpr index_t KIterPerWarp = Problem::HeadDim / WarpGemm::kK;

        constexpr auto do_block_outer_dstr_encoding =
            tile_distribution_encoding<sequence<NWarp>,
                                       tuple<sequence<MIterPerWarp, MWarp>, sequence<KIterPerWarp>>,
                                       tuple<sequence<1, 0>>,
                                       tuple<sequence<1, 0>>,
                                       sequence<1, 2>,
                                       sequence<0, 0>>{};

        constexpr auto do_block_dstr_encode =
            detail::make_embed_tile_distribution_encoding(do_block_outer_dstr_encoding,
                                                          typename WarpGemm::AWarpDstrEncoding{});

        return make_static_tile_distribution(do_block_dstr_encode);
    }

    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr auto MakeVRegBlockDistribution()
    {
        using BlockGemm       = remove_cvref_t<decltype(GetDPBlockGemm<Problem>())>;
        constexpr auto config = BlockGemm::Policy::template GetWarpGemmMWarpNWarp<Problem>();
        using WarpGemm        = remove_cvref_t<decltype(config.template at<0>())>;

        constexpr index_t MWarp = config.template at<1>();
        constexpr index_t NWarp = config.template at<2>();

        constexpr index_t NIterPerWarp = Problem::BlockN / (NWarp * WarpGemm::kN);
        constexpr index_t KIterPerWarp = Problem::HeadDim / WarpGemm::kK;

        constexpr auto v_block_outer_dstr_encoding =
            tile_distribution_encoding<sequence<MWarp>,
                                       tuple<sequence<NIterPerWarp, NWarp>, sequence<KIterPerWarp>>,
                                       tuple<sequence<0, 1>>,
                                       tuple<sequence<0, 1>>,
                                       sequence<1, 2>,
                                       sequence<0, 0>>{};

        constexpr auto v_block_dstr_encode =
            detail::make_embed_tile_distribution_encoding(v_block_outer_dstr_encoding,
                                                          typename WarpGemm::BWarpDstrEncoding{});

        return make_static_tile_distribution(v_block_dstr_encode);
    }

    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr auto MakeKVBlockTileDistribution()
    {
        using KDataType = remove_cvref_t<typename Problem::KDataType>;
        constexpr index_t kBlockSize  = Problem::ThreadsPerBlock;
        constexpr index_t kNPerBlock  = Problem::BlockN;
        constexpr index_t kKPerBlock  = Problem::HeadDim;
        constexpr index_t K1          = 16 / sizeof(KDataType);
        constexpr index_t K0          = kKPerBlock / K1;
        constexpr index_t N1          = get_warp_size() / K0;
        constexpr index_t N0          = kBlockSize / get_warp_size();
        constexpr index_t N2          = kNPerBlock / (N1 * N0);

        return make_static_tile_distribution(
            tile_distribution_encoding<sequence<>,
                                       tuple<sequence<N0, N1, N2>, sequence<K0, K1>>,
                                       tuple<sequence<1>, sequence<1, 2>>,
                                       tuple<sequence<0>, sequence<1, 0>>,
                                       sequence<1, 2>,
                                       sequence<2, 1>>{});
    }

    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr index_t GetSmemSize()
    {
        return 0;
    }

    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr bool IsSupported()
    {
        return Problem::BlockM > 0 && Problem::BlockN > 0 && Problem::HeadDim > 0 &&
               Problem::MaxNnz > 0 && Problem::ThreadsPerBlock > 0 &&
               Problem::ThreadsPerBlock <= CK_TILE_MAX_THREAD_PER_BLOCK;
    }
};

} // namespace jenga
} // namespace example
} // namespace ck_tile
