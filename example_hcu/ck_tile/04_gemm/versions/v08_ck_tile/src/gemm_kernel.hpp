// SPDX-License-Identifier: MIT
#pragma once

#include "gemm_pipeline.hpp"
#include "gemm_epilogue.hpp"

namespace ck_tile::h3 {

// CTA grouping is independent of the producer/consumer and LDS policies.
template <typename Problem_>
struct H3GemmTilePartitioner
{
    using Problem = Problem_;
    struct TileIndex { int feature; int token; };

    CK_TILE_HOST static dim3 GridSize(int tokens, int features)
    {
        return dim3((tokens + Problem::TokenTile - 1) / Problem::TokenTile *
                    (features / Problem::FeatureTile));
    }

    CK_TILE_DEVICE __attribute__((always_inline)) TileIndex operator()(int tokens, int features) const
    {
        const int mt = (features + Problem::FeatureTile - 1) / Problem::FeatureTile;
        const int nt = (tokens + Problem::TokenTile - 1) / Problem::TokenTile;
        const int flat = blockIdx.x;
        const int span = Problem::TokenGroup * mt;
        const int first = (flat / span) * Problem::TokenGroup;
        const int size = min(Problem::TokenGroup, nt - first), local = flat % span;
        return {(local / size) * Problem::FeatureTile, (first + local % size) * Problem::TokenTile};
    }
};

template <typename TilePartitioner_, typename GemmPipeline_, typename EpiloguePipeline_>
struct H3GemmKernel
{
    using TilePartitioner = TilePartitioner_;
    using GemmPipeline = GemmPipeline_;
    using EpiloguePipeline = EpiloguePipeline_;
    using Problem = typename GemmPipeline::Problem;
    using Policy = typename GemmPipeline::Policy;
    using DataType = typename Problem::ADataType;
    using ActivationAccess = typename Policy::ActivationAccess;
    using Acc = typename Problem::AccVector;

    static constexpr int KernelBlockSize = Problem::BlockSize;
    CK_TILE_HOST static dim3 GridSize(int m, int n) { return TilePartitioner::GridSize(m, n); }
    CK_TILE_HOST static constexpr dim3 BlockSize() { return dim3(KernelBlockSize); }
    CK_TILE_HOST_DEVICE static constexpr index_t GetSmemSize() { return GemmPipeline::GetSmemSize(); }

    CK_TILE_DEVICE __attribute__((always_inline)) void operator()(const DataType* weight, const DataType* input,
                                   DataType* output, int features, int tokens, int k,
                                   DataType* shared) const
    {
        const int id = threadIdx.x;
        const auto tile = TilePartitioner{}(tokens, features);
        const int fm = tile.feature, tn = tile.token, count = k / Problem::WeightK;
        if(id >= Problem::ComputeWaves * Problem::WaveSize)
        {
            const auto resource = make_wave_buffer_resource(weight + fm * k, (features - fm) * k * 2);
            GemmPipeline::RunProducer(shared, resource, id - Problem::ComputeWaves * Problem::WaveSize, count, k);
        }
        else
        {
            const int wave = id / 64, lane = id % 64, row = lane % 16, klane = (lane / 16) * 8;
            const int wave_m = wave / Problem::TokenWaves, wave_n = wave % Problem::TokenWaves;
            const auto resource = ActivationAccess::MakeResource(input + tn * k, tokens - tn, k);
            Acc accum[Problem::TokenFrags][Problem::FeatureFrags] = {};
            const auto output_resource = make_wave_buffer_resource(output + tn * features, (tokens - tn) * features * 2);
            GemmPipeline::RunConsumer(shared, resource, accum, count, k, wave, wave_m, wave_n, row, klane);
            EpiloguePipeline{}(accum, output_resource, features, fm, wave_n, row, lane);
        }
    }
};

template <typename Kernel>
__global__ __launch_bounds__(Kernel::KernelBlockSize, 1)
void H3GemmDevice(const typename Kernel::DataType* weight,
                  const typename Kernel::DataType* input,
                  typename Kernel::DataType* output, int features, int tokens, int k)
{
    __shared__ typename Kernel::DataType shared[Kernel::Problem::LdsElements];
    Kernel{}(weight, input, output, features, tokens, k, shared);
}

template <typename Policy>
using H3Kernel = H3GemmKernel<H3GemmTilePartitioner<H3GemmProblem>,
                            H3GemmPipeline<H3GemmProblem, Policy>, H3GemmEpilogue<H3GemmProblem>>;

template <typename Policy>
CK_TILE_HOST int Launch(const void* input, const void* weight, void* output,
                        int m, int n, int k, hipStream_t stream)
{
    if(m <= 0 || n <= 0 || k <= 0 || n % 256 || k % 64 ||
       (reinterpret_cast<uintptr_t>(input) & 15) || (reinterpret_cast<uintptr_t>(weight) & 15))
        return int(hipErrorInvalidValue);
    if constexpr(Policy::Structured)
        if(k >= 8192) return int(hipErrorInvalidValue);
    using Kernel = H3Kernel<Policy>;
    hipLaunchKernelGGL((H3GemmDevice<Kernel>), Kernel::GridSize(m, n), Kernel::BlockSize(), 0, stream,
        static_cast<const bf16_t*>(weight), static_cast<const bf16_t*>(input),
        static_cast<bf16_t*>(output), n, m, k);
    return int(hipGetLastError());
}

} // namespace ck_tile::h3
