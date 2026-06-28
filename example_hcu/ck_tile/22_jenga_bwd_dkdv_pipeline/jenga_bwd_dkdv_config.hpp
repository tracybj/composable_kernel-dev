// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2026, Advanced Micro Devices, Inc. All rights reserved.

#pragma once

#include "ck_tile/core.hpp"

namespace ck_tile {
namespace example {
namespace jenga {

template <typename QDataType_,
          typename KDataType_,
          typename VDataType_,
          typename OGradDataType_,
          typename KGradDataType_,
          typename VGradDataType_,
          typename AccDataType_,
          typename LSEDataType_,
          index_t BlockM_,
          index_t BlockN_,
          index_t HeadDim_,
          index_t ThreadsPerBlock_>
struct JengaBwdDkdvProblem
{
    using QDataType     = remove_cvref_t<QDataType_>;
    using KDataType     = remove_cvref_t<KDataType_>;
    using VDataType     = remove_cvref_t<VDataType_>;
    using OGradDataType = remove_cvref_t<OGradDataType_>;
    using KGradDataType = remove_cvref_t<KGradDataType_>;
    using VGradDataType = remove_cvref_t<VGradDataType_>;
    using AccDataType   = remove_cvref_t<AccDataType_>;
    using LSEDataType   = remove_cvref_t<LSEDataType_>;

    static constexpr index_t BlockM          = BlockM_;
    static constexpr index_t BlockN          = BlockN_;
    static constexpr index_t HeadDim         = HeadDim_;
    static constexpr index_t ThreadsPerBlock = ThreadsPerBlock_;
};

struct jenga_bwd_dkdv_args
{
    const void* q_ptr;
    const void* k_ptr;
    const void* v_ptr;
    const void* do_ptr;
    const void* delta_ptr;
    const void* lse_ptr;
    const void* lse_text_ptr;
    const int32_t* rlut_ptr;
    const int32_t* rlut_size_ptr;
    const int32_t* seqlens_ptr;
    void* dk_ptr;
    void* dv_ptr;

    index_t B;
    index_t H;
    index_t N_Q;
    index_t N_KV;
    index_t D;
    index_t N_Q_BLOCKS;
    index_t N_KV_BLOCKS;
    index_t M0;
    index_t N0;
    index_t max_nnz_r;
    index_t text_block_start;
    float sm_scale;
    float text_amp;

    index_t stride_qz;
    index_t stride_qh;
    index_t stride_qm;
    index_t stride_qd;
    index_t stride_kz;
    index_t stride_kh;
    index_t stride_kn;
    index_t stride_kd;
    index_t stride_vz;
    index_t stride_vh;
    index_t stride_vn;
    index_t stride_vd;
    index_t stride_oz;
    index_t stride_oh;
    index_t stride_om;
    index_t stride_od;
    index_t stride_doz;
    index_t stride_doh;
    index_t stride_dom;
    index_t stride_dod;
    index_t stride_delta_z;
    index_t stride_delta_m;
    index_t stride_dkz;
    index_t stride_dkh;
    index_t stride_dkn;
    index_t stride_dkd;
    index_t stride_dvz;
    index_t stride_dvh;
    index_t stride_dvn;
    index_t stride_dvd;
    index_t stride_lz;
    index_t stride_lm;
    index_t stride_bz;
    index_t stride_bm;
    index_t stride_bn;
    index_t stride_rlutz;
    index_t stride_rlutn;
    index_t stride_rlutk;
    index_t stride_rlut_size_z;
    index_t stride_rlut_size_n;
};

} // namespace jenga
} // namespace example
} // namespace ck_tile
