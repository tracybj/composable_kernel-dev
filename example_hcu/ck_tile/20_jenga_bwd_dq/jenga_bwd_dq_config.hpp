// SPDX-License-Identifier: MIT
// Copyright (c) 2026, Advanced Micro Devices, Inc. All rights reserved.

#pragma once

#include "ck_tile/core.hpp"
#include <string>

namespace ck_tile {
namespace example {
namespace jenga {

template <typename QDataType_,
          typename KDataType_,
          typename VDataType_,
          typename OGradDataType_,
          typename DQDataType_,
          typename AccDataType_,
          index_t BlockM_,
          index_t BlockN_,
          index_t HeadDim_,
          index_t MaxNnz_,
          index_t ThreadsPerBlock_>
struct JengaBwdDqProblem
{
    using QDataType     = remove_cvref_t<QDataType_>;
    using KDataType     = remove_cvref_t<KDataType_>;
    using VDataType     = remove_cvref_t<VDataType_>;
    using OGradDataType = remove_cvref_t<OGradDataType_>;
    using DQDataType    = remove_cvref_t<DQDataType_>;
    using AccDataType   = remove_cvref_t<AccDataType_>;

    static constexpr index_t BlockM          = BlockM_;
    static constexpr index_t BlockN          = BlockN_;
    static constexpr index_t HeadDim         = HeadDim_;
    static constexpr index_t MaxNnz          = MaxNnz_;
    static constexpr index_t ThreadsPerBlock = ThreadsPerBlock_;
};

struct jenga_bwd_dq_traits
{
    std::string data_type = "bf16";
    index_t block_m      = 64;
    index_t block_n      = 64;
    index_t head_dim     = 128;
    index_t max_nnz      = 28;
};

struct jenga_bwd_dq_args
{
    const void* q;
    const void* k;
    const void* v;
    const void* dout;
    const float* deltas;
    const float* lse;
    void* dq;
    const int* lut;
    const int* lut_size;
    const int* seqlens;

    float sm_scale;
    float text_amp;
    int text_block_start;

    index_t stride_qz;
    index_t stride_qm;
    index_t stride_qk;
    index_t stride_kz;
    index_t stride_kn;
    index_t stride_kk;
    index_t stride_vz;
    index_t stride_vn;
    index_t stride_vk;
    index_t stride_doz;
    index_t stride_dom;
    index_t stride_dok;
    index_t stride_dqz;
    index_t stride_dqm;
    index_t stride_dqk;
    index_t stride_dz;
    index_t stride_dm;
    index_t stride_lz;
    index_t stride_lm;
    index_t stride_lutz;
    index_t stride_lutm;
    index_t stride_lutk;

    index_t block_m;
    index_t block_n;
    index_t max_nnz;
    index_t batch;
    index_t nhead;
    index_t seqlen_q;
};

template <typename T>
struct JengaBwdDqTypeName;

template <>
struct JengaBwdDqTypeName<half_t>
{
    static constexpr const char* name = "fp16";
};

template <>
struct JengaBwdDqTypeName<bf16_t>
{
    static constexpr const char* name = "bf16";
};

} // namespace jenga
} // namespace example
} // namespace ck_tile
