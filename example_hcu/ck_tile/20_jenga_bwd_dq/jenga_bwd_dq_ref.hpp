// SPDX-License-Identifier: MIT
// Copyright (c) 2026, Advanced Micro Devices, Inc. All rights reserved.

#pragma once

#include "jenga_bwd_dq_config.hpp"
#include <algorithm>
#include <cmath>
#include <hip/hip_runtime.h>
#include <iostream>
#include <vector>

namespace ck_tile {
namespace example {
namespace jenga {

template <typename DataType>
void reference_jenga_bwd_dq_naive(const jenga_bwd_dq_args& a)
{
    const DataType* q        = static_cast<const DataType*>(a.q);
    const DataType* k        = static_cast<const DataType*>(a.k);
    const DataType* v        = static_cast<const DataType*>(a.v);
    const DataType* dout     = static_cast<const DataType*>(a.dout);
    DataType* dq             = static_cast<DataType*>(a.dq);
    const index_t head_dim   = a.stride_qm;
    const index_t block_m    = a.block_m;
    const index_t block_n    = a.block_n;
    const index_t num_q_blk  = integer_divide_ceil(a.seqlen_q, block_m);
    const float qk_scale     = a.sm_scale * 1.4426950408889634f;

    for(index_t off_hz = 0; off_hz < a.batch * a.nhead; ++off_hz)
    {
        const index_t seqlen = a.seqlens[off_hz / a.nhead];
        for(index_t m = 0; m < seqlen; ++m)
        {
            const index_t q_blk     = m / block_m;
            const int active_blocks = a.lut_size[off_hz * num_q_blk + q_blk];

            for(index_t d = 0; d < head_dim; ++d)
            {
                float dq_acc = 0.0f;

                for(index_t i = 0; i < active_blocks; ++i)
                {
                    const int block_idx = a.lut[off_hz * a.stride_lutz + q_blk * a.stride_lutm +
                                                i * a.stride_lutk];
                    const bool is_text  = block_idx >= a.text_block_start;
                    const index_t n_beg = static_cast<index_t>(block_idx) * block_n;
                    const index_t n_end = std::min(n_beg + block_n, seqlen);

                    for(index_t n = n_beg; n < n_end; ++n)
                    {
                        float qk = 0.0f;
                        float dp = 0.0f;

                        for(index_t kd = 0; kd < head_dim; ++kd)
                        {
                            const float q_val = type_convert<float>(
                                q[off_hz * a.stride_qz + m * a.stride_qm + kd * a.stride_qk]);
                            const float k_val = type_convert<float>(
                                k[off_hz * a.stride_kz + n * a.stride_kn + kd * a.stride_kk]);
                            const float do_val = type_convert<float>(
                                dout[off_hz * a.stride_doz + m * a.stride_dom +
                                     kd * a.stride_dok]);
                            const float v_val = type_convert<float>(
                                v[off_hz * a.stride_vz + n * a.stride_vn + kd * a.stride_vk]);

                            qk += q_val * k_val * qk_scale;
                            dp += do_val * v_val;
                        }

                        if(is_text)
                        {
                            qk += a.text_amp;
                        }

                        const float p =
                            std::exp2(qk - a.lse[off_hz * a.stride_lz + m * a.stride_lm]);
                        const float ds = p * (dp - a.deltas[off_hz * a.stride_dz +
                                                            m * a.stride_dm]);
                        const float k_d = type_convert<float>(
                            k[off_hz * a.stride_kz + n * a.stride_kn + d * a.stride_kk]);

                        dq_acc += ds * k_d * a.sm_scale;
                    }
                }

                dq[off_hz * a.stride_dqz + m * a.stride_dqm + d * a.stride_dqk] =
                    type_convert<DataType>(dq_acc);
            }
        }
    }
}

template <typename DataType>
void reference_jenga_bwd_dq_blockwise(const jenga_bwd_dq_args& a)
{
    const DataType* q        = static_cast<const DataType*>(a.q);
    const DataType* k        = static_cast<const DataType*>(a.k);
    const DataType* v        = static_cast<const DataType*>(a.v);
    const DataType* dout     = static_cast<const DataType*>(a.dout);
    DataType* dq             = static_cast<DataType*>(a.dq);
    const index_t head_dim   = a.stride_qm;
    const index_t block_m    = a.block_m;
    const index_t block_n    = a.block_n;
    const index_t num_q_blk  = integer_divide_ceil(a.seqlen_q, block_m);
    const float qk_scale     = a.sm_scale * 1.4426950408889634f;

    std::vector<index_t> n_indices;
    std::vector<float> ds_values;
    n_indices.reserve(static_cast<std::size_t>(a.max_nnz * block_n));
    ds_values.reserve(static_cast<std::size_t>(a.max_nnz * block_n));

    for(index_t off_hz = 0; off_hz < a.batch * a.nhead; ++off_hz)
    {
        const index_t seqlen = a.seqlens[off_hz / a.nhead];
        for(index_t m = 0; m < seqlen; ++m)
        {
            const index_t q_blk     = m / block_m;
            const int active_blocks = a.lut_size[off_hz * num_q_blk + q_blk];

            n_indices.clear();
            ds_values.clear();

            for(index_t i = 0; i < active_blocks; ++i)
            {
                const int block_idx = a.lut[off_hz * a.stride_lutz + q_blk * a.stride_lutm +
                                            i * a.stride_lutk];
                const bool is_text  = block_idx >= a.text_block_start;
                const index_t n_beg = static_cast<index_t>(block_idx) * block_n;
                const index_t n_end = std::min(n_beg + block_n, seqlen);

                for(index_t n = n_beg; n < n_end; ++n)
                {
                    float qk = 0.0f;
                    float dp = 0.0f;

                    for(index_t kd = 0; kd < head_dim; ++kd)
                    {
                        const float q_val = type_convert<float>(
                            q[off_hz * a.stride_qz + m * a.stride_qm + kd * a.stride_qk]);
                        const float k_val = type_convert<float>(
                            k[off_hz * a.stride_kz + n * a.stride_kn + kd * a.stride_kk]);
                        const float do_val = type_convert<float>(
                            dout[off_hz * a.stride_doz + m * a.stride_dom + kd * a.stride_dok]);
                        const float v_val = type_convert<float>(
                            v[off_hz * a.stride_vz + n * a.stride_vn + kd * a.stride_vk]);

                        qk += q_val * k_val * qk_scale;
                        dp += do_val * v_val;
                    }

                    if(is_text)
                    {
                        qk += a.text_amp;
                    }

                    const float p =
                        std::exp2(qk - a.lse[off_hz * a.stride_lz + m * a.stride_lm]);
                    const float ds =
                        p * (dp - a.deltas[off_hz * a.stride_dz + m * a.stride_dm]);

                    n_indices.push_back(n);
                    ds_values.push_back(ds);
                }
            }

            for(index_t d = 0; d < head_dim; ++d)
            {
                float dq_acc = 0.0f;

                for(std::size_t t = 0; t < n_indices.size(); ++t)
                {
                    const index_t n = n_indices[t];
                    const float k_d = type_convert<float>(
                        k[off_hz * a.stride_kz + n * a.stride_kn + d * a.stride_kk]);
                    dq_acc += ds_values[t] * k_d * a.sm_scale;
                }

                dq[off_hz * a.stride_dqz + m * a.stride_dqm + d * a.stride_dqk] =
                    type_convert<DataType>(dq_acc);
            }
        }
    }
}

template <typename DataType>
void reference_jenga_bwd_dq(const jenga_bwd_dq_args& a)
{
    reference_jenga_bwd_dq_blockwise<DataType>(a);
}

template <typename DataType>
__global__ void reference_build_lse_kernel(jenga_bwd_dq_args a)
{
    const DataType* q       = static_cast<const DataType*>(a.q);
    const DataType* k       = static_cast<const DataType*>(a.k);
    float* lse              = const_cast<float*>(a.lse);
    const index_t block_m   = a.block_m;
    const index_t block_n   = a.block_n;
    const index_t num_q_blk = (a.seqlen_q + block_m - 1) / block_m;
    const float qk_scale    = a.sm_scale * 1.4426950408889634f;
    const index_t row       = static_cast<index_t>(blockIdx.x) * blockDim.x + threadIdx.x;
    const index_t total     = a.batch * a.nhead * a.seqlen_q;

    if(row >= total)
    {
        return;
    }

    const index_t off_hz = row / a.seqlen_q;
    const index_t m      = row - off_hz * a.seqlen_q;
    const index_t seqlen = a.seqlens[off_hz / a.nhead];

    if(m >= seqlen)
    {
        return;
    }

    const index_t q_blk    = m / block_m;
    const int active       = a.lut_size[off_hz * num_q_blk + q_blk];
    float row_max          = -INFINITY;
    float row_sum_exp      = 0.0f;

    for(int pass = 0; pass < 2; ++pass)
    {
        for(index_t i = 0; i < active; ++i)
        {
            const int kb = a.lut[off_hz * a.stride_lutz + q_blk * a.stride_lutm +
                                 i * a.stride_lutk];
            const index_t n_beg = static_cast<index_t>(kb) * block_n;
            const index_t n_end = min(n_beg + block_n, seqlen);

            for(index_t n = n_beg; n < n_end; ++n)
            {
                float qk = 0.0f;
                for(index_t d = 0; d < a.stride_qm; ++d)
                {
                    const float q_val = type_convert<float>(
                        q[off_hz * a.stride_qz + m * a.stride_qm + d * a.stride_qk]);
                    const float k_val = type_convert<float>(
                        k[off_hz * a.stride_kz + n * a.stride_kn + d * a.stride_kk]);
                    qk += q_val * k_val;
                }

                qk = qk * qk_scale + (kb >= a.text_block_start ? a.text_amp : 0.0f);
                if(pass == 0)
                {
                    row_max = fmaxf(row_max, qk);
                }
                else
                {
                    row_sum_exp += exp2f(qk - row_max);
                }
            }
        }
    }

    lse[off_hz * a.stride_lz + m * a.stride_lm] = row_max + log2f(row_sum_exp);
}

template <typename DataType>
__global__ void reference_jenga_bwd_dq_kernel(jenga_bwd_dq_args a)
{
    const DataType* q       = static_cast<const DataType*>(a.q);
    const DataType* k       = static_cast<const DataType*>(a.k);
    const DataType* v       = static_cast<const DataType*>(a.v);
    const DataType* dout    = static_cast<const DataType*>(a.dout);
    DataType* dq            = static_cast<DataType*>(a.dq);
    const index_t block_m   = a.block_m;
    const index_t block_n   = a.block_n;
    const index_t head_dim  = a.stride_qm;
    const index_t num_q_blk = (a.seqlen_q + block_m - 1) / block_m;
    const float qk_scale    = a.sm_scale * 1.4426950408889634f;
    const index_t idx       = static_cast<index_t>(blockIdx.x) * blockDim.x + threadIdx.x;
    const index_t total     = a.batch * a.nhead * a.seqlen_q * head_dim;

    if(idx >= total)
    {
        return;
    }

    const index_t d      = idx % head_dim;
    const index_t row    = idx / head_dim;
    const index_t off_hz = row / a.seqlen_q;
    const index_t m      = row - off_hz * a.seqlen_q;
    const index_t seqlen = a.seqlens[off_hz / a.nhead];

    if(m >= seqlen)
    {
        return;
    }

    const index_t q_blk     = m / block_m;
    const int active_blocks = a.lut_size[off_hz * num_q_blk + q_blk];
    float dq_acc            = 0.0f;

    for(index_t i = 0; i < active_blocks; ++i)
    {
        const int block_idx = a.lut[off_hz * a.stride_lutz + q_blk * a.stride_lutm +
                                    i * a.stride_lutk];
        const bool is_text  = block_idx >= a.text_block_start;
        const index_t n_beg = static_cast<index_t>(block_idx) * block_n;
        const index_t n_end = min(n_beg + block_n, seqlen);

        for(index_t n = n_beg; n < n_end; ++n)
        {
            float qk = 0.0f;
            float dp = 0.0f;

            for(index_t kd = 0; kd < head_dim; ++kd)
            {
                const float q_val = type_convert<float>(
                    q[off_hz * a.stride_qz + m * a.stride_qm + kd * a.stride_qk]);
                const float k_val = type_convert<float>(
                    k[off_hz * a.stride_kz + n * a.stride_kn + kd * a.stride_kk]);
                const float do_val = type_convert<float>(
                    dout[off_hz * a.stride_doz + m * a.stride_dom + kd * a.stride_dok]);
                const float v_val = type_convert<float>(
                    v[off_hz * a.stride_vz + n * a.stride_vn + kd * a.stride_vk]);

                qk += q_val * k_val * qk_scale;
                dp += do_val * v_val;
            }

            if(is_text)
            {
                qk += a.text_amp;
            }

            const float p = exp2f(qk - a.lse[off_hz * a.stride_lz + m * a.stride_lm]);
            const float ds =
                p * (dp - a.deltas[off_hz * a.stride_dz + m * a.stride_dm]);
            const float k_d =
                type_convert<float>(k[off_hz * a.stride_kz + n * a.stride_kn + d * a.stride_kk]);

            dq_acc += ds * k_d * a.sm_scale;
        }
    }

    dq[off_hz * a.stride_dqz + m * a.stride_dqm + d * a.stride_dqk] =
        type_convert<DataType>(dq_acc);
}

template <typename DataType>
bool reference_jenga_bwd_dq_gpu(jenga_bwd_dq_args a, hipStream_t stream = nullptr)
{
    constexpr int threads = 256;
    const index_t rows    = a.batch * a.nhead * a.seqlen_q;
    const index_t elems   = rows * a.stride_qm;

    hipLaunchKernelGGL(HIP_KERNEL_NAME(reference_build_lse_kernel<DataType>),
                       dim3((rows + threads - 1) / threads),
                       dim3(threads),
                       0,
                       stream,
                       a);
    hipError_t err = hipGetLastError();
    if(err != hipSuccess)
    {
        std::cerr << "reference_build_lse_kernel launch failed: " << hipGetErrorString(err)
                  << std::endl;
        return false;
    }

    hipLaunchKernelGGL(HIP_KERNEL_NAME(reference_jenga_bwd_dq_kernel<DataType>),
                       dim3((elems + threads - 1) / threads),
                       dim3(threads),
                       0,
                       stream,
                       a);
    err = hipGetLastError();
    if(err != hipSuccess)
    {
        std::cerr << "reference_jenga_bwd_dq_kernel launch failed: " << hipGetErrorString(err)
                  << std::endl;
        return false;
    }

    err = hipStreamSynchronize(stream);
    if(err != hipSuccess)
    {
        std::cerr << "GPU reference synchronize failed: " << hipGetErrorString(err) << std::endl;
        return false;
    }

    return true;
}

} // namespace jenga
} // namespace example
} // namespace ck_tile
