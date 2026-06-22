// SPDX-License-Identifier: MIT
// Copyright (c) 2026, Advanced Micro Devices, Inc. All rights reserved.

#pragma once

#include "jenga_bwd_dq_pipeline.hpp"
#include <string>

namespace ck_tile {
namespace example {
namespace jenga {

template <typename Pipeline_>
struct JengaBwdDqTilePartitioner
{
    using Pipeline = remove_cvref_t<Pipeline_>;

    CK_TILE_HOST static dim3 GridSize(const jenga_bwd_dq_args& a)
    {
        return dim3(integer_divide_ceil(a.seqlen_q, Pipeline::BlockM), a.batch * a.nhead, 1);
    }

    CK_TILE_DEVICE auto operator()() const
    {
        return make_tuple(static_cast<index_t>(blockIdx.x), static_cast<index_t>(blockIdx.y));
    }
};

template <typename Pipeline_>
struct JengaBwdDqKernel
{
    using Pipeline = remove_cvref_t<Pipeline_>;
    using TilePartitioner = JengaBwdDqTilePartitioner<Pipeline>;

    static constexpr index_t BlockM          = Pipeline::BlockM;
    static constexpr index_t ThreadsPerBlock = Pipeline::ThreadsPerBlock;
    static constexpr index_t KernelBlockSize = ThreadsPerBlock;

    CK_TILE_HOST static std::string GetName()
    {
        using QDataType = typename Pipeline::QDataType;
        return std::string("jenga_bwd_dq_") + JengaBwdDqTypeName<QDataType>::name + "_bm" +
               std::to_string(Pipeline::BlockM) + "_bn" + std::to_string(Pipeline::BlockN) +
               "_d" + std::to_string(Pipeline::HeadDim) + "_nnz" +
               std::to_string(Pipeline::MaxNnz) + "_" + Pipeline::name;
    }

    CK_TILE_HOST static auto MakeKargs(const jenga_bwd_dq_args& a) { return a; }

    CK_TILE_HOST static dim3 GridSize(const jenga_bwd_dq_args& a)
    {
        return TilePartitioner::GridSize(a);
    }

    CK_TILE_HOST static constexpr dim3 BlockSize() { return dim3(KernelBlockSize); }

    CK_TILE_HOST static constexpr dim3 BlockSize(const jenga_bwd_dq_args&) { return BlockSize(); }

    CK_TILE_HOST_DEVICE static constexpr index_t GetSmemSize() { return Pipeline::GetSmemSize(); }

    CK_TILE_DEVICE void operator()(const jenga_bwd_dq_args& a) const
    {
        using QDataType     = typename Pipeline::QDataType;
        using KDataType     = typename Pipeline::KDataType;
        using VDataType     = typename Pipeline::VDataType;
        using OGradDataType = typename Pipeline::OGradDataType;
        using DQDataType    = typename Pipeline::DQDataType;

        static constexpr index_t BlockN  = Pipeline::BlockN;
        static constexpr index_t HeadDim = Pipeline::HeadDim;

        const auto [i_m_block, i_bh] = TilePartitioner{}();
        const index_t i_m            = i_m_block * BlockM;

        const auto q_bh_view = make_naive_tensor_view<address_space_enum::global>(
            static_cast<const QDataType*>(a.q) + i_bh * a.stride_qz,
            make_tuple(a.seqlen_q, number<HeadDim>{}),
            make_tuple(a.stride_qm, a.stride_qk),
            number<1>{},
            number<8>{});
        const auto q_pad = pad_tensor_view(
            q_bh_view, make_tuple(number<BlockM>{}, number<HeadDim>{}), sequence<true, false>{});
        const auto q_block_window = make_tile_window(
            q_pad,
            make_tuple(number<BlockM>{}, number<HeadDim>{}),
            {i_m, 0});

        const auto k_dram = make_naive_tensor_view<address_space_enum::global>(
            static_cast<const KDataType*>(a.k),
            make_tuple(a.batch * a.nhead, a.seqlen_q, number<HeadDim>{}),
            make_tuple(a.stride_kz, a.stride_kn, a.stride_kk),
            number<1>{},
            number<8>{});

        const auto v_dram = make_naive_tensor_view<address_space_enum::global>(
            static_cast<const VDataType*>(a.v),
            make_tuple(a.batch * a.nhead, a.seqlen_q, number<HeadDim>{}),
            make_tuple(a.stride_vz, a.stride_vn, a.stride_vk),
            number<1>{},
            number<8>{});

        const auto dout_bh_view = make_naive_tensor_view<address_space_enum::global>(
            static_cast<const OGradDataType*>(a.dout) + i_bh * a.stride_doz,
            make_tuple(a.seqlen_q, number<HeadDim>{}),
            make_tuple(a.stride_dom, a.stride_dok),
            number<1>{},
            number<8>{});
        const auto dout_pad = pad_tensor_view(
            dout_bh_view, make_tuple(number<BlockM>{}, number<HeadDim>{}), sequence<true, false>{});
        const auto dout_block_window = make_tile_window(
            dout_pad,
            make_tuple(number<BlockM>{}, number<HeadDim>{}),
            {i_m, 0});

        const auto delta_dram = make_naive_tensor_view<address_space_enum::global>(
            static_cast<const float*>(a.deltas),
            make_tuple(a.batch * a.nhead, a.seqlen_q),
            make_tuple(a.stride_dz, a.stride_dm),
            number<1>{},
            number<1>{});
        const auto delta_pad = pad_tensor_view(
            delta_dram, make_tuple(number<1>{}, number<BlockM>{}), sequence<false, true>{});
        const auto delta_block_window = make_tile_window(
            delta_pad, make_tuple(number<1>{}, number<BlockM>{}), {i_bh, i_m});

        const auto lse_dram = make_naive_tensor_view<address_space_enum::global>(
            static_cast<const float*>(a.lse),
            make_tuple(a.batch * a.nhead, a.seqlen_q),
            make_tuple(a.stride_lz, a.stride_lm),
            number<1>{},
            number<1>{});
        const auto lse_pad = pad_tensor_view(
            lse_dram, make_tuple(number<1>{}, number<BlockM>{}), sequence<false, true>{});
        const auto lse_block_window =
            make_tile_window(lse_pad, make_tuple(number<1>{}, number<BlockM>{}), {i_bh, i_m});

        auto dq_bh_view = make_naive_tensor_view<address_space_enum::global>(
            static_cast<DQDataType*>(a.dq) + i_bh * a.stride_dqz,
            make_tuple(a.seqlen_q, number<HeadDim>{}),
            make_tuple(a.stride_dqm, a.stride_dqk),
            number<1>{},
            number<8>{});
        auto dq_pad = pad_tensor_view(
            dq_bh_view, make_tuple(number<BlockM>{}, number<HeadDim>{}), sequence<true, false>{});
        auto dq_block_window = make_tile_window(
            dq_pad,
            make_tuple(number<BlockM>{}, number<HeadDim>{}),
            {i_m, 0});

        __shared__ char smem[GetSmemSize()];

        Pipeline{}(q_block_window,
                   k_dram,
                   v_dram,
                   dout_block_window,
                   delta_block_window,
                   lse_block_window,
                   dq_block_window,
                   a,
                   smem);
    }
};

} // namespace jenga
} // namespace example
} // namespace ck_tile
