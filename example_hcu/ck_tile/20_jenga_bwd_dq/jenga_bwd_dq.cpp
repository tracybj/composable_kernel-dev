// SPDX-License-Identifier: MIT
// Copyright (c) 2026, Advanced Micro Devices, Inc. All rights reserved.

#include "jenga_bwd_dq.hpp"
#include <iostream>

namespace jenga = ck_tile::example::jenga;

#define JENGA_BWD_DQ_DISPATCH_WITH_PIPELINE(dtype_, block_m_, block_n_, head_dim_, max_nnz_, nthreads_, fast_only_) \
    do                                                                                                             \
    {                                                                                                              \
        using data_t = dtype_;                                                                                     \
        using problem = jenga::JengaBwdDqProblem<data_t,                                                           \
                                                 data_t,                                                           \
                                                 data_t,                                                           \
                                                 data_t,                                                           \
                                                 data_t,                                                           \
                                                 float,                                                            \
                                                 block_m_,                                                         \
                                                 block_n_,                                                         \
                                                 head_dim_,                                                        \
                                                 max_nnz_,                                                         \
                                                 nthreads_>;                                                       \
        using pipeline =                                                                                           \
            jenga::JengaBwdDqPipeline<problem, jenga::JengaBwdDqDefaultPolicy, fast_only_>;                        \
        using kernel   = jenga::JengaBwdDqKernel<pipeline>;                                                        \
        auto kargs     = kernel::MakeKargs(a);                                                                     \
        const dim3 grids     = kernel::GridSize(a);                                                                \
        const dim3 blocks    = kernel::BlockSize(a);                                                               \
        const auto lds_bytes = kernel::GetSmemSize();                                                              \
        if(s.log_level_ > 0)                                                                                       \
        {                                                                                                          \
            std::cout << "Launching kernel: " << kernel::GetName() << " with args:"                                \
                      << " grid: {" << grids.x << ", " << grids.y << ", " << grids.z                            \
                      << "}"                                                                                      \
                      << ", blocks: {" << blocks.x << ", " << blocks.y << ", "                                 \
                      << blocks.z << "}" << std::endl;                                                            \
        }                                                                                                          \
        return ck_tile::launch_kernel(                                                                             \
            s, ck_tile::make_kernel<kernel::ThreadsPerBlock>(                                                      \
                   kernel{}, grids, blocks, lds_bytes, kargs));                                                    \
    } while(false)

#define JENGA_BWD_DQ_DISPATCH(dtype_, block_m_, block_n_, head_dim_, max_nnz_, nthreads_)       \
    do                                                                                          \
    {                                                                                           \
        const bool use_fast_only = a.seqlen_q % block_m_ == 0 && a.seqlen_q % block_n_ == 0 &&  \
                                   a.seqlen_q / block_n_ >= max_nnz_;                           \
        if(use_fast_only)                                                                       \
        {                                                                                       \
            JENGA_BWD_DQ_DISPATCH_WITH_PIPELINE(                                                \
                dtype_, block_m_, block_n_, head_dim_, max_nnz_, nthreads_, true);              \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            JENGA_BWD_DQ_DISPATCH_WITH_PIPELINE(                                                \
                dtype_, block_m_, block_n_, head_dim_, max_nnz_, nthreads_, false);             \
        }                                                                                       \
    } while(false)



#define JENGA_BWD_DQ_DISPATCH_NNZ(dtype_, block_m_, block_n_, nthreads_) \
    do                                                                 \
    {                                                                  \
        if(t.max_nnz == 4)                                             \
            JENGA_BWD_DQ_DISPATCH(dtype_, block_m_, block_n_, 128, 4, nthreads_); \
        if(t.max_nnz == 28)                                             \
            JENGA_BWD_DQ_DISPATCH(dtype_, block_m_, block_n_, 128, 28, nthreads_); \
        if(t.max_nnz == 118)                                             \
            JENGA_BWD_DQ_DISPATCH(dtype_, block_m_, block_n_, 128, 118, nthreads_); \
    } while(false)

float jenga_bwd_dq(jenga::jenga_bwd_dq_traits t, jenga::jenga_bwd_dq_args a, ck_tile::stream_config s)
{
    if(t.block_m == 64 && t.block_n == 64 && t.head_dim == 128)
    {
        if(t.data_type == "fp16")
        {
            JENGA_BWD_DQ_DISPATCH_NNZ(ck_tile::half_t, 64, 64, 512);
        }
        if(t.data_type == "bf16")
        {
            JENGA_BWD_DQ_DISPATCH_NNZ(ck_tile::bf16_t, 64, 64, 512);
        }
    }

    std::cerr << "unsupported jenga_bwd_dq config: dtype=" << t.data_type
              << " block_m=" << t.block_m << " block_n=" << t.block_n
              << " head_dim=" << t.head_dim << " max_nnz=" << t.max_nnz << std::endl;
    return -1.0f;
}
