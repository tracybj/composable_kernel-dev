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

    if(args.D != Kernel::HeadDim || args.N_Q_BLOCKS <= 0 || args.N_KV_BLOCKS <= 0)
    {
        throw std::runtime_error(
            "jenga_bwd_dkdv_pipeline supports D=128 and non-empty grids");
    }

    const dim3 grids      = Kernel::GridSize(args.N_KV_BLOCKS, args.B * args.H);
    constexpr dim3 blocks = Kernel::BlockSize();

    if(s.log_level_ > 0)
    {
        std::cout << "Launching jenga_bwd_dkdv_pipeline grid: {" << grids.x << ", " << grids.y
                  << ", " << grids.z << "}, block: {" << blocks.x << ", " << blocks.y << ", "
                  << blocks.z << "}" << std::endl;
    }

    return ck_tile::launch_kernel(
        s,
        ck_tile::make_kernel<Kernel::ThreadsPerBlock, 1>(
            Kernel{}, grids, blocks, 0, args));
}

#include "run_jenga_bwd_dkdv_example.inc"

int main(int argc, char* argv[]) { return !run_jenga_bwd_dkdv_example(argc, argv); }
