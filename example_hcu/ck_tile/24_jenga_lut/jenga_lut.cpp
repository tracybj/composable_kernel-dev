#include <hip/hip_runtime.h>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

#define HIP_CHECK(cmd)                                                                        \
    do                                                                                        \
    {                                                                                         \
        hipError_t e = (cmd);                                                                 \
        if(e != hipSuccess)                                                                   \
        {                                                                                     \
            std::fprintf(stderr, "HIP error %s:%d: %s\n", __FILE__, __LINE__,                 \
                         hipGetErrorString(e));                                               \
            std::exit(1);                                                                     \
        }                                                                                     \
    } while(false)

__global__ void jenga_build_lut_kernel(const uint8_t* __restrict__ block_mask,
                                       int32_t* __restrict__ lut,
                                       int32_t* __restrict__ lut_size,
                                       int q_blocks,
                                       int k_blocks,
                                       int lut_capacity)
{
    const int q  = blockIdx.x;
    const int bh = blockIdx.y;

    if(threadIdx.x != 0)
    {
        return;
    }

    const size_t mask_base = (static_cast<size_t>(bh) * q_blocks + q) * k_blocks;
    const size_t lut_base  = (static_cast<size_t>(bh) * q_blocks + q) * lut_capacity;

    int count = 0;
    for(int k = 0; k < k_blocks; ++k)
    {
        if(block_mask[mask_base + k] != 0)
        {
            if(count < lut_capacity)
            {
                lut[lut_base + count] = k;
            }
            ++count;
        }
    }

    lut_size[static_cast<size_t>(bh) * q_blocks + q] = count;
    for(int i = count; i < lut_capacity; ++i)
    {
        lut[lut_base + i] = k_blocks;
    }
}

__global__ void jenga_build_reverse_lut_kernel(const uint8_t* __restrict__ block_mask,
                                               int32_t* __restrict__ rlut,
                                               int32_t* __restrict__ rlut_size,
                                               int q_blocks,
                                               int k_blocks,
                                               int rlut_capacity)
{
    const int k  = blockIdx.x;
    const int bh = blockIdx.y;

    if(threadIdx.x != 0)
    {
        return;
    }

    const size_t rlut_base = (static_cast<size_t>(bh) * k_blocks + k) * rlut_capacity;

    int count = 0;
    for(int q = 0; q < q_blocks; ++q)
    {
        const size_t mask_idx = (static_cast<size_t>(bh) * q_blocks + q) * k_blocks + k;
        if(block_mask[mask_idx] != 0)
        {
            if(count < rlut_capacity)
            {
                rlut[rlut_base + count] = q;
            }
            ++count;
        }
    }

    rlut_size[static_cast<size_t>(bh) * k_blocks + k] = count;
    for(int i = count; i < rlut_capacity; ++i)
    {
        rlut[rlut_base + i] = q_blocks;
    }
}

static void fill_wan21_mask(std::vector<uint8_t>& mask, int bh_count, int q_blocks, int k_blocks)
{
    constexpr int top_k = 28;
    std::fill(mask.begin(), mask.end(), uint8_t{0});

    for(int bh = 0; bh < bh_count; ++bh)
    {
        for(int q = 0; q < q_blocks; ++q)
        {
            for(int t = 0; t < top_k; ++t)
            {
                const int k = (q + bh * 13 + t * 7) % k_blocks;
                mask[(static_cast<size_t>(bh) * q_blocks + q) * k_blocks + k] = 1;
            }
        }
    }
    return;
}

static void build_lut_ref(const std::vector<uint8_t>& mask,
                          std::vector<int32_t>& lut,
                          std::vector<int32_t>& lut_size,
                          int bh_count,
                          int q_blocks,
                          int k_blocks,
                          int lut_capacity)
{
    for(int bh = 0; bh < bh_count; ++bh)
    {
        for(int q = 0; q < q_blocks; ++q)
        {
            const size_t mask_base = (static_cast<size_t>(bh) * q_blocks + q) * k_blocks;
            const size_t lut_base  = (static_cast<size_t>(bh) * q_blocks + q) * lut_capacity;
            int count = 0;
            for(int k = 0; k < k_blocks; ++k)
            {
                if(mask[mask_base + k] != 0)
                {
                    lut[lut_base + count] = k;
                    ++count;
                }
            }
            lut_size[static_cast<size_t>(bh) * q_blocks + q] = count;
            for(int i = count; i < lut_capacity; ++i)
            {
                lut[lut_base + i] = k_blocks;
            }
        }
    }
    return;
}

static void build_reverse_lut_ref(const std::vector<uint8_t>& mask,
                                  std::vector<int32_t>& rlut,
                                  std::vector<int32_t>& rlut_size,
                                  int bh_count,
                                  int q_blocks,
                                  int k_blocks,
                                  int rlut_capacity)
{
    for(int bh = 0; bh < bh_count; ++bh)
    {
        for(int k = 0; k < k_blocks; ++k)
        {
            const size_t rlut_base = (static_cast<size_t>(bh) * k_blocks + k) * rlut_capacity;
            int count = 0;
            for(int q = 0; q < q_blocks; ++q)
            {
                if(mask[(static_cast<size_t>(bh) * q_blocks + q) * k_blocks + k] != 0)
                {
                    rlut[rlut_base + count] = q;
                    ++count;
                }
            }
            rlut_size[static_cast<size_t>(bh) * k_blocks + k] = count;
            for(int i = count; i < rlut_capacity; ++i)
            {
                rlut[rlut_base + i] = q_blocks;
            }
        }
    }
    return;
}

static bool compare(const char* name,
                    const std::vector<int32_t>& got,
                    const std::vector<int32_t>& ref,
                    size_t* first_bad)
{
    for(size_t i = 0; i < ref.size(); ++i)
    {
        if(got[i] != ref[i])
        {
            *first_bad = i;
            std::printf("  %s mismatch at flat=%zu got=%d ref=%d\n", name, i, got[i], ref[i]);
            return false;
        }
    }
    return true;
}

int main(int argc, char** argv)
{
    const int warmup = argc > 1 ? std::atoi(argv[1]) : 20;
    const int repeat = argc > 2 ? std::atoi(argv[2]) : 200;

    constexpr int B = 1;
    constexpr int H = 40;
    constexpr int BH = B * H;
    constexpr int seqlen = 18048;
    constexpr int block_size = 64;
    constexpr int q_blocks = seqlen / block_size;
    constexpr int k_blocks = q_blocks;
    constexpr int lut_capacity = 28;
    constexpr int rlut_capacity = 28;

    const size_t mask_numel = static_cast<size_t>(BH) * q_blocks * k_blocks;
    const size_t lut_numel = static_cast<size_t>(BH) * q_blocks * lut_capacity;
    const size_t lut_size_numel = static_cast<size_t>(BH) * q_blocks;
    const size_t rlut_numel = static_cast<size_t>(BH) * k_blocks * rlut_capacity;
    const size_t rlut_size_numel = static_cast<size_t>(BH) * k_blocks;

    std::vector<uint8_t> mask_h(mask_numel);
    std::vector<int32_t> lut_ref(lut_numel);
    std::vector<int32_t> lut_size_ref(lut_size_numel);
    std::vector<int32_t> rlut_ref(rlut_numel);
    std::vector<int32_t> rlut_size_ref(rlut_size_numel);
    std::vector<int32_t> lut_h(lut_numel, -1);
    std::vector<int32_t> lut_size_h(lut_size_numel, -1);
    std::vector<int32_t> rlut_h(rlut_numel, -1);
    std::vector<int32_t> rlut_size_h(rlut_size_numel, -1);

    fill_wan21_mask(mask_h, BH, q_blocks, k_blocks);
    build_lut_ref(mask_h, lut_ref, lut_size_ref, BH, q_blocks, k_blocks, lut_capacity);
    build_reverse_lut_ref(mask_h, rlut_ref, rlut_size_ref, BH, q_blocks, k_blocks, rlut_capacity);

    int max_nnz = 0;
    int max_nnz_r = 0;
    for(int v : lut_size_ref)
    {
        max_nnz = std::max(max_nnz, v);
    }
    for(int v : rlut_size_ref)
    {
        max_nnz_r = std::max(max_nnz_r, v);
    }

    uint8_t* mask_d = nullptr;
    int32_t* lut_d = nullptr;
    int32_t* lut_size_d = nullptr;
    int32_t* rlut_d = nullptr;
    int32_t* rlut_size_d = nullptr;

    HIP_CHECK(hipMalloc(&mask_d, mask_numel * sizeof(uint8_t)));
    HIP_CHECK(hipMalloc(&lut_d, lut_numel * sizeof(int32_t)));
    HIP_CHECK(hipMalloc(&lut_size_d, lut_size_numel * sizeof(int32_t)));
    HIP_CHECK(hipMalloc(&rlut_d, rlut_numel * sizeof(int32_t)));
    HIP_CHECK(hipMalloc(&rlut_size_d, rlut_size_numel * sizeof(int32_t)));

    HIP_CHECK(hipMemcpy(mask_d, mask_h.data(), mask_numel * sizeof(uint8_t), hipMemcpyHostToDevice));

    const dim3 lut_grid(q_blocks, BH, 1);
    const dim3 rlut_grid(k_blocks, BH, 1);
    const dim3 block(64, 1, 1);

    auto launch_lut = [&]() {
        jenga_build_lut_kernel<<<lut_grid, block>>>(
            mask_d, lut_d, lut_size_d, q_blocks, k_blocks, lut_capacity);
    };
    auto launch_rlut = [&]() {
        jenga_build_reverse_lut_kernel<<<rlut_grid, block>>>(
            mask_d, rlut_d, rlut_size_d, q_blocks, k_blocks, rlut_capacity);
    };

    launch_lut();
    launch_rlut();
    HIP_CHECK(hipGetLastError());
    HIP_CHECK(hipDeviceSynchronize());

    HIP_CHECK(hipMemcpy(lut_h.data(), lut_d, lut_numel * sizeof(int32_t), hipMemcpyDeviceToHost));
    HIP_CHECK(hipMemcpy(lut_size_h.data(), lut_size_d, lut_size_numel * sizeof(int32_t), hipMemcpyDeviceToHost));
    HIP_CHECK(hipMemcpy(rlut_h.data(), rlut_d, rlut_numel * sizeof(int32_t), hipMemcpyDeviceToHost));
    HIP_CHECK(hipMemcpy(rlut_size_h.data(), rlut_size_d, rlut_size_numel * sizeof(int32_t), hipMemcpyDeviceToHost));

    size_t first_bad = 0;
    const bool ok_lut_size = compare("lut_size", lut_size_h, lut_size_ref, &first_bad);
    const bool ok_lut = compare("lut", lut_h, lut_ref, &first_bad);
    const bool ok_rlut_size = compare("rlut_size", rlut_size_h, rlut_size_ref, &first_bad);
    const bool ok_rlut = compare("rlut", rlut_h, rlut_ref, &first_bad);
    const bool ok = ok_lut_size && ok_lut && ok_rlut_size && ok_rlut;

    for(int i = 0; i < warmup; ++i)
    {
        launch_lut();
        launch_rlut();
    }
    HIP_CHECK(hipGetLastError());
    HIP_CHECK(hipDeviceSynchronize());

    hipEvent_t start, mid, stop;
    HIP_CHECK(hipEventCreate(&start));
    HIP_CHECK(hipEventCreate(&mid));
    HIP_CHECK(hipEventCreate(&stop));

    HIP_CHECK(hipEventRecord(start, nullptr));
    for(int i = 0; i < repeat; ++i)
    {
        launch_lut();
    }
    HIP_CHECK(hipEventRecord(mid, nullptr));
    for(int i = 0; i < repeat; ++i)
    {
        launch_rlut();
    }
    HIP_CHECK(hipEventRecord(stop, nullptr));
    HIP_CHECK(hipEventSynchronize(stop));

    float lut_ms_total = 0.0f;
    float rlut_ms_total = 0.0f;
    HIP_CHECK(hipEventElapsedTime(&lut_ms_total, start, mid));
    HIP_CHECK(hipEventElapsedTime(&rlut_ms_total, mid, stop));

    const double lut_ms = static_cast<double>(lut_ms_total) / repeat;
    const double rlut_ms = static_cast<double>(rlut_ms_total) / repeat;
    const double both_ms = lut_ms + rlut_ms;

    const double lut_bytes = static_cast<double>(mask_numel) +
                             static_cast<double>(lut_numel + lut_size_numel) * sizeof(int32_t);
    const double rlut_bytes = static_cast<double>(mask_numel) +
                              static_cast<double>(rlut_numel + rlut_size_numel) * sizeof(int32_t);
    const double both_bytes = lut_bytes + rlut_bytes;
    const double both_gbps = both_bytes / (both_ms * 1.0e-3) / 1.0e9;

    std::printf("Jenga block_mask -> LUT benchmark + correctness\n");
    std::printf("  config: wan2.1 B=%d H=%d BH=%d seqlen=%d block_size=%d q_blocks=%d k_blocks=%d top_k=28\n",
                B, H, BH, seqlen, block_size, q_blocks, k_blocks);
    std::printf("  capacities: lut_capacity=%d rlut_capacity=%d max_nnz=%d max_nnz_r=%d\n",
                lut_capacity, rlut_capacity, max_nnz, max_nnz_r);
    std::printf("  correctness=%s lut_size=%s lut=%s rlut_size=%s rlut=%s\n",
                ok ? "PASS" : "FAIL",
                ok_lut_size ? "PASS" : "FAIL",
                ok_lut ? "PASS" : "FAIL",
                ok_rlut_size ? "PASS" : "FAIL",
                ok_rlut ? "PASS" : "FAIL");
    std::printf("  grid_lut=(%u,%u,%u) grid_rlut=(%u,%u,%u) block=(%u,%u,%u) warmup=%d repeat=%d\n",
                lut_grid.x, lut_grid.y, lut_grid.z,
                rlut_grid.x, rlut_grid.y, rlut_grid.z,
                block.x, block.y, block.z, warmup, repeat);
    std::printf("  lut_time_ms=%.6f rlut_time_ms=%.6f total_time_ms=%.6f\n",
                lut_ms, rlut_ms, both_ms);
    std::printf("  approx_bandwidth_GBps=%.3f bytes_per_iter=%.0f\n", both_gbps, both_bytes);

    HIP_CHECK(hipEventDestroy(start));
    HIP_CHECK(hipEventDestroy(mid));
    HIP_CHECK(hipEventDestroy(stop));
    HIP_CHECK(hipFree(mask_d));
    HIP_CHECK(hipFree(lut_d));
    HIP_CHECK(hipFree(lut_size_d));
    HIP_CHECK(hipFree(rlut_d));
    HIP_CHECK(hipFree(rlut_size_d));

    return ok ? 0 : 1;
}
