// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2026, Advanced Micro Devices, Inc. All rights reserved.

#include <hip/hip_runtime.h>
#include <iostream>
#include <stdexcept>

#include "ck_tile/core.hpp"
#include "ck_tile/host.hpp"
#include "jenga_bwd_dkdv.hpp"
#include "jenga_bwd_dkdv_config.hpp"
#include "jenga_bwd_dkdv_policy.hpp"
#include "jenga_bwd_dkdv_pipeline.hpp"
#include "jenga_bwd_dkdv_kernel.hpp"

float jenga_bwd_dkdv_pipeline_calc(const ck_tile::example::jenga::jenga_bwd_dkdv_args& args,
                                    const ck_tile::stream_config& s)
{
    using Problem = ck_tile::example::jenga::JengaBwdDkdvProblem<
        QDataType,
        KDataType,
        VDataType,
        OGradDataType,
        KGradDataType,
        VGradDataType,
        AccDataType,
        LSEDataType,
        64,  // BlockM
        64,  // BlockN
        128, // HeadDim
        28,  // dummy MaxNnz
        256  // ThreadsPerBlock
    >;

    using Policy = ck_tile::example::jenga::JengaBwdDkdvDefaultPolicy<Problem>;
    using Pipeline = ck_tile::example::jenga::JengaBwdDkdvPipeline<Problem, Policy>;
    using Kernel = ck_tile::example::jenga::JengaBwdDkdvKernel<Pipeline>;
    using FullTilePipeline = ck_tile::example::jenga::JengaBwdDkdvPipeline<Problem, Policy, true>;
    using FullTileKernel = ck_tile::example::jenga::JengaBwdDkdvKernel<FullTilePipeline>;

    if(args.D != Kernel::HeadDim || args.N_Q_BLOCKS <= 0 || args.N_KV_BLOCKS <= 0)
    {
        throw std::runtime_error(
            "jenga_bwd_dkdv_pipeline supports D=128 and non-empty grids");
    }

    const dim3 grids      = Kernel::GridSize(args.N_KV_BLOCKS, args.B * args.H);
    constexpr dim3 blocks = Kernel::BlockSize();
    const bool full_tile_supported =
        args.B == 1 && args.N_Q == args.N_KV && args.D == Kernel::HeadDim &&
        args.N_Q % Kernel::BlockM == 0 && args.N_KV % Kernel::BlockN == 0 &&
        args.stride_qm % 8 == 0 && args.stride_qd == 1 && args.stride_kn % 8 == 0 &&
        args.stride_kd == 1 && args.stride_vn % 8 == 0 && args.stride_vd == 1 &&
        args.stride_dom % 8 == 0 && args.stride_dod == 1 && args.stride_dvn % 8 == 0 &&
        args.stride_dvd == 1 && args.stride_dkn % 8 == 0 && args.stride_dkd == 1 &&
        reinterpret_cast<std::uintptr_t>(args.q_ptr) % 16 == 0 &&
        reinterpret_cast<std::uintptr_t>(args.k_ptr) % 16 == 0 &&
        reinterpret_cast<std::uintptr_t>(args.v_ptr) % 16 == 0 &&
        reinterpret_cast<std::uintptr_t>(args.do_ptr) % 16 == 0 &&
        reinterpret_cast<std::uintptr_t>(args.dk_ptr) % 16 == 0 &&
        reinterpret_cast<std::uintptr_t>(args.dv_ptr) % 16 == 0;

    if(s.log_level_ > 0)
    {
        std::cout << "Launching jenga_bwd_dkdv_pipeline grid: {" << grids.x << ", " << grids.y
                  << ", " << grids.z << "}, block: {" << blocks.x << ", " << blocks.y << ", "
                  << blocks.z << "}, full_tile=" << (full_tile_supported ? 1 : 0) << std::endl;
    }

    if(full_tile_supported)
    {
        return ck_tile::launch_kernel(
            s,
            ck_tile::make_kernel<FullTileKernel::ThreadsPerBlock, 1>(
                FullTileKernel{}, grids, blocks, 0, args));
    }

    return ck_tile::launch_kernel(
        s,
        ck_tile::make_kernel<Kernel::ThreadsPerBlock, 1>(
            Kernel{}, grids, blocks, 0, args));
}

#include "run_jenga_bwd_dkdv_example.inc"

int main(int argc, char* argv[]) { return !run_jenga_bwd_dkdv_example(argc, argv); }
