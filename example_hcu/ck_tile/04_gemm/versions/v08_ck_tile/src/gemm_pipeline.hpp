// SPDX-License-Identifier: MIT
#pragma once

#include "gemm_policy.hpp"

namespace ck_tile::h3 {

// Custom CK-Tile pipeline for asymmetric compute/producer wave roles. It keeps
// v07's audited instruction order instead of routing through a symmetric pipeline.
template <typename Problem_, typename Policy_>
struct H3GemmPipeline
{
    using Problem = Problem_;
    using Policy = Policy_;
    using DataType = typename Problem::ADataType;
    using Vector = typename Problem::Vector;
    using Acc = typename Problem::AccVector;
    using WarpGemm = typename Problem::WarpGemm;
    using ActivationAccess = typename Policy::ActivationAccess;

    static constexpr int MT = Problem::FeatureFrags;
    static constexpr int NT = Problem::TokenFrags;
    static constexpr int BlockSize = Problem::BlockSize;

    CK_TILE_HOST_DEVICE static constexpr index_t GetSmemSize() { return Policy::GetSmemSize(); }

    CK_TILE_DEVICE __attribute__((always_inline)) static void RunProducer(DataType* shared, gfx936::Resource resource,
                                          int loader, int count, int k)
    {
        Policy::StageWeight(shared, resource, loader, 0, k);
        gfx936::ProducerHandshake();
        if constexpr(Policy::PairedProducer)
        {
            for(int tile = 0; tile < count; tile += 2)
            {
                if(tile + 1 < count)
                    Policy::template StageWeight<1>(shared, resource, loader, tile + 1, k);
                gfx936::ProducerHandshake();
                // An odd final tile has one consumer call and one handshake.
                if(tile + 1 < count)
                {
                    if(tile + 2 < count)
                        Policy::template StageWeight<0>(shared, resource, loader, tile + 2, k);
                    gfx936::ProducerHandshake();
                }
            }
        }
        else
        {
            for(int tile = 0; tile < count; ++tile)
            {
                if(tile + 1 < count)
                    Policy::StageWeight(shared, resource, loader, tile + 1, k);
                gfx936::ProducerHandshake();
            }
        }
    }

    CK_TILE_DEVICE __attribute__((always_inline)) static void RunConsumer(DataType* shared, gfx936::Resource resource,
                                          Acc (&accum)[NT][MT], int count, int k,
                                          int wave, int wave_m, int wave_n,
                                          int row, int klane)
    {
        auto consume_wave_group = [&](auto early_tag) __attribute__((always_inline)) {
            constexpr bool early = decltype(early_tag)::value;
            Vector bank[2][NT], a_fragments[MT];
            #pragma unroll
            for(int j = 0; j < NT; ++j)
                bank[0][j] = ActivationAccess::Load(resource, wave_n * NT * 16 + row + j * 16, klane, k);
            gfx936::ProducerHandshake();
            gfx936::ScheduleFence();

            auto consume_tile = [&](int tile, auto slot_tag) __attribute__((always_inline)) {
                constexpr int offset = decltype(slot_tag)::value *
                                       (Problem::FeatureTile * Problem::WeightK);
                #pragma unroll
                for(int phase = 0; phase < 2; ++phase)
                {
                    const int next_phase = tile * 2 + phase + 1;
                    const int next_k = (next_phase / 2) * 64 + (next_phase % 2) * 32 + klane;
                    #pragma unroll
                    for(int i = 0; i < MT; ++i)
                    {
                        if(i == Problem::PrefetchPoint)
                        {
                            #pragma unroll
                            for(int j = 0; j < NT; ++j)
                                bank[1 - phase][j] = ActivationAccess::Load(
                                    resource, wave_n * NT * 16 + row + j * 16, next_k, k);
                        }
                        a_fragments[i] = Policy::LoadWeightFragment(shared + offset +
                            Policy::WeightOffset(wave_m * MT * 16 + row + i * 16, phase * 32 + klane));
                    }
                    // Future loads are newer than current-bank loads. Structured
                    // terminal prefetches have zero rows; drain them before use.
                    if constexpr(Policy::Structured)
                    {
                        if(next_phase >= count * 2) gfx936::WaitAllGlobal();
                        else gfx936::WaitGlobal<NT>();
                    }
                    else gfx936::WaitGlobal<NT>();
                    gfx936::ScheduleFence();
                    if(early)
                    {
                        // All A fragments must be private before releasing LDS.
                        gfx936::ScheduleFence();
                        gfx936::EarlyConsumerBarrier();
                        gfx936::ScheduleFence();
                    }
                    #pragma unroll
                    for(int j = 0; j < NT; ++j)
                    {
                        #pragma unroll
                        for(int i = 0; i < MT; ++i)
                        {
                            WarpGemm{}(accum[j][i], a_fragments[i].lo, bank[phase][j].lo);
                            if(j == 0 && i == 0) gfx936::RaiseComputePriority();
                        }
                    }
                    #pragma unroll
                    for(int j = 0; j < NT; ++j)
                    {
                        #pragma unroll
                        for(int i = 0; i < MT; ++i)
                            WarpGemm{}(accum[j][i], a_fragments[i].hi, bank[phase][j].hi);
                    }
                    gfx936::RestorePriority();
                    if(!early)
                    {
                        gfx936::ScheduleFence();
                        gfx936::LateConsumerBarrier();
                    }
                }
            };
            for(int tile = 0; tile < count; tile += 2)
            {
                consume_tile(tile, number<0>{});
                if(tile + 1 < count) consume_tile(tile + 1, number<1>{});
            }
            // Keep unused terminal destinations live until all loads complete;
            // they cannot be recycled into live A/accumulator registers early.
            gfx936::WaitAllGlobal();
            gfx936::ScheduleFence();
            asm volatile("" :: "v"(bank[0][0]), "v"(bank[0][1]) : "memory");
        };
        if(__builtin_amdgcn_readfirstlane(wave) >= Problem::EarlyWaves)
            consume_wave_group(number<1>{});
        else
            consume_wave_group(number<0>{});
    }
};

} // namespace ck_tile::h3
