// SPDX-License-Identifier: MIT
// Copyright (c) 2026, Advanced Micro Devices, Inc. All rights reserved.

#include "ck_tile/host.hpp"
#include "jenga_bwd_dq.hpp"
#include "jenga_bwd_dq_ref.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

namespace jenga = ck_tile::example::jenga;

template <typename DataType>
float invoke_jenga_bwd_dq(ck_tile::DeviceMem& q_dev,
                          ck_tile::DeviceMem& k_dev,
                          ck_tile::DeviceMem& v_dev,
                          ck_tile::DeviceMem& dout_dev,
                          ck_tile::DeviceMem& deltas_dev,
                          ck_tile::DeviceMem& lse_dev,
                          ck_tile::DeviceMem& dq_dev,
                          ck_tile::DeviceMem& lut_dev,
                          ck_tile::DeviceMem& lut_size_dev,
                          ck_tile::DeviceMem& seqlens_dev,
                          const jenga::jenga_bwd_dq_traits& traits,
                          jenga::jenga_bwd_dq_args args,
                          int kname,
                          int warmup,
                          int repeat)
{
    args.q        = q_dev.GetDeviceBuffer();
    args.k        = k_dev.GetDeviceBuffer();
    args.v        = v_dev.GetDeviceBuffer();
    args.dout     = dout_dev.GetDeviceBuffer();
    args.deltas   = static_cast<const float*>(deltas_dev.GetDeviceBuffer());
    args.lse      = static_cast<const float*>(lse_dev.GetDeviceBuffer());
    args.dq       = dq_dev.GetDeviceBuffer();
    args.lut      = static_cast<const int*>(lut_dev.GetDeviceBuffer());
    args.lut_size = static_cast<const int*>(lut_size_dev.GetDeviceBuffer());
    args.seqlens  = static_cast<const int*>(seqlens_dev.GetDeviceBuffer());

    return jenga_bwd_dq(
        traits, args, ck_tile::stream_config{nullptr, true, kname ? 1 : 0, warmup, repeat});
}

template <typename DataType>
void build_lse(const ck_tile::HostTensor<DataType>& q,
               const ck_tile::HostTensor<DataType>& k,
               const ck_tile::HostTensor<int>& lut,
               const ck_tile::HostTensor<int>& lut_size,
               ck_tile::HostTensor<float>& lse,
               const jenga::jenga_bwd_dq_args& a,
               int block_m,
               int block_n,
               int head_dim)
{
    const float qk_scale = a.sm_scale * 1.4426950408889634f;
    const int q_blocks   = ck_tile::integer_divide_ceil(a.seqlen_q, block_m);

    for(int off_hz = 0; off_hz < a.batch * a.nhead; ++off_hz)
    {
        const int seqlen = a.seqlens[off_hz / a.nhead];
        for(int m = 0; m < seqlen; ++m)
        {
            const int qb      = m / block_m;
            const int active  = lut_size(off_hz, qb);
            float row_max     = -std::numeric_limits<float>::infinity();
            float row_sum_exp = 0.0f;

            for(int pass = 0; pass < 2; ++pass)
            {
                for(int i = 0; i < active; ++i)
                {
                    const int kb    = lut(off_hz, qb, i);
                    const int n_beg = kb * block_n;
                    const int n_end = std::min(n_beg + block_n, seqlen);
                    for(int n = n_beg; n < n_end; ++n)
                    {
                        float qk = 0.0f;
                        for(int d = 0; d < head_dim; ++d)
                        {
                            qk += ck_tile::type_convert<float>(q(off_hz, m, d)) *
                                  ck_tile::type_convert<float>(k(off_hz, n, d));
                        }
                        qk = qk * qk_scale + (kb >= a.text_block_start ? a.text_amp : 0.0f);
                        if(pass == 0)
                        {
                            row_max = std::max(row_max, qk);
                        }
                        else
                        {
                            row_sum_exp += std::exp2(qk - row_max);
                        }
                    }
                }
            }

            lse(off_hz, m) = row_max + std::log2(row_sum_exp);
        }
    }
}

template <typename DataType>
bool run(const ck_tile::ArgParser& arg_parser)
{
    const int batch            = arg_parser.get_int("b");
    const int nhead            = arg_parser.get_int("h");
    const int seqlen           = arg_parser.get_int("s");
    const int head_dim         = arg_parser.get_int("d");
    const int block_m          = arg_parser.get_int("bm");
    const int block_n          = arg_parser.get_int("bn");
    const int max_nnz          = arg_parser.get_int("nnz");
    const int validate         = arg_parser.get_int("v");
    const std::string v_mode    = arg_parser.get_str("v_mode");
    const int kname            = arg_parser.get_int("kname");
    const int warmup           = arg_parser.get_int("warmup");
    const int repeat           = arg_parser.get_int("repeat");


    if(head_dim != 128 || block_m != 64 || block_n != 64 ||
       !(max_nnz == 4 || max_nnz == 28 || max_nnz == 118))
    {
        std::cerr << "unsupported jenga_bwd_dq shape: currently supports d=128, bm=64, "
                     "bn=64, nnz in {4, 28, 118}; got d="
                  << head_dim << " bm=" << block_m << " bn=" << block_n
                  << " nnz=" << max_nnz << std::endl;
        return false;
    }

    const int bh               = batch * nhead;
    const int q_blocks         = ck_tile::integer_divide_ceil(seqlen, block_m);
    const int kv_blocks        = ck_tile::integer_divide_ceil(seqlen, block_n);
    const float sm_scale       = 1.0f / std::sqrt(static_cast<float>(head_dim));

    using namespace ck_tile::literals;

    auto f_bhd_descriptor = [](std::size_t bh_, std::size_t s_, std::size_t d_) {
        return ck_tile::HostTensorDescriptor({bh_, s_, d_}, {s_ * d_, d_, 1_uz});
    };
    auto f_bh_seq_descriptor = [](std::size_t bh_, std::size_t s_) {
        return ck_tile::HostTensorDescriptor({bh_, s_}, {s_, 1_uz});
    };
    auto f_lut_descriptor = [](std::size_t bh_, std::size_t q_blocks_, std::size_t max_nnz_) {
        return ck_tile::HostTensorDescriptor(
            {bh_, q_blocks_, max_nnz_}, {q_blocks_ * max_nnz_, max_nnz_, 1_uz});
    };

    ck_tile::HostTensor<DataType> q(f_bhd_descriptor(bh, seqlen, head_dim));
    ck_tile::HostTensor<DataType> k(f_bhd_descriptor(bh, seqlen, head_dim));
    ck_tile::HostTensor<DataType> v(f_bhd_descriptor(bh, seqlen, head_dim));
    ck_tile::HostTensor<DataType> dout(f_bhd_descriptor(bh, seqlen, head_dim));
    ck_tile::HostTensor<DataType> dq(f_bhd_descriptor(bh, seqlen, head_dim));
    ck_tile::HostTensor<DataType> dq_ref(f_bhd_descriptor(bh, seqlen, head_dim));
    ck_tile::HostTensor<float> deltas(f_bh_seq_descriptor(bh, seqlen));
    ck_tile::HostTensor<float> lse(f_bh_seq_descriptor(bh, seqlen));
    ck_tile::HostTensor<int> seqlens({batch});
    ck_tile::HostTensor<int> lut(f_lut_descriptor(bh, q_blocks, max_nnz));
    ck_tile::HostTensor<int> lut_size({bh, q_blocks}, {q_blocks, 1});

    ck_tile::FillUniformDistribution<DataType>{-0.5f, 0.5f}(q);
    ck_tile::FillUniformDistribution<DataType>{-0.5f, 0.5f}(k);
    ck_tile::FillUniformDistribution<DataType>{-0.5f, 0.5f}(v);
    ck_tile::FillUniformDistribution<DataType>{-0.5f, 0.5f}(dout);
    ck_tile::FillUniformDistribution<float>{-0.5f, 0.5f}(deltas);
    dq.SetZero();
    dq_ref.SetZero();
    lse.SetZero();
    lut.SetZero();
    lut_size.SetZero();

    for(int z = 0; z < bh; ++z)
    {
        for(int qb = 0; qb < q_blocks; ++qb)
        {
            const int active = std::min(max_nnz, kv_blocks);
            lut_size(z, qb)  = active;
            for(int i = 0; i < max_nnz; ++i)
            {
                lut(z, qb, i) = (qb * max_nnz + i) % kv_blocks;
            }
        }
    }

    for(int b = 0; b < batch; ++b)
    {
        seqlens(b) = seqlen;
    }

    jenga::jenga_bwd_dq_args host_args;
    host_args.q                = q.data();
    host_args.k                = k.data();
    host_args.v                = v.data();
    host_args.dout             = dout.data();
    host_args.deltas           = deltas.data();
    host_args.lse              = lse.data();
    host_args.dq               = dq_ref.data();
    host_args.lut              = lut.data();
    host_args.lut_size         = lut_size.data();
    host_args.seqlens          = seqlens.data();
    host_args.sm_scale         = sm_scale;
    host_args.text_amp         = arg_parser.get_float("text_amp");
    host_args.text_block_start = arg_parser.get_int("text_block_start");
    host_args.stride_qz        = seqlen * head_dim;
    host_args.stride_qm        = head_dim;
    host_args.stride_qk        = 1;
    host_args.stride_kz        = seqlen * head_dim;
    host_args.stride_kn        = head_dim;
    host_args.stride_kk        = 1;
    host_args.stride_vz        = seqlen * head_dim;
    host_args.stride_vn        = head_dim;
    host_args.stride_vk        = 1;
    host_args.stride_doz       = seqlen * head_dim;
    host_args.stride_dom       = head_dim;
    host_args.stride_dok       = 1;
    host_args.stride_dqz       = seqlen * head_dim;
    host_args.stride_dqm       = head_dim;
    host_args.stride_dqk       = 1;
    host_args.stride_dz        = seqlen;
    host_args.stride_dm        = 1;
    host_args.stride_lz        = seqlen;
    host_args.stride_lm        = 1;
    host_args.stride_lutz      = q_blocks * max_nnz;
    host_args.stride_lutm      = max_nnz;
    host_args.stride_lutk      = 1;
    host_args.block_m          = block_m;
    host_args.block_n          = block_n;
    host_args.max_nnz          = max_nnz;
    host_args.batch            = batch;
    host_args.nhead            = nhead;
    host_args.seqlen_q         = seqlen;

    if(validate && v_mode != "cpu_block" && v_mode != "cpu_naive" && v_mode != "gpu")
    {
        std::cerr << "unsupported validation mode: " << v_mode
                  << " (expected cpu_block, cpu_naive, or gpu)" << std::endl;
        return false;
    }

    ck_tile::DeviceMem q_dev(q.get_element_space_size_in_bytes());
    ck_tile::DeviceMem k_dev(k.get_element_space_size_in_bytes());
    ck_tile::DeviceMem v_dev(v.get_element_space_size_in_bytes());
    ck_tile::DeviceMem dout_dev(dout.get_element_space_size_in_bytes());
    ck_tile::DeviceMem deltas_dev(deltas.get_element_space_size_in_bytes());
    ck_tile::DeviceMem lse_dev(lse.get_element_space_size_in_bytes());
    ck_tile::DeviceMem dq_dev(dq.get_element_space_size_in_bytes());
    ck_tile::DeviceMem dq_ref_dev(dq_ref.get_element_space_size_in_bytes());
    ck_tile::DeviceMem lut_dev(lut.get_element_space_size_in_bytes());
    ck_tile::DeviceMem lut_size_dev(lut_size.get_element_space_size_in_bytes());
    ck_tile::DeviceMem seqlens_dev(seqlens.get_element_space_size_in_bytes());

    q_dev.ToDevice(q.data());
    k_dev.ToDevice(k.data());
    v_dev.ToDevice(v.data());
    dout_dev.ToDevice(dout.data());
    deltas_dev.ToDevice(deltas.data());
    dq_dev.SetZero();
    dq_ref_dev.SetZero();
    lut_dev.ToDevice(lut.data());
    lut_size_dev.ToDevice(lut_size.data());
    seqlens_dev.ToDevice(seqlens.data());

    if(validate)
    {
        std::cout << "Generating " << v_mode << " reference..." << std::flush;
        if(v_mode == "gpu")
        {
            jenga::jenga_bwd_dq_args ref_args = host_args;
            ref_args.q                        = q_dev.GetDeviceBuffer();
            ref_args.k                        = k_dev.GetDeviceBuffer();
            ref_args.v                        = v_dev.GetDeviceBuffer();
            ref_args.dout                     = dout_dev.GetDeviceBuffer();
            ref_args.deltas = static_cast<const float*>(deltas_dev.GetDeviceBuffer());
            ref_args.lse    = static_cast<const float*>(lse_dev.GetDeviceBuffer());
            ref_args.dq     = dq_ref_dev.GetDeviceBuffer();
            ref_args.lut    = static_cast<const int*>(lut_dev.GetDeviceBuffer());
            ref_args.lut_size = static_cast<const int*>(lut_size_dev.GetDeviceBuffer());
            ref_args.seqlens  = static_cast<const int*>(seqlens_dev.GetDeviceBuffer());

            if(!jenga::reference_jenga_bwd_dq_gpu<DataType>(ref_args))
            {
                return false;
            }
        }
        else
        {
            build_lse(q, k, lut, lut_size, lse, host_args, block_m, block_n, head_dim);
            if(v_mode == "cpu_naive")
            {
                jenga::reference_jenga_bwd_dq_naive<DataType>(host_args);
            }
            else
            {
                jenga::reference_jenga_bwd_dq_blockwise<DataType>(host_args);
            }
            lse_dev.ToDevice(lse.data());
        }
        std::cout << "done" << std::endl;
    }
    else
    {
        lse_dev.ToDevice(lse.data());
    }

    jenga::jenga_bwd_dq_traits traits{arg_parser.get_str("prec"), block_m, block_n, head_dim, max_nnz};
    const float ave_time = invoke_jenga_bwd_dq<DataType>(q_dev,
                                                         k_dev,
                                                         v_dev,
                                                         dout_dev,
                                                         deltas_dev,
                                                         lse_dev,
                                                         dq_dev,
                                                         lut_dev,
                                                         lut_size_dev,
                                                         seqlens_dev,
                                                         traits,
                                                         host_args,
                                                         kname,
                                                         warmup,
                                                         repeat);

    const std::size_t num_byte = q.get_element_space_size_in_bytes() +
                                 k.get_element_space_size_in_bytes() +
                                 v.get_element_space_size_in_bytes() +
                                 dout.get_element_space_size_in_bytes() +
                                 deltas.get_element_space_size_in_bytes() +
                                 lse.get_element_space_size_in_bytes() +
                                 dq.get_element_space_size_in_bytes() +
                                 lut.get_element_space_size_in_bytes() +
                                 lut_size.get_element_space_size_in_bytes() +
                                 seqlens.get_element_space_size_in_bytes();
    const float gb_per_sec = num_byte / 1.E6f / ave_time;
    std::cout << "[" << arg_parser.get_str("prec") << "]"
              << " b:" << batch << " h:" << nhead << " s:" << seqlen << " d:" << head_dim
              << " bm:" << block_m << " bn:" << block_n << " nnz:" << max_nnz
              << " scale:" << sm_scale << ", " << ave_time << " ms, " << gb_per_sec
              << " GB/s" << std::flush;

    bool pass = true;
    if(validate)
    {
        dq_dev.FromDevice(dq.data());
        if(v_mode == "gpu")
        {
            dq_ref_dev.FromDevice(dq_ref.data());
        }
        pass = ck_tile::check_err(dq, dq_ref, "dq Error: Incorrect results!", 2e-2, 2e-2);
        std::cout << ", valid:" << (pass ? "y" : "n") << "(" << v_mode << ")";
    }
    std::cout << std::endl;

    return pass;
}

int main(int argc, char* argv[])
{
    bool parse_result;
    ck_tile::ArgParser arg_parser;
    std::tie(parse_result, arg_parser) = create_args(argc, argv);
    if(!parse_result)
    {
        return -1;
    }

    if(arg_parser.get_str("prec") == "fp16")
    {
        return run<ck_tile::half_t>(arg_parser) ? 0 : 1;
    }
    if(arg_parser.get_str("prec") == "bf16")
    {
        return run<ck_tile::bf16_t>(arg_parser) ? 0 : 1;
    }

    std::cerr << "unsupported prec: " << arg_parser.get_str("prec") << std::endl;
    return 1;
}
