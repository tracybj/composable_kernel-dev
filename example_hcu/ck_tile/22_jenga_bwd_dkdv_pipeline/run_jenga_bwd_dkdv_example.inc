// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2025, , Inc. All rights reserved.
#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using ck_tile::example::jenga::jenga_bwd_dkdv_args;

namespace {

template <typename T>
float as_float(T x)
{
    return ck_tile::type_convert<float>(x);
}

std::size_t q_offset(const jenga_bwd_dkdv_args& a,
                     ck_tile::index_t b,
                     ck_tile::index_t h,
                     ck_tile::index_t m,
                     ck_tile::index_t d)
{
    return static_cast<std::size_t>(b) * a.stride_qz + static_cast<std::size_t>(h) * a.stride_qh +
           static_cast<std::size_t>(m) * a.stride_qm + static_cast<std::size_t>(d) * a.stride_qd;
}

std::size_t kv_offset(ck_tile::index_t stride_z,
                      ck_tile::index_t stride_h,
                      ck_tile::index_t stride_n,
                      ck_tile::index_t stride_d,
                      ck_tile::index_t b,
                      ck_tile::index_t h,
                      ck_tile::index_t n,
                      ck_tile::index_t d)
{
    return static_cast<std::size_t>(b) * stride_z + static_cast<std::size_t>(h) * stride_h +
           static_cast<std::size_t>(n) * stride_n + static_cast<std::size_t>(d) * stride_d;
}

std::size_t lse_offset(const jenga_bwd_dkdv_args& a,
                       ck_tile::index_t off_hz,
                       ck_tile::index_t m)
{
    return static_cast<std::size_t>(off_hz) * a.stride_lz +
           static_cast<std::size_t>(m) * a.stride_lm;
}

std::size_t mask_offset(const jenga_bwd_dkdv_args& a,
                        ck_tile::index_t off_hz,
                        ck_tile::index_t qb,
                        ck_tile::index_t kb)
{
    return static_cast<std::size_t>(off_hz) * a.stride_bz +
           static_cast<std::size_t>(qb) * a.stride_bm + static_cast<std::size_t>(kb) * a.stride_bn;
}

std::size_t delta_offset(const jenga_bwd_dkdv_args& a,
                         ck_tile::index_t off_hz,
                         ck_tile::index_t m)
{
    return static_cast<std::size_t>(off_hz) * a.stride_delta_z +
           static_cast<std::size_t>(m) * a.stride_delta_m;
}

std::size_t rlut_offset(const jenga_bwd_dkdv_args& a,
                        ck_tile::index_t off_hz,
                        ck_tile::index_t kb,
                        ck_tile::index_t i)
{
    return static_cast<std::size_t>(off_hz) * a.stride_rlutz +
           static_cast<std::size_t>(kb) * a.stride_rlutn +
           static_cast<std::size_t>(i) * a.stride_rlutk;
}

std::size_t rlut_size_offset(const jenga_bwd_dkdv_args& a,
                             ck_tile::index_t off_hz,
                             ck_tile::index_t kb)
{
    return static_cast<std::size_t>(off_hz) * a.stride_rlut_size_z +
           static_cast<std::size_t>(kb) * a.stride_rlut_size_n;
}

void compute_delta(const std::vector<ODataType>& o,
                   const std::vector<OGradDataType>& dout,
                   const std::vector<int32_t>& seqlens,
                   const jenga_bwd_dkdv_args& a,
                   std::vector<AccDataType>& delta)
{
    std::fill(delta.begin(), delta.end(), 0.0f);

    for(ck_tile::index_t b = 0; b < a.B; ++b)
    {
        const ck_tile::index_t seqlen = seqlens[b];
        for(ck_tile::index_t h = 0; h < a.H; ++h)
        {
            const ck_tile::index_t off_hz = b * a.H + h;
            for(ck_tile::index_t m = 0; m < std::min(a.N_Q, seqlen); ++m)
            {
                float acc = 0.0f;
                for(ck_tile::index_t d = 0; d < a.D; ++d)
                {
                    acc += as_float(o[kv_offset(a.stride_oz,
                                                a.stride_oh,
                                                a.stride_om,
                                                a.stride_od,
                                                b,
                                                h,
                                                m,
                                                d)]) *
                           as_float(dout[kv_offset(a.stride_doz,
                                                   a.stride_doh,
                                                   a.stride_dom,
                                                   a.stride_dod,
                                                   b,
                                                   h,
                                                   m,
                                                   d)]);
                }
                delta[delta_offset(a, off_hz, m)] = acc;
            }
        }
    }
}

ck_tile::index_t build_reverse_lut(const std::vector<int32_t>& block_mask,
                                   const jenga_bwd_dkdv_args& a,
                                   std::vector<int32_t>& rlut,
                                   std::vector<int32_t>& rlut_size)
{
    ck_tile::index_t max_nnz_r = 0;
    const ck_tile::index_t batch_heads = a.B * a.H;

    for(ck_tile::index_t off_hz = 0; off_hz < batch_heads; ++off_hz)
    {
        for(ck_tile::index_t kb = 0; kb < a.N_KV_BLOCKS; ++kb)
        {
            ck_tile::index_t count = 0;
            for(ck_tile::index_t qb = 0; qb < a.N_Q_BLOCKS; ++qb)
            {
                count += block_mask[mask_offset(a, off_hz, qb, kb)] != 0 ? 1 : 0;
            }
            max_nnz_r = std::max(max_nnz_r, count);
        }
    }

    max_nnz_r = std::max<ck_tile::index_t>(max_nnz_r, 1);
    rlut.assign(static_cast<std::size_t>(batch_heads) * a.N_KV_BLOCKS * max_nnz_r, 0);
    rlut_size.assign(static_cast<std::size_t>(batch_heads) * a.N_KV_BLOCKS, 0);

    for(ck_tile::index_t off_hz = 0; off_hz < batch_heads; ++off_hz)
    {
        for(ck_tile::index_t kb = 0; kb < a.N_KV_BLOCKS; ++kb)
        {
            ck_tile::index_t count = 0;
            for(ck_tile::index_t qb = 0; qb < a.N_Q_BLOCKS; ++qb)
            {
                if(block_mask[mask_offset(a, off_hz, qb, kb)] != 0)
                {
                    rlut[(static_cast<std::size_t>(off_hz) * a.N_KV_BLOCKS + kb) * max_nnz_r +
                         count] = static_cast<int32_t>(qb);
                    ++count;
                }
            }
            rlut_size[static_cast<std::size_t>(off_hz) * a.N_KV_BLOCKS + kb] =
                static_cast<int32_t>(count);
        }
    }

    return max_nnz_r;
}

void compute_lse(const std::vector<QDataType>& q,
                 const std::vector<KDataType>& k,
                 const std::vector<int32_t>& block_mask,
                 const std::vector<int32_t>& seqlens,
                 const jenga_bwd_dkdv_args& a,
                 std::vector<LSEDataType>& lse)
{
    const float qk_scale = a.sm_scale * ck_tile::log2e_v<float>;

    for(ck_tile::index_t b = 0; b < a.B; ++b)
    {
        const ck_tile::index_t seqlen = seqlens[b];
        for(ck_tile::index_t h = 0; h < a.H; ++h)
        {
            const ck_tile::index_t off_hz = b * a.H + h;
            for(ck_tile::index_t m = 0; m < a.N_Q; ++m)
            {
                float row_max = -std::numeric_limits<float>::infinity();
                const ck_tile::index_t qb = m / a.M0;

                if(m < seqlen)
                {
                    for(ck_tile::index_t kb = 0; kb < a.N_KV_BLOCKS; ++kb)
                    {
                        if(block_mask[mask_offset(a, off_hz, qb, kb)] == 0)
                        {
                            continue;
                        }

                        const ck_tile::index_t n_begin = kb * a.N0;
                        const ck_tile::index_t n_end   = std::min(n_begin + a.N0, seqlen);
                        const bool is_text_block       = kb >= a.text_block_start;

                        for(ck_tile::index_t n = n_begin; n < n_end; ++n)
                        {
                            float qk_val = 0.0f;
                            for(ck_tile::index_t d = 0; d < a.D; ++d)
                            {
                                qk_val += as_float(q[q_offset(a, b, h, m, d)]) *
                                          as_float(k[kv_offset(a.stride_kz,
                                                               a.stride_kh,
                                                               a.stride_kn,
                                                               a.stride_kd,
                                                               b,
                                                               h,
                                                               n,
                                                               d)]);
                            }
                            qk_val = qk_val * qk_scale + (is_text_block ? a.text_amp : 0.0f);
                            row_max = std::max(row_max, qk_val);
                        }
                    }
                }

                float row_sum = 0.0f;
                if(std::isfinite(row_max))
                {
                    for(ck_tile::index_t kb = 0; kb < a.N_KV_BLOCKS; ++kb)
                    {
                        if(block_mask[mask_offset(a, off_hz, qb, kb)] == 0)
                        {
                            continue;
                        }

                        const ck_tile::index_t n_begin = kb * a.N0;
                        const ck_tile::index_t n_end   = std::min(n_begin + a.N0, seqlen);
                        const bool is_text_block       = kb >= a.text_block_start;

                        for(ck_tile::index_t n = n_begin; n < n_end; ++n)
                        {
                            float qk_val = 0.0f;
                            for(ck_tile::index_t d = 0; d < a.D; ++d)
                            {
                                qk_val += as_float(q[q_offset(a, b, h, m, d)]) *
                                          as_float(k[kv_offset(a.stride_kz,
                                                               a.stride_kh,
                                                               a.stride_kn,
                                                               a.stride_kd,
                                                               b,
                                                               h,
                                                               n,
                                                               d)]);
                            }
                            qk_val = qk_val * qk_scale + (is_text_block ? a.text_amp : 0.0f);
                            row_sum += std::exp2(qk_val - row_max);
                        }
                    }
                }

                lse[lse_offset(a, off_hz, m)] =
                    std::isfinite(row_max) ? row_max + std::log2(row_sum)
                                           : -std::numeric_limits<float>::infinity();
            }
        }
    }
}

void reference_jenga_bwd_dkdv(const std::vector<QDataType>& q,
                              const std::vector<KDataType>& k,
                              const std::vector<VDataType>& v,
                              const std::vector<OGradDataType>& dout,
                              const std::vector<AccDataType>& delta,
                              const std::vector<LSEDataType>& lse,
                              const std::vector<int32_t>& rlut,
                              const std::vector<int32_t>& rlut_size,
                              const std::vector<int32_t>& seqlens,
                              const jenga_bwd_dkdv_args& a,
                              std::vector<KGradDataType>& dk,
                              std::vector<VGradDataType>& dv)
{
    const float qk_scale = a.sm_scale * ck_tile::log2e_v<float>;
    std::fill(dk.begin(), dk.end(), ck_tile::type_convert<KGradDataType>(0.0f));
    std::fill(dv.begin(), dv.end(), ck_tile::type_convert<VGradDataType>(0.0f));

    for(ck_tile::index_t b = 0; b < a.B; ++b)
    {
        const ck_tile::index_t seqlen = seqlens[b];
        for(ck_tile::index_t h = 0; h < a.H; ++h)
        {
            const ck_tile::index_t off_hz = b * a.H + h;
            for(ck_tile::index_t kb = 0; kb < a.N_KV_BLOCKS; ++kb)
            {
                const ck_tile::index_t n_begin = kb * a.N0;
                const ck_tile::index_t n_end   = std::min(n_begin + a.N0, seqlen);
                const bool is_text_block       = kb >= a.text_block_start;

                for(ck_tile::index_t n = n_begin; n < n_end; ++n)
                {
                    for(ck_tile::index_t d = 0; d < a.D; ++d)
                    {
                        float dk_acc = 0.0f;
                        float dv_acc = 0.0f;

                        const ck_tile::index_t num_active =
                            rlut_size[rlut_size_offset(a, off_hz, kb)];

                        for(ck_tile::index_t i = 0; i < a.max_nnz_r; ++i)
                        {
                            if(i >= num_active)
                            {
                                continue;
                            }

                            const ck_tile::index_t qb = rlut[rlut_offset(a, off_hz, kb, i)];
                            const ck_tile::index_t q_begin = qb * a.M0;
                            const ck_tile::index_t q_end =
                                std::min(std::min(q_begin + a.M0, a.N_Q), seqlen);

                            for(ck_tile::index_t m = q_begin; m < q_end; ++m)
                            {
                                float qk_val = 0.0f;
                                float dp = 0.0f;

                                for(ck_tile::index_t dd = 0; dd < a.D; ++dd)
                                {
                                    const float do_val =
                                        as_float(dout[kv_offset(a.stride_doz,
                                                                a.stride_doh,
                                                                a.stride_dom,
                                                                a.stride_dod,
                                                                b,
                                                                h,
                                                                m,
                                                                dd)]);
                                    dp += do_val *
                                          as_float(v[kv_offset(a.stride_vz,
                                                               a.stride_vh,
                                                               a.stride_vn,
                                                               a.stride_vd,
                                                               b,
                                                               h,
                                                               n,
                                                               dd)]);
                                    qk_val += as_float(q[q_offset(a, b, h, m, dd)]) *
                                              as_float(k[kv_offset(a.stride_kz,
                                                                   a.stride_kh,
                                                                   a.stride_kn,
                                                                   a.stride_kd,
                                                                   b,
                                                                   h,
                                                                   n,
                                                                   dd)]);
                                }

                                qk_val = qk_val * qk_scale +
                                         (is_text_block ? a.text_amp : 0.0f);
                                const float p =
                                    std::exp2(qk_val - lse[lse_offset(a, off_hz, m)]);
                                const float do_d =
                                    as_float(dout[kv_offset(a.stride_doz,
                                                            a.stride_doh,
                                                            a.stride_dom,
                                                            a.stride_dod,
                                                            b,
                                                            h,
                                                            m,
                                                            d)]);
                                const float q_d = as_float(q[q_offset(a, b, h, m, d)]);
                                const float delta_val = delta[delta_offset(a, off_hz, m)];

                                dv_acc += p * do_d;
                                dk_acc += p * (dp - delta_val) * q_d * a.sm_scale;
                             }
                        }

                        dk[kv_offset(a.stride_dkz, a.stride_dkh, a.stride_dkn, a.stride_dkd, b, h, n, d)] =
                            ck_tile::type_convert<KGradDataType>(dk_acc);
                        dv[kv_offset(a.stride_dvz, a.stride_dvh, a.stride_dvn, a.stride_dvd, b, h, n, d)] =
                            ck_tile::type_convert<VGradDataType>(dv_acc);
                    }
                }
            }
        }
    }
}

template <typename T>
double cosine_similarity(const std::vector<T>& x, const std::vector<T>& y)
{
    double dot = 0.0;
    double nx  = 0.0;
    double ny  = 0.0;
    for(std::size_t i = 0; i < x.size(); ++i)
    {
        const double xv = as_float(x[i]);
        const double yv = as_float(y[i]);
        dot += xv * yv;
        nx += xv * xv;
        ny += yv * yv;
    }
    return (nx == 0.0 || ny == 0.0) ? 0.0 : dot / std::sqrt(nx * ny);
}

template <typename T>
std::pair<double, double> diff_stats(const std::vector<T>& x, const std::vector<T>& y)
{
    double max_diff  = 0.0;
    double mean_diff = 0.0;
    for(std::size_t i = 0; i < x.size(); ++i)
    {
        const double diff = std::abs(static_cast<double>(as_float(x[i])) -
                                     static_cast<double>(as_float(y[i])));
        max_diff = std::max(max_diff, diff);
        mean_diff += diff;
    }
    mean_diff /= static_cast<double>(x.size());
    return {max_diff, mean_diff};
}

template <typename T>
std::pair<double, std::size_t> norm_and_nnz(const std::vector<T>& x)
{
    double norm = 0.0;
    std::size_t nnz = 0;
    for(const auto& v : x)
    {
        const double fv = as_float(v);
        norm += fv * fv;
        nnz += std::abs(fv) > 0.0 ? 1 : 0;
    }
    return {std::sqrt(norm), nnz};
}

} // namespace

int run_jenga_bwd_dkdv_example(int argc, char* argv[])
{
    auto [parse_ok, arg_parser] = create_args(argc, argv);
    if(!parse_ok)
    {
        return 0;
    }

    const ck_tile::index_t B    = arg_parser.get_int("b");
    const ck_tile::index_t H    = arg_parser.get_int("h");
    const ck_tile::index_t N_Q  = arg_parser.get_int("n_q");
    const ck_tile::index_t N_KV = arg_parser.get_int("n_kv");
    const ck_tile::index_t D    = arg_parser.get_int("d");
    const ck_tile::index_t M0   = arg_parser.get_int("m0");
    const ck_tile::index_t N0   = arg_parser.get_int("n0");
    const ck_tile::index_t text_blocks = arg_parser.get_int("text_blocks");
    const float text_amp        = arg_parser.get_float("text_amp");
    const ck_tile::index_t mask_radius = arg_parser.get_int("mask_radius");
    const std::string mask_type = arg_parser.get_str("mask_type");
    const int k_active          = arg_parser.get_int("k_active");
    const int seed              = arg_parser.get_int("seed");
    const int do_validation     = arg_parser.get_int("v");
    const int warmup            = arg_parser.get_int("warmup");
    const int repeat            = arg_parser.get_int("repeat");
    const bool gpu_timer        = arg_parser.get_str("timer") != "cpu";

    if(D != 128 || M0 != 128 || N0 != 64)
    {
        std::cerr << "This first clean example supports d=128, m0=128, n0=64." << std::endl;
        return 0;
    }

    const ck_tile::index_t N_Q_BLOCKS  = ck_tile::integer_divide_ceil(N_Q, M0);
    const ck_tile::index_t N_KV_BLOCKS = ck_tile::integer_divide_ceil(N_KV, N0);
    const ck_tile::index_t text_block_start =
        std::max<ck_tile::index_t>(0, N_KV_BLOCKS - text_blocks);
    const float sm_scale = 1.0f / std::sqrt(static_cast<float>(D));

    const std::size_t q_numel    = static_cast<std::size_t>(B) * H * N_Q * D;
    const std::size_t kv_numel   = static_cast<std::size_t>(B) * H * N_KV * D;
    const std::size_t lse_numel  = static_cast<std::size_t>(B) * H * N_Q;
    const std::size_t mask_numel = static_cast<std::size_t>(B) * H * N_Q_BLOCKS * N_KV_BLOCKS;

    std::vector<QDataType> q(q_numel);
    std::vector<KDataType> k(kv_numel);
    std::vector<VDataType> v(kv_numel);
    std::vector<ODataType> o(q_numel);
    std::vector<OGradDataType> dout(q_numel);
    std::vector<AccDataType> delta(lse_numel);
    std::vector<LSEDataType> lse(lse_numel);
    std::vector<LSEDataType> lse_for_pipeline(lse_numel);
    std::vector<LSEDataType> lse_text_for_pipeline(lse_numel);
    std::vector<int32_t> block_mask(mask_numel, 0);
    std::vector<int32_t> rlut;
    std::vector<int32_t> rlut_size;
    std::vector<int32_t> seqlens(B);
    std::vector<KGradDataType> dk_device(kv_numel);
    std::vector<VGradDataType> dv_device(kv_numel);
    std::vector<KGradDataType> dk_ref(kv_numel);
    std::vector<VGradDataType> dv_ref(kv_numel);

    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist(-0.25f, 0.25f);
    auto fill = [&](auto& xs) {
        if(do_validation != 0)
        {
            for(auto& x : xs)
            {
                x = ck_tile::type_convert<std::remove_reference_t<decltype(x)>>(dist(rng));
            }
        }
        else
        {
            using T = typename std::decay_t<decltype(xs)>::value_type;
            std::fill(xs.begin(), xs.end(), ck_tile::type_convert<T>(0.1f));
        }
    };

    fill(q);
    fill(k);
    fill(v);
    fill(o);
    fill(dout);

    for(ck_tile::index_t b = 0; b < B; ++b)
    {
        seqlens[b] = static_cast<int32_t>(std::max<ck_tile::index_t>(
            1, N_KV - (b % 3) * std::min<ck_tile::index_t>(N0 / 2, N_KV - 1)));
    }

    jenga_bwd_dkdv_args args{};
    args.B = B;
    args.H = H;
    args.N_Q = N_Q;
    args.N_KV = N_KV;
    args.D = D;
    args.N_Q_BLOCKS = N_Q_BLOCKS;
    args.N_KV_BLOCKS = N_KV_BLOCKS;
    args.M0 = M0;
    args.N0 = N0;
    args.max_nnz_r = 1;
    args.text_block_start = text_block_start;
    args.sm_scale = sm_scale;
    args.text_amp = text_amp;

    args.stride_qz = H * N_Q * D;
    args.stride_qh = N_Q * D;
    args.stride_qm = D;
    args.stride_qd = 1;
    args.stride_kz = H * N_KV * D;
    args.stride_kh = N_KV * D;
    args.stride_kn = D;
    args.stride_kd = 1;
    args.stride_vz = H * N_KV * D;
    args.stride_vh = N_KV * D;
    args.stride_vn = D;
    args.stride_vd = 1;
    args.stride_oz = H * N_Q * D;
    args.stride_oh = N_Q * D;
    args.stride_om = D;
    args.stride_od = 1;
    args.stride_doz = H * N_Q * D;
    args.stride_doh = N_Q * D;
    args.stride_dom = D;
    args.stride_dod = 1;
    args.stride_delta_z = N_Q;
    args.stride_delta_m = 1;
    args.stride_dkz = H * N_KV * D;
    args.stride_dkh = N_KV * D;
    args.stride_dkn = D;
    args.stride_dkd = 1;
    args.stride_dvz = H * N_KV * D;
    args.stride_dvh = N_KV * D;
    args.stride_dvn = D;
    args.stride_dvd = 1;
    args.stride_lz = N_Q;
    args.stride_lm = 1;
    args.stride_bz = N_Q_BLOCKS * N_KV_BLOCKS;
    args.stride_bm = N_KV_BLOCKS;
    args.stride_bn = 1;
    args.stride_rlut_size_z = N_KV_BLOCKS;
    args.stride_rlut_size_n = 1;

    if(mask_type == "random")
    {
        int actual_k_active = k_active;
        if(actual_k_active <= 0)
        {
            if(N_Q == 18048 && N_KV == 18048)
            {
                actual_k_active = 143;
            }
            else if(N_Q == 75648 && N_KV == 75648)
            {
                actual_k_active = 592;
            }
            else
            {
                actual_k_active = N_KV_BLOCKS / 2;
            }
        }
        actual_k_active = std::min<int>(actual_k_active, N_KV_BLOCKS);

        std::vector<ck_tile::index_t> kv_indices(N_KV_BLOCKS);
        std::iota(kv_indices.begin(), kv_indices.end(), 0);

        std::mt19937 rng(seed);

        for(ck_tile::index_t b = 0; b < B; ++b)
        {
            for(ck_tile::index_t h = 0; h < H; ++h)
            {
                const ck_tile::index_t off_hz = b * H + h;
                for(ck_tile::index_t qb = 0; qb < N_Q_BLOCKS; ++qb)
                {
                    std::shuffle(kv_indices.begin(), kv_indices.end(), rng);
                    for(int i = 0; i < actual_k_active; ++i)
                    {
                        block_mask[mask_offset(args, off_hz, qb, kv_indices[i])] = 1;
                    }
                }
            }
        }
    }
    else
    {
        for(ck_tile::index_t b = 0; b < B; ++b)
        {
            for(ck_tile::index_t h = 0; h < H; ++h)
            {
                const ck_tile::index_t off_hz = b * H + h;
                for(ck_tile::index_t qb = 0; qb < N_Q_BLOCKS; ++qb)
                {
                    for(ck_tile::index_t kb = 0; kb < N_KV_BLOCKS; ++kb)
                    {
                        const bool in_local_window =
                            std::abs(static_cast<int>(qb) - static_cast<int>(kb)) <= mask_radius;
                        const bool is_text_block   = kb >= text_block_start;
                        block_mask[mask_offset(args, off_hz, qb, kb)] =
                            (in_local_window || is_text_block) ? 1 : 0;
                    }
                }
            }
        }
    }

    args.max_nnz_r = build_reverse_lut(block_mask, args, rlut, rlut_size);
    args.stride_rlutz = N_KV_BLOCKS * args.max_nnz_r;
    args.stride_rlutn = args.max_nnz_r;
    args.stride_rlutk = 1;

    if(do_validation != 0)
    {
        compute_delta(o, dout, seqlens, args, delta);
        compute_lse(q, k, block_mask, seqlens, args, lse);
    }
    else
    {
        fill(delta);
        fill(lse);
    }
    for(std::size_t i = 0; i < lse.size(); ++i)
    {
        lse_for_pipeline[i] = lse[i];
        lse_text_for_pipeline[i] = std::isfinite(lse[i]) ? lse[i] - text_amp : lse[i];
    }

    // Bias is unused by jenga_bwd_dkdv_pipeline, so we skip its allocation/initialization.

    ck_tile::DeviceMem q_buf(q_numel * sizeof(QDataType));
    ck_tile::DeviceMem k_buf(kv_numel * sizeof(KDataType));
    ck_tile::DeviceMem v_buf(kv_numel * sizeof(VDataType));
    ck_tile::DeviceMem o_buf(q_numel * sizeof(ODataType));
    ck_tile::DeviceMem do_buf(q_numel * sizeof(OGradDataType));
    ck_tile::DeviceMem delta_buf(lse_numel * sizeof(AccDataType));
    ck_tile::DeviceMem lse_buf(lse_numel * sizeof(LSEDataType));
    ck_tile::DeviceMem lse_text_buf(lse_numel * sizeof(LSEDataType));
    ck_tile::DeviceMem block_mask_buf(mask_numel * sizeof(int32_t));
    ck_tile::DeviceMem rlut_buf(rlut.size() * sizeof(int32_t));
    ck_tile::DeviceMem rlut_size_buf(rlut_size.size() * sizeof(int32_t));
    ck_tile::DeviceMem seqlens_buf(B * sizeof(int32_t));
    ck_tile::DeviceMem dq_tmp_buf(q_numel * sizeof(AccDataType));
    ck_tile::DeviceMem dk_buf(kv_numel * sizeof(KGradDataType));
    ck_tile::DeviceMem dv_buf(kv_numel * sizeof(VGradDataType));

    q_buf.ToDevice(q.data());
    k_buf.ToDevice(k.data());
    v_buf.ToDevice(v.data());
    o_buf.ToDevice(o.data());
    do_buf.ToDevice(dout.data());
    delta_buf.ToDevice(delta.data());
    lse_buf.ToDevice(lse_for_pipeline.data());
    lse_text_buf.ToDevice(lse_text_for_pipeline.data());
    block_mask_buf.ToDevice(block_mask.data());
    rlut_buf.ToDevice(rlut.data());
    rlut_size_buf.ToDevice(rlut_size.data());
    seqlens_buf.ToDevice(seqlens.data());
    dq_tmp_buf.SetZero();
    dk_buf.SetZero();
    dv_buf.SetZero();

    args.q_ptr = q_buf.GetDeviceBuffer();
    args.k_ptr = k_buf.GetDeviceBuffer();
    args.v_ptr = v_buf.GetDeviceBuffer();
    args.o_ptr = o_buf.GetDeviceBuffer();
    args.do_ptr = do_buf.GetDeviceBuffer();
    args.delta_ptr = delta_buf.GetDeviceBuffer();
    args.lse_ptr = lse_buf.GetDeviceBuffer();
    args.lse_text_ptr = lse_text_buf.GetDeviceBuffer();
    args.bias_ptr = nullptr;
    args.block_mask_ptr = static_cast<const int32_t*>(block_mask_buf.GetDeviceBuffer());
    args.rlut_ptr = static_cast<const int32_t*>(rlut_buf.GetDeviceBuffer());
    args.rlut_size_ptr = static_cast<const int32_t*>(rlut_size_buf.GetDeviceBuffer());
    args.seqlens_ptr = static_cast<const int32_t*>(seqlens_buf.GetDeviceBuffer());
    args.dq_tmp_ptr = dq_tmp_buf.GetDeviceBuffer();
    args.dk_ptr = dk_buf.GetDeviceBuffer();
    args.dv_ptr = dv_buf.GetDeviceBuffer();

    std::cout << "Jenga bwd dK/dV: B=" << B << " H=" << H << " N_Q=" << N_Q
              << " N_KV=" << N_KV << " D=" << D << " q_blocks=" << N_Q_BLOCKS
              << " kv_blocks=" << N_KV_BLOCKS << " text_start=" << text_block_start
              << " text_amp=" << text_amp << " max_nnz_r=" << args.max_nnz_r << std::endl;

    const ck_tile::stream_config stream_config{nullptr, true, gpu_timer ? 1 : 0, warmup, repeat};
    const float ave_time = jenga_bwd_dkdv_pipeline_calc(args, stream_config);
    std::cout << "Average time: " << ave_time << " ms" << std::endl;

    dk_buf.FromDevice(dk_device.data());
    dv_buf.FromDevice(dv_device.data());

    bool pass = true;
    if(do_validation != 0)
    {
        args.q_ptr = q.data();
        args.k_ptr = k.data();
        args.v_ptr = v.data();
        args.o_ptr = o.data();
        args.do_ptr = dout.data();
        args.delta_ptr = delta.data();
        args.lse_ptr = lse.data();
        args.lse_text_ptr = nullptr;
        args.bias_ptr = nullptr;
        args.block_mask_ptr = block_mask.data();
        args.rlut_ptr = rlut.data();
        args.rlut_size_ptr = rlut_size.data();
        args.seqlens_ptr = seqlens.data();
        args.dq_tmp_ptr = nullptr;
        args.dk_ptr = dk_ref.data();
        args.dv_ptr = dv_ref.data();

        reference_jenga_bwd_dkdv(
            q, k, v, dout, delta, lse, rlut, rlut_size, seqlens, args, dk_ref, dv_ref);

        const auto [dk_max, dk_mean] = diff_stats(dk_device, dk_ref);
        const auto [dv_max, dv_mean] = diff_stats(dv_device, dv_ref);
        const double dk_cos = cosine_similarity(dk_device, dk_ref);
        const double dv_cos = cosine_similarity(dv_device, dv_ref);
        const auto [dk_dev_norm, dk_dev_nnz] = norm_and_nnz(dk_device);
        const auto [dv_dev_norm, dv_dev_nnz] = norm_and_nnz(dv_device);
        const auto [dk_ref_norm, dk_ref_nnz] = norm_and_nnz(dk_ref);
        const auto [dv_ref_norm, dv_ref_nnz] = norm_and_nnz(dv_ref);

        std::cout << "dK max_diff=" << dk_max << " mean_diff=" << dk_mean
                  << " cosine=" << dk_cos << std::endl;
        std::cout << "dV max_diff=" << dv_max << " mean_diff=" << dv_mean
                  << " cosine=" << dv_cos << std::endl;
        std::cout << "dK norm(device/ref)=" << dk_dev_norm << "/" << dk_ref_norm
                  << " nnz(device/ref)=" << dk_dev_nnz << "/" << dk_ref_nnz << std::endl;
        std::cout << "dV norm(device/ref)=" << dv_dev_norm << "/" << dv_ref_norm
                  << " nnz(device/ref)=" << dv_dev_nnz << "/" << dv_ref_nnz << std::endl;

        pass = dk_cos > 0.999 && dv_cos > 0.999;
        std::cout << "Validation: " << (pass ? "PASS" : "FAIL") << std::endl;
    }

    return pass ? 1 : 0;
}
