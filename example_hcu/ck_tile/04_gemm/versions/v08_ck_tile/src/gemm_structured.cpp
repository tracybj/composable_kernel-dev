// SPDX-License-Identifier: MIT
#include "gemm_kernel.hpp"

extern "C" int h3_gemm(const void* input, const void* weight, void* output,
                        int m, int n, int k, hipStream_t stream)
{
    return ck_tile::h3::Launch<ck_tile::h3::StructuredPolicy>(input, weight, output, m, n, k, stream);
}
