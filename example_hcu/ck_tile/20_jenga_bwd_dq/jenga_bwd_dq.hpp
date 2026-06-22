// SPDX-License-Identifier: MIT
// Copyright (c) 2026, Advanced Micro Devices, Inc. All rights reserved.

#pragma once

#include "ck_tile/host.hpp"
#include "ck_tile/host/kernel_launch.hpp"
#include "jenga_bwd_dq_kernel.hpp"

inline auto create_args(int argc, char* argv[])
{
    ck_tile::ArgParser arg_parser;
    arg_parser.insert("b", "1", "batch size")
        .insert("h", "40", "number of heads")
        .insert("s", "18048", "sequence length")
        .insert("d", "128", "head dimension: currently only 128")
        .insert("bm", "64", "query block size: currently only 64")
        .insert("bn", "64", "key/value block size: currently only 64")
        .insert("nnz", "28", "active key blocks per query block: 4, 28, or 118")
        .insert("prec", "bf16", "fp16 or bf16")
        .insert("text_amp", "0.0", "bias added to text blocks")
        .insert("text_block_start", "1024", "first text block index")
        .insert("v", "0", "validation")
        .insert("v_mode", "cpu_block", "validation mode: cpu_block, cpu_naive, or gpu")
        .insert("kname", "1", "print kernel name")
        .insert("warmup", "10", "cold iterations")
        .insert("repeat", "50", "hot iterations");

    return std::make_tuple(arg_parser.parse(argc, argv), arg_parser);
}

float jenga_bwd_dq(ck_tile::example::jenga::jenga_bwd_dq_traits t,
                   ck_tile::example::jenga::jenga_bwd_dq_args a,
                   ck_tile::stream_config s);
