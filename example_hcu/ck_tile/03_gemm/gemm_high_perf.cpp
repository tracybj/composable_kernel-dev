// SPDX-License-Identifier: MIT
// Copyright (c) 2024, Inc. All rights reserved.

#include <hip/hip_runtime.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>

#include "ck_tile/ops/epilogue.hpp"
#include "ck_tile/ops/gemm.hpp"
#include "ck_tile/host.hpp"

#include "ck_tile/ops/gemm/pipeline/mmac_gemm_pipeline_ag_bg_cr_mem.hpp"
#include "ck_tile/ops/gemm/kernel/universal_gemm_kernel.hpp"
#include "ck_tile/ops/gemm/kernel/gemm_tile_partitioner.hpp"
#include "ck_tile/ops/gemm/pipeline/tile_gemm_traits.hpp"
#include "ck_tile/ops/epilogue/cshuffle_epilogue.hpp"
#include "ck_tile/ops/gemm/block/mmac_block_gemm_asmem_bsmem_creg_v1.hpp"

#include "gemm_high_perf.hpp"

// Define a custom policy structure to override MmacBlockGemmASmemBSmemCRegV1 policy's hardcoded MWarp and NWarp
template <typename BlockWarps, typename WarpGemm>
struct MyBlockGemmPolicy
{
    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr auto GetWarpGemmMWarpNWarp()
    {
        constexpr ck_tile::index_t m_warps = BlockWarps::at(ck_tile::number<0>{});
        constexpr ck_tile::index_t n_warps = BlockWarps::at(ck_tile::number<1>{});
        return ck_tile::make_tuple(WarpGemm{}, m_warps, n_warps);
    }
};

template <typename BlockWarps, typename WarpGemm>
struct MyMmacGemmPipelinePolicy : public ck_tile::MmacGemmPipelineAGmemBGmemCRegV1DefaultPolicy
{
    template <typename Problem>
    CK_TILE_HOST_DEVICE static constexpr auto GetBlockGemm()
    {
        return ck_tile::MmacBlockGemmASmemBSmemCRegV1<Problem, MyBlockGemmPolicy<BlockWarps, WarpGemm>>{};
    }
};

// Define a custom wrapper for GemmPipelineAgBgCrMem to add missing GetVectorSize methods and expose base types required by UniversalGemmKernel
template <typename Problem, typename Policy = ck_tile::MmacGemmPipelineAGmemBGmemCRegV1DefaultPolicy>
struct MyGemmPipelineAgBgCrMem : public ck_tile::GemmPipelineAgBgCrMem<Problem, Policy>
{
    using Base = ck_tile::GemmPipelineAgBgCrMem<Problem, Policy>;
    using Base::Base;

    using ADataType = typename Base::ADataType;
    using BDataType = typename Base::BDataType;
    using CDataType = typename Base::CDataType;
    using ALayout   = typename Base::ALayout;
    using BLayout   = typename Base::BLayout;
    using CLayout   = typename Base::CLayout;

    static constexpr auto BlockSize = Base::BlockSize;
    static constexpr bool kPadM     = Base::kPadM;
    static constexpr bool kPadN     = Base::kPadN;
    static constexpr bool kPadK     = Base::kPadK;

    static constexpr bool Preshuffle       = false;
    static constexpr bool DoubleSmemBuffer = false;
    static constexpr ck_tile::index_t NumWaveGroups = 1;

    CK_TILE_HOST_DEVICE static constexpr ck_tile::index_t GetSmemSize()
    {
        return Base::GetSmemSize();
    }

    template <bool IsWave32Host = false>
    static constexpr ck_tile::index_t GetVectorSizeA()
    {
        return Base::VectorSizeA;
    }

    template <bool IsWave32Host = false>
    static constexpr ck_tile::index_t GetVectorSizeB()
    {
        return Base::VectorSizeB;
    }

    static constexpr ck_tile::index_t GetVectorSizeA()
    {
        return Base::VectorSizeA;
    }

    static constexpr ck_tile::index_t GetVectorSizeB()
    {
        return Base::VectorSizeB;
    }

    static constexpr ck_tile::index_t GetSmemPackB()
    {
        return 1;
    }

    static const std::string GetName()
    {
        return "mem_pipeline";
    }
};

template <typename ALayout, typename BLayout, typename CLayout>
float gemm_calc(const gemm_basic_args& args, const ck_tile::stream_config& s)
{
    // High-performance configurations:
#ifndef M_TILE
#define M_TILE 128
#endif
#ifndef N_TILE
#define N_TILE 128
#endif
#ifndef K_TILE
#define K_TILE 64
#endif

#ifndef M_WARP
#define M_WARP 4
#endif
#ifndef N_WARP
#define N_WARP 2
#endif
#ifndef K_WARP
#define K_WARP 1
#endif

#ifndef M_WARP_TILE
#define M_WARP_TILE 32
#endif
#ifndef N_WARP_TILE
#define N_WARP_TILE 64
#endif
#ifndef K_WARP_TILE
#define K_WARP_TILE 32
#endif

#ifndef WARP_GEMM_POLICY
#define WARP_GEMM_POLICY ck_tile::WarpGemmMmacF16F16F32_WT32x64x32_MR2NR1MI1NI4
#endif

    constexpr ck_tile::index_t M_Tile = M_TILE;
    constexpr ck_tile::index_t N_Tile = N_TILE;
    constexpr ck_tile::index_t K_Tile = K_TILE; // K=64 for fp16 high performance

    constexpr ck_tile::index_t M_Warp = M_WARP;
    constexpr ck_tile::index_t N_Warp = N_WARP;
    constexpr ck_tile::index_t K_Warp = K_WARP;

    // Use WT32x64x32 config to match GFX936 template specialties of WarpGemmMmacDispatcher
    constexpr ck_tile::index_t M_Warp_Tile = M_WARP_TILE;
    constexpr ck_tile::index_t N_Warp_Tile = N_WARP_TILE;
    constexpr ck_tile::index_t K_Warp_Tile = K_WARP_TILE;

    constexpr bool kPadM = false;
    constexpr bool kPadN = false;
    constexpr bool kPadK = false;

    constexpr int kBlockPerCu = 1;

    using GemmShape =
        ck_tile::TileGemmShape<ck_tile::sequence<M_Tile, N_Tile, K_Tile>,
                               ck_tile::sequence<M_Warp, N_Warp, K_Warp>,
                               ck_tile::sequence<M_Warp_Tile, N_Warp_Tile, K_Warp_Tile>>;
    
    // Use GemmSpatiallyLocalTilePartitioner (non-static member based) to prevent device variable linking issues
    using TilePartitioner = ck_tile::GemmSpatiallyLocalTilePartitioner<GemmShape, 1, 1>;

    using Traits = ck_tile::TileGemmTraits<kPadM, kPadN, kPadK, ALayout, BLayout, CLayout>;

    using BaseGemmPipeline = ck_tile::BaseGemmPipelineAgBgCrMem<
        ck_tile::GemmPipelineProblem<ADataType, BDataType, AccDataType, GemmShape, Traits>>;

    const ck_tile::index_t num_loop    = TilePartitioner::GetLoopNum(args.K);
    const bool has_hot_loop            = BaseGemmPipeline::BlockHasHotloop(num_loop);
    const ck_tile::TailNumber tail_num = BaseGemmPipeline::GetBlockLoopTailNum(num_loop);

    float ave_time{0};

    const auto Run = [&](const auto has_hot_loop_, const auto tail_number_) {
        constexpr bool has_hot_loop_v = has_hot_loop_.value;
        constexpr auto tail_number_v  = tail_number_.value;

        using GemmPipeline = MyGemmPipelineAgBgCrMem<
            ck_tile::UniversalGemmPipelineProblem<ADataType,
                                                  BDataType,
                                                  AccDataType,
                                                  GemmShape,
                                                  Traits,
                                                  ck_tile::GemmPipelineScheduler::Intrawave,
                                                  has_hot_loop_v,
                                                  tail_number_v>,
            MyMmacGemmPipelinePolicy<ck_tile::sequence<M_Warp, N_Warp, K_Warp>, WARP_GEMM_POLICY>>;

        using GemmEpilogue = ck_tile::CShuffleEpilogue<
            ck_tile::CShuffleEpilogueProblem<ADataType,
                                             BDataType,
                                             ck_tile::tuple<>,
                                             AccDataType,
                                             CDataType,
                                             ck_tile::tuple<>,
                                             CLayout,
                                             ck_tile::element_wise::PassThrough,
                                             M_Tile,
                                             N_Tile,
                                             M_Warp,
                                             N_Warp,
                                             M_Warp_Tile,
                                             N_Warp_Tile,
                                             K_Warp_Tile,
                                             false,
                                             ck_tile::memory_operation_enum::set>,
            WARP_GEMM_POLICY>;

        using Kernel = ck_tile::UniversalGemmKernel<TilePartitioner, GemmPipeline, GemmEpilogue>;

        ck_tile::UniversalGemmHostArgs<1, 1, 0> host_args(
            std::array<const void*, 1>{args.p_a},
            std::array<const void*, 1>{args.p_b},
            std::array<const void*, 0>{},
            args.p_c,
            args.kbatch,
            args.M,
            args.N,
            args.K,
            std::array<ck_tile::index_t, 1>{args.stride_A},
            std::array<ck_tile::index_t, 1>{args.stride_B},
            std::array<ck_tile::index_t, 0>{},
            args.stride_C
        );

        auto kargs = Kernel::MakeKernelArgs(host_args);

        if(!Kernel::IsSupportedArgument(kargs))
        {
            throw std::runtime_error("Kernel arguments not supported!");
        }

        const dim3 grids      = Kernel::GridSize(args.M, args.N, args.kbatch);
        constexpr dim3 blocks = dim3(Kernel::kBlockSize); // Resolve non-constexpr BlockSize()

        if(s.log_level_ > 0)
        {
            std::cout << "Launching kernel with args:"
                      << " grid: {" << grids.x << ", " << grids.y << ", " << grids.z << "}"
                      << ", blocks: {" << blocks.x << ", " << blocks.y << ", " << blocks.z << "}"
                      << std::endl;
        }

        ave_time = ck_tile::launch_kernel(
            s, ck_tile::make_kernel<blocks.x, kBlockPerCu>(Kernel{}, grids, blocks, 0, kargs));
        return ave_time;
    };

    if(has_hot_loop)
    {
        if(tail_num == ck_tile::TailNumber::One)
        {
            Run(ck_tile::bool_constant<true>{},
                ck_tile::integral_constant<ck_tile::TailNumber, ck_tile::TailNumber::One>{});
        }
        else if(tail_num == ck_tile::TailNumber::Full)
        {
            Run(ck_tile::bool_constant<true>{},
                ck_tile::integral_constant<ck_tile::TailNumber, ck_tile::TailNumber::Full>{});
        }

        if constexpr(BaseGemmPipeline::PrefetchStages > 2)
        {
            if(tail_num == ck_tile::TailNumber::Two)
            {
                Run(ck_tile::bool_constant<true>{},
                    ck_tile::integral_constant<ck_tile::TailNumber, ck_tile::TailNumber::Two>{});
            }
        }
        if constexpr(BaseGemmPipeline::PrefetchStages > 3)
        {
            if(tail_num == ck_tile::TailNumber::Three)
            {
                Run(ck_tile::bool_constant<true>{},
                    ck_tile::integral_constant<ck_tile::TailNumber, ck_tile::TailNumber::Three>{});
            }
        }
        if constexpr(BaseGemmPipeline::PrefetchStages > 4)
        {
            if(tail_num == ck_tile::TailNumber::Four)
            {
                Run(ck_tile::bool_constant<true>{},
                    ck_tile::integral_constant<ck_tile::TailNumber, ck_tile::TailNumber::Four>{});
            }
        }
        if constexpr(BaseGemmPipeline::PrefetchStages > 5)
        {
            if(tail_num == ck_tile::TailNumber::Five)
            {
                Run(ck_tile::bool_constant<true>{},
                    ck_tile::integral_constant<ck_tile::TailNumber, ck_tile::TailNumber::Five>{});
            }
        }
        if constexpr(BaseGemmPipeline::PrefetchStages > 6)
        {
            if(tail_num == ck_tile::TailNumber::Six)
            {
                Run(ck_tile::bool_constant<true>{},
                    ck_tile::integral_constant<ck_tile::TailNumber, ck_tile::TailNumber::Six>{});
            }
        }
        if constexpr(BaseGemmPipeline::PrefetchStages > 7)
        {
            if(tail_num == ck_tile::TailNumber::Seven)
            {
                Run(ck_tile::bool_constant<true>{},
                    ck_tile::integral_constant<ck_tile::TailNumber, ck_tile::TailNumber::Seven>{});
            }
        }
    }
    else
    {
        if(tail_num == ck_tile::TailNumber::Full)
        {
            Run(ck_tile::bool_constant<false>{},
                ck_tile::integral_constant<ck_tile::TailNumber, ck_tile::TailNumber::Full>{});
        }
        else
        {
            std::ostringstream err;
            err << "When there's no hot loop, this tail number \"" << tail_num
                << "\" is not supported! " << __FILE__ << ":" << __LINE__
                << ", in function: " << __func__;
            throw std::runtime_error(err.str());
        }
    }

    return ave_time;
}

#define CK_TILE_GEMM_HIGH_PERF_ONLY_TN
#include "run_gemm_example.inc"

int main(int argc, char* argv[]) { return !run_gemm_example(argc, argv); }
