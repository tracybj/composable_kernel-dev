// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2026, Advanced Micro Devices, Inc. All rights reserved.

#pragma once

#include <tuple>
#include "ck_tile/core.hpp"
#include "ck_tile/host.hpp"
#include "ck_tile/host/kernel_launch.hpp"
#include "jenga_bwd_dkdv_config.hpp"

struct JengaBwdDkdvTypeConfig
{
    using QDataType     = ck_tile::bf16_t;
    using KDataType     = ck_tile::bf16_t;
    using VDataType     = ck_tile::bf16_t;
    using ODataType     = ck_tile::bf16_t;
    using OGradDataType = ck_tile::bf16_t;
    using KGradDataType = ck_tile::bf16_t;
    using VGradDataType = ck_tile::bf16_t;
    using AccDataType   = float;
    using LSEDataType   = float;
};

using QDataType     = JengaBwdDkdvTypeConfig::QDataType;
using KDataType     = JengaBwdDkdvTypeConfig::KDataType;
using VDataType     = JengaBwdDkdvTypeConfig::VDataType;
using ODataType     = JengaBwdDkdvTypeConfig::ODataType;
using OGradDataType = JengaBwdDkdvTypeConfig::OGradDataType;
using KGradDataType = JengaBwdDkdvTypeConfig::KGradDataType;
using VGradDataType = JengaBwdDkdvTypeConfig::VGradDataType;
using AccDataType   = JengaBwdDkdvTypeConfig::AccDataType;
using LSEDataType   = JengaBwdDkdvTypeConfig::LSEDataType;

inline auto create_args(int argc, char* argv[])
{
    ck_tile::ArgParser arg_parser;
    arg_parser.insert("b", "1", "batch size")
        .insert("h", "40", "number of heads")
        .insert("n_q", "18048", "query sequence length")
        .insert("n_kv", "18048", "key/value sequence length")
        .insert("d", "128", "head dimension; first version supports 128")
        .insert("m0", "64", "query block size")
        .insert("n0", "64", "key/value block size")
        .insert("text_blocks", "1", "number of trailing text KV blocks")
        .insert("text_amp", "0.0", "text block logit bias")
        .insert("mask_radius", "2", "local block mask radius before text blocks")
        .insert("mask_type", "synthetic", "mask type: synthetic or random")
        .insert("k_active", "0", "number of active key blocks per query block (only for mask_type=random; 0 means automatic)")
        .insert("seed", "42", "random seed")
        .insert("v", "1", "CPU validation")
        .insert("warmup", "2", "warmup iterations")
        .insert("repeat", "3", "timed iterations")
        .insert("timer", "gpu", "gpu or cpu timer");

    bool result = arg_parser.parse(argc, argv);
    return std::make_tuple(result, arg_parser);
}

float jenga_bwd_dkdv_pipeline_calc(const ck_tile::example::jenga::jenga_bwd_dkdv_args& args,
                                    const ck_tile::stream_config& s);
