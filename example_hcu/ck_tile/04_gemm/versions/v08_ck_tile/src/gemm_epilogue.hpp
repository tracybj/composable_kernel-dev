// SPDX-License-Identifier: MIT
#pragma once

#include "arch/gfx936.hpp"

namespace ck_tile::h3 {

template <typename Problem_>
struct H3GemmEpilogue
{
    using Problem = Problem_;
    using ODataType = typename Problem::CDataType;
    using Acc = typename Problem::AccVector;
    static constexpr int MT = Problem::FeatureFrags;
    static constexpr int NT = Problem::TokenFrags;

    CK_TILE_HOST_DEVICE static constexpr index_t GetSmemSize() { return 0; }

    CK_TILE_DEVICE __attribute__((always_inline)) void operator()(const Acc (&accum)[NT][MT], gfx936::Resource output,
                                   int features, int fm, int wave_n, int row, int lane) const
    {
        #pragma unroll
        for(int j = 0; j < NT; ++j)
        {
            #pragma unroll
            for(int i = 0; i < MT; ++i)
            {
                #pragma unroll
                for(int x = 0; x < 4; ++x)
                {
                    // Every token displacement belongs in bounds-checked voffset.
                    // Moving x to scalar soffset would corrupt partial M tails.
                    const unsigned voffset = ((wave_n * NT * 16 + j * 16 + lane / 16 + x * 4) *
                                              features + fm + row) * 2;
                    const auto value = bit_cast<ODataType>(gfx936::ConvertBf16Rne(accum[j][i][x]));
                    gfx936::StoreBf16(value, output, voffset, i * 32);
                }
            }
        }
    }
};

} // namespace ck_tile::h3
