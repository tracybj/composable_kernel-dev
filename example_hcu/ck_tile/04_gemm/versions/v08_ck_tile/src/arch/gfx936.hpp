// SPDX-License-Identifier: MIT
#pragma once

#include "../gemm_problem.hpp"
#include "ck_tile/core/arch/hcu_buffer_addressing.hpp"

namespace ck_tile::h3::gfx936 {

using Resource = int32x4_t;

// One call participates in two consecutive CTA barriers. Producers finish the
// future weight load between them; consumers release the old slot at the second.
CK_TILE_DEVICE __attribute__((always_inline)) void ProducerHandshake()
{
    asm volatile("s_barrier\n\ts_waitcnt vmcnt(0)\n\ts_barrier" ::: "memory");
}

CK_TILE_DEVICE __attribute__((always_inline)) void ScheduleFence() { __builtin_amdgcn_sched_barrier(0); }
CK_TILE_DEVICE __attribute__((always_inline)) void WaitAllGlobal() { asm volatile("s_waitcnt vmcnt(0)" ::: "memory"); }

template <int Outstanding>
CK_TILE_DEVICE __attribute__((always_inline)) void WaitGlobal()
{
    asm volatile("s_waitcnt vmcnt(%0)" :: "n"(Outstanding) : "memory");
}

CK_TILE_DEVICE __attribute__((always_inline)) void EarlyConsumerBarrier()
{
    asm volatile("s_waitcnt lgkmcnt(0)\n\ts_barrier" ::: "memory");
}

CK_TILE_DEVICE __attribute__((always_inline)) void LateConsumerBarrier() { asm volatile("s_barrier" ::: "memory"); }
CK_TILE_DEVICE __attribute__((always_inline)) void RaiseComputePriority() { asm volatile("s_setprio 1" ::: "memory"); }
CK_TILE_DEVICE __attribute__((always_inline)) void RestorePriority() { asm volatile("s_setprio 0" ::: "memory"); }

// Only scheduled full-feature weight tiles may use this scalar-offset load.
// Activation/output bounds must keep all row displacement in checked voffset.
CK_TILE_DEVICE __attribute__((always_inline)) void LoadWeightWithScalarOffset(uintptr_t m0,
                                              const Resource& resource,
                                              unsigned vector_offset,
                                              unsigned scalar_offset)
{
    const unsigned wave_m0 = __builtin_amdgcn_readfirstlane(static_cast<unsigned>(m0));
    asm volatile("s_mov_b32 m0, %0\n\t"
                 "buffer_load_dwordx4 %1, %2, %3 offen lds"
                 :: "s"(wave_m0), "v"(vector_offset), "s"(resource), "s"(scalar_offset)
                 : "memory", "m0");
}

CK_TILE_DEVICE __attribute__((always_inline)) unsigned StructuredStride(int k)
{
    return 1u << (31 - __builtin_clz(static_cast<unsigned>(k * 2)));
}

CK_TILE_DEVICE __attribute__((always_inline)) Resource MakeStructuredResource(const void* input, int rows, int k)
{
    const unsigned stride = StructuredStride(k);
    auto res = make_wave_buffer_resource(input, static_cast<unsigned>(rows) * stride, stride);
    res[2] = rows; // Bounds are logical rows, not bytes. make_wave_buffer_resource sets bit62.
    return res;
}

CK_TILE_DEVICE __attribute__((always_inline)) Resource RebaseStructured(Resource res, int kk, int k)
{
    const unsigned common = __builtin_amdgcn_readfirstlane(static_cast<unsigned>(kk & ~31) * 2);
    constexpr uint64_t mask = (uint64_t(1) << 48) - 1;
    const uint64_t descriptor = bit_cast<uint64_t>(res.lo);
    const uint64_t pointer = ((descriptor & mask) + common) & mask;
    res.lo = bit_cast<int32x2_t>((descriptor & ~mask) | pointer);
    res[2] = common < static_cast<unsigned>(k * 2) ? res[2] : 0;
    return res;
}

CK_TILE_DEVICE __attribute__((always_inline)) Resource LoadStructured(Resource res, int row, int kk, int k)
{
    res = RebaseStructured(res, kk, k);
    const unsigned remainder = static_cast<unsigned>(k * 2) - StructuredStride(k);
    const int32x2_t offsets = {row, static_cast<int>(static_cast<unsigned>(row) * remainder + (kk & 31) * 2)};
    Resource data;
    asm volatile("buffer_load_dwordx4 %0, %1, %2, 0 idxen offen"
                 : "=v"(data) : "v"(offsets), "s"(res) : "memory");
    return data;
}

// CK's BF16 RNE semantics, including infinities and low-payload NaNs.
CK_TILE_DEVICE __attribute__((always_inline)) unsigned short ConvertBf16Rne(float value)
{
    const unsigned u = bit_cast<unsigned>(value);
    const unsigned rounded = u + 0x7fff + ((u >> 16) & 1);
    const unsigned special = u | (unsigned((u & 0xffff) != 0) << 16);
    const unsigned mask = 0u - unsigned((u & 0x7f800000) == 0x7f800000);
    return static_cast<unsigned short>(((rounded & ~mask) | (special & mask)) >> 16);
}

CK_TILE_DEVICE __attribute__((always_inline)) void StoreBf16(bf16_t value, Resource res, unsigned voffset, int immediate)
{
    const unsigned short data = bit_cast<unsigned short>(value);
    asm volatile("buffer_store_short %0, %1, %2, %3 offen offset:%4"
                 :: "v"(data), "v"(voffset), "s"(res), "s"(0), "n"(immediate) : "memory");
}

} // namespace ck_tile::h3::gfx936
