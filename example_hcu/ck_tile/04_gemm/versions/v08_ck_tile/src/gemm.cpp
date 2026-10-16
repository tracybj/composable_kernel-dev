// SPDX-License-Identifier: MIT
#include "gemm_kernel.hpp"

// The public ABI and exact shape dispatch match v07. No repeated .cpp includes,
// symbol-renaming macros, runtime policy switches, or historical experiment paths.
extern "C" int h3_gemm(const void* input, const void* weight, void* output,
                        int m, int n, int k, hipStream_t stream)
{
    using namespace ck_tile::h3;
    if(n == 5376 && (k == 7168 || k == 3584))
        return Launch<ProducerPolicy>(input, weight, output, m, n, k, stream);
    if((n == 10752 || n == 14336) && k == 5376)
        return Launch<StructuredPolicy>(input, weight, output, m, n, k, stream);
    return Launch<RawPolicy>(input, weight, output, m, n, k, stream);
}

// Explicit direct entries are useful for isolated 4096^3 comparisons. The main
// h3_gemm entry above retains v07's supported domain and fallback decisions.
extern "C" int h3_gemm_structured(const void* input, const void* weight, void* output,
                                   int m, int n, int k, hipStream_t stream)
{
    return ck_tile::h3::Launch<ck_tile::h3::StructuredPolicy>(input, weight, output, m, n, k, stream);
}

extern "C" int h3_gemm_producer(const void* input, const void* weight, void* output,
                                 int m, int n, int k, hipStream_t stream)
{
    return ck_tile::h3::Launch<ck_tile::h3::ProducerPolicy>(input, weight, output, m, n, k, stream);
}
