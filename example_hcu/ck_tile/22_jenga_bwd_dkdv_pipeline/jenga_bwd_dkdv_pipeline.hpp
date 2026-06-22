// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2026, Advanced Micro Devices, Inc. All rights reserved.

#pragma once

#include "ck_tile/core.hpp"
#include "jenga_bwd_dkdv_config.hpp"
#include "jenga_bwd_dkdv_policy.hpp"

namespace ck_tile {
namespace example {
namespace jenga {

template <typename Problem, typename Policy = JengaBwdDkdvDefaultPolicy<Problem>>
struct JengaBwdDkdvPipeline
{
    using QDataType     = typename Problem::QDataType;
    using KDataType     = typename Problem::KDataType;
    using VDataType     = typename Problem::VDataType;
    using OGradDataType = typename Problem::OGradDataType;
    using KGradDataType = typename Problem::KGradDataType;
    using VGradDataType = typename Problem::VGradDataType;
    using AccDataType   = typename Problem::AccDataType;
    using LSEDataType   = typename Problem::LSEDataType;

    static constexpr index_t BlockM          = Problem::BlockM;
    static constexpr index_t BlockN          = Problem::BlockN;
    static constexpr index_t HeadDim         = Problem::HeadDim;
    static constexpr index_t MaxNnz          = Problem::MaxNnz;
    static constexpr index_t ThreadsPerBlock = Problem::ThreadsPerBlock;

    CK_TILE_DEVICE static void run(const jenga_bwd_dkdv_args& args,
                                   ck_tile::index_t kv_block,
                                   ck_tile::index_t off_hz,
                                   ck_tile::index_t batch,
                                   ck_tile::index_t head,
                                   ck_tile::index_t seqlen)
    {
        constexpr auto qk_gemm = typename Policy::QKBlockGemm{};
        constexpr auto dv_gemm = typename Policy::DVBlockGemm{};
        constexpr auto dp_gemm = typename Policy::DPBlockGemm{};
        constexpr auto dk_gemm = typename Policy::DKBlockGemm{};
        const ck_tile::index_t start_n = kv_block * BlockN;

        const QDataType* q_ptr =
            static_cast<const QDataType*>(args.q_ptr) +
            static_cast<ck_tile::long_index_t>(batch) * args.stride_qz +
            static_cast<ck_tile::long_index_t>(head) * args.stride_qh;
        const KDataType* k_ptr =
            static_cast<const KDataType*>(args.k_ptr) +
            static_cast<ck_tile::long_index_t>(batch) * args.stride_kz +
            static_cast<ck_tile::long_index_t>(head) * args.stride_kh;
        const OGradDataType* do_ptr =
            static_cast<const OGradDataType*>(args.do_ptr) +
            static_cast<ck_tile::long_index_t>(batch) * args.stride_doz +
            static_cast<ck_tile::long_index_t>(head) * args.stride_doh;
        const VDataType* v_ptr =
            static_cast<const VDataType*>(args.v_ptr) +
            static_cast<ck_tile::long_index_t>(batch) * args.stride_vz +
            static_cast<ck_tile::long_index_t>(head) * args.stride_vh;
        const AccDataType* delta_ptr =
            static_cast<const AccDataType*>(args.delta_ptr) +
            static_cast<ck_tile::long_index_t>(off_hz) * args.stride_delta_z;
        KGradDataType* dk_ptr =
            static_cast<KGradDataType*>(args.dk_ptr) +
            static_cast<ck_tile::long_index_t>(batch) * args.stride_dkz +
            static_cast<ck_tile::long_index_t>(head) * args.stride_dkh;
        VGradDataType* dv_ptr =
            static_cast<VGradDataType*>(args.dv_ptr) +
            static_cast<ck_tile::long_index_t>(batch) * args.stride_dvz +
            static_cast<ck_tile::long_index_t>(head) * args.stride_dvh;

        const int32_t* rlut_ptr =
            args.rlut_ptr + static_cast<ck_tile::long_index_t>(off_hz) * args.stride_rlutz +
            static_cast<ck_tile::long_index_t>(kv_block) * args.stride_rlutn;
        const int32_t num_active =
            args.rlut_size_ptr[static_cast<ck_tile::long_index_t>(off_hz) *
                                   args.stride_rlut_size_z +
                               static_cast<ck_tile::long_index_t>(kv_block) *
                                   args.stride_rlut_size_n];
        if(num_active <= 0)
        {
            return;
        }

        const bool is_text_block = kv_block >= args.text_block_start;
        const LSEDataType* lse_ptr =
            static_cast<const LSEDataType*>(is_text_block ? args.lse_text_ptr : args.lse_ptr) +
            static_cast<ck_tile::long_index_t>(off_hz) * args.stride_lz;
        constexpr float qk_scale = ck_tile::log2e_v<float>;
        const float scale_log2e  = args.sm_scale * qk_scale;

        __shared__ typename Policy::LdsStorage smem;

        const bool block_is_boundary = (start_n + BlockN > seqlen) || (args.D < HeadDim);
        const bool block_can_vectorize_transposed_do = (args.stride_dom % 8 == 0) && (args.stride_dod == 1) && (reinterpret_cast<std::uintptr_t>(do_ptr) % 16 == 0);
        const bool block_can_vectorize_transposed_q  = (args.stride_qm % 8 == 0) && (args.stride_qd == 1) && (reinterpret_cast<std::uintptr_t>(q_ptr) % 16 == 0);

        auto dv_acc = decltype(dv_gemm.MakeCBlockTile()){};
        auto dk_acc = decltype(dk_gemm.MakeCBlockTile()){};
        ck_tile::clear_tile(dv_acc);
        ck_tile::clear_tile(dk_acc);

        QDataType* q_smem = smem.qk_input.q;
        KDataType* k_smem = smem.qk_input.k;

        for(ck_tile::index_t active_i = 0; active_i < args.max_nnz_r; ++active_i)
        {
            if(active_i >= num_active)
            {
                break;
            }

            const ck_tile::index_t qb =
                rlut_ptr[static_cast<ck_tile::long_index_t>(active_i) * args.stride_rlutk];
            const ck_tile::index_t q_start = qb * BlockM;
            if(q_start >= args.N_Q)
            {
                continue;
            }

            const bool is_boundary = block_is_boundary || (q_start + BlockM > seqlen);
            if(!is_boundary)
            {
                const int4* q_ptr_vec = reinterpret_cast<const int4*>(q_ptr + static_cast<ck_tile::long_index_t>(q_start) * args.stride_qm);
                int4* q_smem_vec = reinterpret_cast<int4*>(q_smem);
                #pragma unroll
                for(int i = 0; i < 8; ++i)
                {
                    q_smem_vec[threadIdx.x + i * 256] = q_ptr_vec[threadIdx.x + i * 256];
                }

                const int4* k_ptr_vec = reinterpret_cast<const int4*>(k_ptr + static_cast<ck_tile::long_index_t>(start_n) * args.stride_kn);
                int4* k_smem_vec = reinterpret_cast<int4*>(k_smem);
                #pragma unroll
                for(int i = 0; i < 4; ++i)
                {
                    k_smem_vec[threadIdx.x + i * 256] = k_ptr_vec[threadIdx.x + i * 256];
                }
            }
            else
            {
                for(ck_tile::index_t linear = threadIdx.x; linear < BlockM * HeadDim;
                    linear += blockDim.x)
                {
                    const ck_tile::index_t m_rel = linear / HeadDim;
                    const ck_tile::index_t d     = linear - m_rel * HeadDim;
                    const ck_tile::index_t m     = q_start + m_rel;
                    q_smem[linear] =
                        (m < args.N_Q && m < seqlen && d < args.D)
                            ? q_ptr[static_cast<ck_tile::long_index_t>(m) * args.stride_qm +
                                    static_cast<ck_tile::long_index_t>(d) * args.stride_qd]
                            : ck_tile::type_convert<QDataType>(0.0f);
                }

                for(ck_tile::index_t linear = threadIdx.x; linear < BlockN * HeadDim;
                    linear += blockDim.x)
                {
                    const ck_tile::index_t n_rel = linear / HeadDim;
                    const ck_tile::index_t d     = linear - n_rel * HeadDim;
                    const ck_tile::index_t n     = start_n + n_rel;
                    k_smem[linear] =
                        (n < seqlen && d < args.D)
                            ? k_ptr[static_cast<ck_tile::long_index_t>(n) * args.stride_kn +
                                    static_cast<ck_tile::long_index_t>(d) * args.stride_kd]
                            : ck_tile::type_convert<KDataType>(0.0f);
                }
            }

            ck_tile::block_sync_lds();

            auto q_lds_view = ck_tile::make_tensor_view<ck_tile::address_space_enum::lds>(
                q_smem, MakeSimpleLdsDescriptor<BlockM, HeadDim>());
            auto k_lds_view = ck_tile::make_tensor_view<ck_tile::address_space_enum::lds>(
                k_smem, MakeSimpleLdsDescriptor<BlockN, HeadDim>());

            auto q_lds_window = ck_tile::make_tile_window(
                q_lds_view,
                ck_tile::make_tuple(ck_tile::number<BlockM>{}, ck_tile::number<HeadDim>{}),
                {0, 0});
            auto k_lds_window = ck_tile::make_tile_window(
                k_lds_view,
                ck_tile::make_tuple(ck_tile::number<BlockN>{}, ck_tile::number<HeadDim>{}),
                {0, 0});

            auto qk_acc = decltype(qk_gemm.MakeCBlockTile()){};
            ck_tile::clear_tile(qk_acc);
            qk_gemm(qk_acc, q_lds_window, k_lds_window);
            ck_tile::block_sync_lds();

            QDataType* p_t_smem      = smem.dv_input.p_t;
            OGradDataType* do_t_smem = smem.dv_input.do_t;

            const auto qk_out = qk_gemm.MakeOuputLayout(qk_acc);
            constexpr auto qk_spans = decltype(qk_out)::get_distributed_spans();

            constexpr auto M_spans = qk_spans[ck_tile::number<0>{}];
            float lse_regs[decltype(M_spans)::Impl::size()];
            float delta_regs[decltype(M_spans)::Impl::size()];

            ck_tile::sweep_tile_span(M_spans, [&](auto idx0) {
                constexpr auto dummy_tile_idx = ck_tile::make_tuple(idx0, ck_tile::tile_distributed_index<1, 0, 0, 0>{});
                const auto x_idx = ck_tile::get_x_indices_from_distributed_indices(
                    qk_out.get_tile_distribution(), dummy_tile_idx);
                const ck_tile::index_t m_rel = x_idx.at(ck_tile::number<0>{});
                const ck_tile::index_t m     = q_start + m_rel;
                const bool row_valid         = m < args.N_Q && m < seqlen;
                lse_regs[decltype(idx0)::Impl::at(0)] = row_valid ? lse_ptr[static_cast<ck_tile::long_index_t>(m) * args.stride_lm]
                                           : 0.0f;
                delta_regs[decltype(idx0)::Impl::at(0)] = row_valid ? delta_ptr[static_cast<ck_tile::long_index_t>(m) * args.stride_delta_m]
                                             : 0.0f;
            });

            ck_tile::sweep_tile_span(M_spans, [&](auto idx0) {
                constexpr auto dummy_tile_idx = ck_tile::make_tuple(idx0, ck_tile::tile_distributed_index<1, 0, 0, 0>{});
                const auto x_idx = ck_tile::get_x_indices_from_distributed_indices(
                    qk_out.get_tile_distribution(), dummy_tile_idx);
                const ck_tile::index_t m_rel = x_idx.at(ck_tile::number<0>{});
                const ck_tile::index_t m     = q_start + m_rel;
                const bool row_valid         = m < args.N_Q && m < seqlen;
                const float row_lse_log2     = lse_regs[decltype(idx0)::Impl::at(0)];

                ck_tile::sweep_tile_span(qk_spans[ck_tile::number<1>{}], [&](auto idx1) {
                    constexpr auto tile_idx = ck_tile::make_tuple(idx0, idx1);
                    const auto x_idx_inner = ck_tile::get_x_indices_from_distributed_indices(
                        qk_out.get_tile_distribution(), tile_idx);
                    const ck_tile::index_t n_rel = x_idx_inner.at(ck_tile::number<1>{});
                    const ck_tile::index_t n     = start_n + n_rel;
                    const ck_tile::index_t mask_qb = (args.stride_bz / args.stride_bm > args.N_Q_BLOCKS)
                                                     ? (qb * 2 + (m_rel >= 64 ? 1 : 0))
                                                     : qb;
                    const size_t mask_idx = static_cast<size_t>(off_hz) * args.stride_bz +
                                             static_cast<size_t>(mask_qb) * args.stride_bm +
                                             static_cast<size_t>(kv_block) * args.stride_bn;
                    const bool is_active = args.block_mask_ptr[mask_idx] != 0;
                    const bool valid             = row_valid && n < seqlen && is_active;
                    const float p =
                        valid ? ck_tile::exp2(qk_out[tile_idx] * scale_log2e - row_lse_log2) : 0.0f;
                    p_t_smem[static_cast<ck_tile::long_index_t>(n_rel) * BlockM + m_rel] =
                        ck_tile::type_convert<QDataType>(p);
                });
            });

            const bool can_vectorize_transposed_do = !is_boundary && block_can_vectorize_transposed_do;
            if(can_vectorize_transposed_do)
            {
                const int thread_m_rel = threadIdx.x >> 4;
                const int thread_d     = (threadIdx.x & 15) << 3;
                #pragma unroll
                for(int i = 0; i < 8; ++i)
                {
                    int m_rel = thread_m_rel + i * 16;
                    int m = q_start + m_rel;
                    const int4* do_ptr_vec = reinterpret_cast<const int4*>(do_ptr + static_cast<ck_tile::long_index_t>(m) * args.stride_dom + thread_d);
                    int4 val = *do_ptr_vec;
                    const OGradDataType* val_bf16 = reinterpret_cast<const OGradDataType*>(&val);
                    #pragma unroll
                    for(int j = 0; j < 8; ++j)
                    {
                        do_t_smem[(thread_d + j) * BlockM + m_rel] = val_bf16[j];
                    }
                }
            }
            else
            {
                for(ck_tile::index_t linear = threadIdx.x; linear < HeadDim * BlockM;
                    linear += blockDim.x)
                {
                    const ck_tile::index_t d     = linear / BlockM;
                    const ck_tile::index_t m_rel = linear - d * BlockM;
                    const ck_tile::index_t m     = q_start + m_rel;
                    do_t_smem[linear] =
                        (m < args.N_Q && m < seqlen && d < args.D)
                            ? do_ptr[static_cast<ck_tile::long_index_t>(m) * args.stride_dom +
                                     static_cast<ck_tile::long_index_t>(d) * args.stride_dod]
                            : ck_tile::type_convert<OGradDataType>(0.0f);
                }
            }

            ck_tile::block_sync_lds();

            auto p_t_lds_view = ck_tile::make_tensor_view<ck_tile::address_space_enum::lds>(
                p_t_smem, MakeSimpleLdsDescriptor<BlockN, BlockM>());
            auto do_t_lds_view = ck_tile::make_tensor_view<ck_tile::address_space_enum::lds>(
                do_t_smem, MakeSimpleLdsDescriptor<HeadDim, BlockM>());

            auto p_t_lds_window = ck_tile::make_tile_window(
                p_t_lds_view,
                ck_tile::make_tuple(ck_tile::number<BlockN>{}, ck_tile::number<BlockM>{}),
                {0, 0});
            auto do_t_lds_window = ck_tile::make_tile_window(
                do_t_lds_view,
                ck_tile::make_tuple(ck_tile::number<HeadDim>{}, ck_tile::number<BlockM>{}),
                {0, 0});

            dv_gemm(dv_acc, p_t_lds_window, do_t_lds_window);

            ck_tile::block_sync_lds();

            auto dp_do_smem = reinterpret_cast<OGradDataType*>(smem.qk_input.q);
            auto v_smem     = reinterpret_cast<VDataType*>(smem.qk_input.k);

            if(!is_boundary)
            {
                const int4* dp_do_ptr_vec = reinterpret_cast<const int4*>(do_ptr + static_cast<ck_tile::long_index_t>(q_start) * args.stride_dom);
                int4* dp_do_smem_vec = reinterpret_cast<int4*>(dp_do_smem);
                #pragma unroll
                for(int i = 0; i < 8; ++i)
                {
                    dp_do_smem_vec[threadIdx.x + i * 256] = dp_do_ptr_vec[threadIdx.x + i * 256];
                }

                const int4* v_ptr_vec = reinterpret_cast<const int4*>(v_ptr + static_cast<ck_tile::long_index_t>(start_n) * args.stride_vn);
                int4* v_smem_vec = reinterpret_cast<int4*>(v_smem);
                #pragma unroll
                for(int i = 0; i < 4; ++i)
                {
                    v_smem_vec[threadIdx.x + i * 256] = v_ptr_vec[threadIdx.x + i * 256];
                }
            }
            else
            {
                for(ck_tile::index_t linear = threadIdx.x; linear < BlockM * HeadDim;
                    linear += blockDim.x)
                {
                    const ck_tile::index_t m_rel = linear / HeadDim;
                    const ck_tile::index_t d     = linear - m_rel * HeadDim;
                    const ck_tile::index_t m     = q_start + m_rel;
                    dp_do_smem[linear] =
                        (m < args.N_Q && m < seqlen && d < args.D)
                            ? do_ptr[static_cast<ck_tile::long_index_t>(m) * args.stride_dom +
                                     static_cast<ck_tile::long_index_t>(d) * args.stride_dod]
                            : ck_tile::type_convert<OGradDataType>(0.0f);
                }

                for(ck_tile::index_t linear = threadIdx.x; linear < BlockN * HeadDim;
                    linear += blockDim.x)
                {
                    const ck_tile::index_t n_rel = linear / HeadDim;
                    const ck_tile::index_t d     = linear - n_rel * HeadDim;
                    const ck_tile::index_t n     = start_n + n_rel;
                    v_smem[linear] =
                        (n < seqlen && d < args.D)
                            ? v_ptr[static_cast<ck_tile::long_index_t>(n) * args.stride_vn +
                                    static_cast<ck_tile::long_index_t>(d) * args.stride_vd]
                            : ck_tile::type_convert<VDataType>(0.0f);
                }
            }

            ck_tile::block_sync_lds();

            auto dp_do_lds_view = ck_tile::make_tensor_view<ck_tile::address_space_enum::lds>(
                dp_do_smem, MakeSimpleLdsDescriptor<BlockM, HeadDim>());
            auto v_lds_view = ck_tile::make_tensor_view<ck_tile::address_space_enum::lds>(
                v_smem, MakeSimpleLdsDescriptor<BlockN, HeadDim>());

            auto dp_do_lds_window = ck_tile::make_tile_window(
                dp_do_lds_view,
                ck_tile::make_tuple(ck_tile::number<BlockM>{}, ck_tile::number<HeadDim>{}),
                {0, 0});
            auto v_lds_window = ck_tile::make_tile_window(
                v_lds_view,
                ck_tile::make_tuple(ck_tile::number<BlockN>{}, ck_tile::number<HeadDim>{}),
                {0, 0});

            auto dp_acc = decltype(dp_gemm.MakeCBlockTile()){};
            ck_tile::clear_tile(dp_acc);
            dp_gemm(dp_acc, dp_do_lds_window, v_lds_window);
            const auto dp_out       = dp_gemm.MakeOuputLayout(dp_acc);
            constexpr auto dp_spans = decltype(dp_out)::get_distributed_spans();

            ck_tile::block_sync_lds();

            QDataType* ds_t_smem = smem.dk_input.ds_t;
            QDataType* q_t_smem  = smem.dk_input.q_t;

            ck_tile::sweep_tile_span(dp_spans[ck_tile::number<0>{}], [&](auto idx0) {
                constexpr auto dummy_tile_idx = ck_tile::make_tuple(idx0, ck_tile::tile_distributed_index<1, 0, 0, 0>{});
                const auto x_idx = ck_tile::get_x_indices_from_distributed_indices(
                    dp_out.get_tile_distribution(), dummy_tile_idx);
                const ck_tile::index_t m_rel = x_idx.at(ck_tile::number<0>{});
                const ck_tile::index_t m     = q_start + m_rel;
                const bool row_valid         = m < args.N_Q && m < seqlen;
                const float row_lse_log2     = lse_regs[decltype(idx0)::Impl::at(0)];
                const float delta            = delta_regs[decltype(idx0)::Impl::at(0)];

                ck_tile::sweep_tile_span(dp_spans[ck_tile::number<1>{}], [&](auto idx1) {
                    constexpr auto tile_idx = ck_tile::make_tuple(idx0, idx1);
                    const auto x_idx_inner = ck_tile::get_x_indices_from_distributed_indices(
                        dp_out.get_tile_distribution(), tile_idx);
                    const ck_tile::index_t n_rel = x_idx_inner.at(ck_tile::number<1>{});
                    const ck_tile::index_t n     = start_n + n_rel;
                    const ck_tile::index_t mask_qb = (args.stride_bz / args.stride_bm > args.N_Q_BLOCKS)
                                                     ? (qb * 2 + (m_rel >= 64 ? 1 : 0))
                                                     : qb;
                    const size_t mask_idx = static_cast<size_t>(off_hz) * args.stride_bz +
                                             static_cast<size_t>(mask_qb) * args.stride_bm +
                                             static_cast<size_t>(kv_block) * args.stride_bn;
                    const bool is_active = args.block_mask_ptr[mask_idx] != 0;
                    const bool valid             = row_valid && n < seqlen && is_active;
 
                    const float p =
                        valid ? ck_tile::exp2(qk_out[tile_idx] * scale_log2e - row_lse_log2) : 0.0f;
                    const float ds = p * (dp_out[tile_idx] - delta);
                    ds_t_smem[static_cast<ck_tile::long_index_t>(n_rel) * BlockM + m_rel] =
                        ck_tile::type_convert<QDataType>(ds);
                });
            });

            const bool can_vectorize_transposed_q = !is_boundary && block_can_vectorize_transposed_q;
            if(can_vectorize_transposed_q)
            {
                const int thread_m_rel = threadIdx.x >> 4;
                const int thread_d     = (threadIdx.x & 15) << 3;
                #pragma unroll
                for(int i = 0; i < 8; ++i)
                {
                    int m_rel = thread_m_rel + i * 16;
                    int m = q_start + m_rel;
                    const int4* q_ptr_vec = reinterpret_cast<const int4*>(q_ptr + static_cast<ck_tile::long_index_t>(m) * args.stride_qm + thread_d);
                    int4 val = *q_ptr_vec;
                    const QDataType* val_bf16 = reinterpret_cast<const QDataType*>(&val);
                    #pragma unroll
                    for(int j = 0; j < 8; ++j)
                    {
                        q_t_smem[(thread_d + j) * BlockM + m_rel] = val_bf16[j];
                    }
                }
            }
            else
            {
                for(ck_tile::index_t linear = threadIdx.x; linear < HeadDim * BlockM;
                    linear += blockDim.x)
                {
                    const ck_tile::index_t d     = linear / BlockM;
                    const ck_tile::index_t m_rel = linear - d * BlockM;
                    const ck_tile::index_t m     = q_start + m_rel;
                    q_t_smem[linear] =
                        (m < args.N_Q && m < seqlen && d < args.D)
                            ? q_ptr[static_cast<ck_tile::long_index_t>(m) * args.stride_qm +
                                    static_cast<ck_tile::long_index_t>(d) * args.stride_qd]
                            : ck_tile::type_convert<QDataType>(0.0f);
                }
            }

            ck_tile::block_sync_lds();

            auto ds_t_lds_view = ck_tile::make_tensor_view<ck_tile::address_space_enum::lds>(
                ds_t_smem, MakeSimpleLdsDescriptor<BlockN, BlockM>());
            auto q_t_lds_view = ck_tile::make_tensor_view<ck_tile::address_space_enum::lds>(
                q_t_smem, MakeSimpleLdsDescriptor<HeadDim, BlockM>());

            auto ds_t_lds_window = ck_tile::make_tile_window(
                ds_t_lds_view,
                ck_tile::make_tuple(ck_tile::number<BlockN>{}, ck_tile::number<BlockM>{}),
                {0, 0});
            auto q_t_lds_window = ck_tile::make_tile_window(
                q_t_lds_view,
                ck_tile::make_tuple(ck_tile::number<HeadDim>{}, ck_tile::number<BlockM>{}),
                {0, 0});

            dk_gemm(dk_acc, ds_t_lds_window, q_t_lds_window);

            ck_tile::block_sync_lds();
        }

        StoreMmacOutputTileToLdsRowMajor<BlockN, HeadDim>(dv_gemm, dv_acc, smem.qk);
        ck_tile::block_sync_lds();

        const bool is_boundary_write = (start_n + BlockN > seqlen) || (args.D < HeadDim);
        const bool can_vectorize_write_dv = !is_boundary_write && (args.stride_dvn % 8 == 0) && (args.stride_dvd == 1) && (reinterpret_cast<std::uintptr_t>(dv_ptr) % 16 == 0);
        if(can_vectorize_write_dv)
        {
            const int thread_n_rel = threadIdx.x >> 4;
            const int thread_d     = (threadIdx.x & 15) << 3;
            #pragma unroll
            for(int i = 0; i < BlockN / 16; ++i)
            {
                int n_rel = thread_n_rel + i * 16;
                int n = start_n + n_rel;
                int offset = n_rel * HeadDim + thread_d;
                ck_tile::bf16_t val_bf16[8];
                #pragma unroll
                for(int j = 0; j < 8; ++j)
                {
                    val_bf16[j] = ck_tile::type_convert<ck_tile::bf16_t>(smem.qk[offset + j]);
                }
                int4* dv_ptr_vec = reinterpret_cast<int4*>(dv_ptr + static_cast<ck_tile::long_index_t>(n) * args.stride_dvn + thread_d);
                *dv_ptr_vec = *reinterpret_cast<const int4*>(val_bf16);
            }
        }
        else
        {
            for(ck_tile::index_t linear = threadIdx.x; linear < BlockN * HeadDim; linear += blockDim.x)
            {
                const ck_tile::index_t n_rel = linear / HeadDim;
                const ck_tile::index_t d     = linear - n_rel * HeadDim;
                const ck_tile::index_t n     = start_n + n_rel;

                if(n < seqlen && d < args.D)
                {
                    dv_ptr[static_cast<ck_tile::long_index_t>(n) * args.stride_dvn +
                           static_cast<ck_tile::long_index_t>(d) * args.stride_dvd] =
                        ck_tile::type_convert<VGradDataType>(smem.qk[linear]);
                }
            }
        }

        ck_tile::block_sync_lds();

        StoreMmacOutputTileToLdsRowMajor<BlockN, HeadDim>(dk_gemm, dk_acc, smem.qk);
        ck_tile::block_sync_lds();

        const bool can_vectorize_write_dk = !is_boundary_write && (args.stride_dkn % 8 == 0) && (args.stride_dkd == 1) && (reinterpret_cast<std::uintptr_t>(dk_ptr) % 16 == 0);
        if(can_vectorize_write_dk)
        {
            const int thread_n_rel = threadIdx.x >> 4;
            const int thread_d     = (threadIdx.x & 15) << 3;
            #pragma unroll
            for(int i = 0; i < BlockN / 16; ++i)
            {
                int n_rel = thread_n_rel + i * 16;
                int n = start_n + n_rel;
                int offset = n_rel * HeadDim + thread_d;
                ck_tile::bf16_t val_bf16[8];
                #pragma unroll
                for(int j = 0; j < 8; ++j)
                {
                    val_bf16[j] = ck_tile::type_convert<ck_tile::bf16_t>(smem.qk[offset + j] * args.sm_scale);
                }
                int4* dk_ptr_vec = reinterpret_cast<int4*>(dk_ptr + static_cast<ck_tile::long_index_t>(n) * args.stride_dkn + thread_d);
                *dk_ptr_vec = *reinterpret_cast<const int4*>(val_bf16);
            }
        }
        else
        {
            for(ck_tile::index_t linear = threadIdx.x; linear < BlockN * HeadDim; linear += blockDim.x)
            {
                const ck_tile::index_t n_rel = linear / HeadDim;
                const ck_tile::index_t d     = linear - n_rel * HeadDim;
                const ck_tile::index_t n     = start_n + n_rel;

                if(n < seqlen && d < args.D)
                {
                    dk_ptr[static_cast<ck_tile::long_index_t>(n) * args.stride_dkn +
                           static_cast<ck_tile::long_index_t>(d) * args.stride_dkd] =
                        ck_tile::type_convert<KGradDataType>(smem.qk[linear] * args.sm_scale);
                }
            }
        }
    }
};

} // namespace jenga
} // namespace example
} // namespace ck_tile
