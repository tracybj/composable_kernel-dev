// SPDX-License-Identifier: MIT
#pragma once

#include "arch/gfx936.hpp"
#include <type_traits>

namespace ck_tile::h3 {

struct RawActivationAccess
{
    static constexpr bool IsStructured = false;

    CK_TILE_DEVICE __attribute__((always_inline)) static auto MakeResource(const void* input, int rows, int k)
    {
        return make_wave_buffer_resource(input, rows * k * 2);
    }

    CK_TILE_DEVICE __attribute__((always_inline)) static auto Load(gfx936::Resource res, int row, int kk, int k)
    {
        const unsigned common = __builtin_amdgcn_readfirstlane(kk & ~31) * 2;
        const uint64_t pointer = bit_cast<uint64_t>(res.lo) + common;
        res.lo = bit_cast<int32x2_t>(pointer);
        res[2] -= common;
        return bit_cast<H3GemmProblem::Vector>(
            amd_buffer_load_impl_inline_asm<bf16_t, 8>(res, (row * k + (kk & 31)) * 2, 0));
    }
};

struct StructuredActivationAccess
{
    static constexpr bool IsStructured = true;

    CK_TILE_DEVICE __attribute__((always_inline)) static auto MakeResource(const void* input, int rows, int k)
    {
        return gfx936::MakeStructuredResource(input, rows, k);
    }

    CK_TILE_DEVICE __attribute__((always_inline)) static auto Load(gfx936::Resource res, int row, int kk, int k)
    {
        return bit_cast<H3GemmProblem::Vector>(gfx936::LoadStructured(res, row, kk, k));
    }
};

template <typename Problem_, typename ActivationAccess_, bool PairedProducer_>
struct H3GemmPolicy
{
    using Problem = Problem_;
    using ActivationAccess = ActivationAccess_;
    using DataType = typename Problem::ADataType;
    using Vector = typename Problem::Vector;
    static constexpr bool PairedProducer = PairedProducer_;
    static constexpr bool Structured = ActivationAccess::IsStructured;

    CK_TILE_HOST_DEVICE static constexpr index_t GetSmemSize()
    {
        return Problem::LdsElements * sizeof(DataType);
    }

    // Verified gfx936 m0 wrap layout. This is shared by both weight slots.
    CK_TILE_DEVICE __attribute__((always_inline)) static int WeightOffset(int row, int col)
    {
        constexpr int WK = Problem::WeightK;
        // Both forms describe the same nonnegative tile coordinates. With the
        // pinned compiler, the scalar-offset producer needs the explicit range
        // type to retain its scheduling; plain/raw keep v07's signed expression.
        using Index = std::conditional_t<PairedProducer, unsigned, int>;
        const Index r = static_cast<Index>(row);
        const Index c = static_cast<Index>(col);
        const Index p = r % 4;
        return (r / (2048 / WK)) * 2048 + p * 512 +
               (((r % (2048 / WK)) / 4) * WK + c + p * 16) % 512;
    }

    template <int FixedSlot = -1>
    CK_TILE_DEVICE __attribute__((always_inline)) static void StageWeight(DataType* shared, gfx936::Resource res,
                                         int loader, int tile, int k)
    {
        constexpr int WK = Problem::WeightK;
        constexpr int FW = Problem::FeatureTile;
        static_assert(FixedSlot >= -1 && FixedSlot <= 1);
        const int slot = FixedSlot < 0 ? tile % 2 : FixedSlot;
        unsigned common = 0;
        if constexpr(PairedProducer)
            common = __builtin_amdgcn_readfirstlane(static_cast<unsigned>(tile * WK * 2));
        const int wave = loader / 64, lane = loader % 64;
        #pragma unroll
        for(int j = 0; j < (WK / 8) * (FW / 128) / 2; ++j)
        {
            const int row = wave + (lane / (WK / 8)) * 4 + j * (2048 / WK);
            const int col = (lane % (WK / 8)) * 8;
            const uintptr_t address = reinterpret_cast<uintptr_t>(shared) +
                                      slot * (FW * WK * 2) + wave * 1024 + j * 4096;
            const uintptr_t m0 = address | (uintptr_t(2 * wave) << 16);
            if constexpr(PairedProducer)
                gfx936::LoadWeightWithScalarOffset(m0, res, (row * k + col) * 2, common);
            else
                hcu_async_buffer_load_asm_impl<DataType, 8>(m0, res, (row * k + tile * WK + col) * 2, 0);
        }
    }

    CK_TILE_DEVICE __attribute__((always_inline)) static Vector LoadWeightFragment(const DataType* address)
    {
        return *reinterpret_cast<const Vector*>(address);
    }
};

using RawPolicy = H3GemmPolicy<H3GemmProblem, RawActivationAccess, false>;
using StructuredPolicy = H3GemmPolicy<H3GemmProblem, StructuredActivationAccess, false>;
using ProducerPolicy = H3GemmPolicy<H3GemmProblem, StructuredActivationAccess, true>;

} // namespace ck_tile::h3
