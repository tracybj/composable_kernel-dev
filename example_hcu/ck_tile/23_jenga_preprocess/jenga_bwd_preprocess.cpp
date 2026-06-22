#include "ck_tile/core.hpp"
#include "ck_tile/host/kernel_launch.hpp"
#include "jenga_bwd_preprocess_kernel.hpp"
#include "ck_tile/ops/fmha/pipeline/block_fmha_bwd_pipeline_problem.hpp"

#include <hip/hip_runtime.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <string>
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

struct JengaPreprocessTraits
{
    static constexpr bool kPadSeqLenQ             = true;
    static constexpr bool kPadHeadDimV            = false;
    static constexpr ck_tile::index_t kBlockPerCu = 1;
};

using Problem = ck_tile::BlockFmhaBwdOGradDotOPipelineProblem<ck_tile::bf16_t,
                                                              ck_tile::bf16_t,
                                                              float,
                                                              64,
                                                              128,
                                                              false,
                                                              JengaPreprocessTraits>;
using Dot    = ck_tile::BlockFmhaBwdOGradDotO<Problem>;
using Kernel = ck_tile::JengaBwdPreprocessKernel<Dot>;

static float make_o(int bh, int m, int d)
{
    const int x = (bh * 131 + m * 17 + d * 7) % 97;
    return (static_cast<float>(x) - 48.0f) * 0.015625f;
}

static float make_do(int bh, int m, int d)
{
    const int x = (bh * 43 + m * 29 + d * 11) % 89;
    return (static_cast<float>(x) - 44.0f) * 0.013671875f;
}

static double cpu_ref_row(const std::vector<ck_tile::bf16_t>& o_h,
                          const std::vector<ck_tile::bf16_t>& do_h,
                          int L,
                          int D,
                          int bh,
                          int m)
{
    double acc = 0.0;
    const size_t base = (static_cast<size_t>(bh) * L + m) * D;
    for(int d = 0; d < D; ++d)
    {
        acc += static_cast<double>(static_cast<float>(o_h[base + d])) *
               static_cast<double>(static_cast<float>(do_h[base + d]));
    }
    return acc;
}

static std::vector<size_t> make_check_indices(int B, int H, int L)
{
    const int BH = B * H;
    std::vector<size_t> idx;
    auto push = [&](int bh, int m) {
        if(bh >= 0 && bh < BH && m >= 0 && m < L)
        {
            idx.push_back(static_cast<size_t>(bh) * L + m);
        }
    };

    for(int bh : {0, 1, H - 1, H, BH / 2, BH - 1})
    {
        for(int m : {0, 1, 63, 64, 127, 128, L / 4, L / 2, L - 129, L - 65, L - 1})
        {
            push(bh, m);
        }
    }

    const size_t total = static_cast<size_t>(BH) * L;
    for(size_t i = 0; i < 4096 && i < total; ++i)
    {
        size_t v = (i * 2654435761ULL + 12345ULL) % total;
        idx.push_back(v);
    }

    std::sort(idx.begin(), idx.end());
    idx.erase(std::unique(idx.begin(), idx.end()), idx.end());
    return idx;
}

static bool run_one(const char* name, int L, int warmup, int repeat)
{
    constexpr int B  = 1;
    constexpr int H  = 40;
    constexpr int D  = 128;
    constexpr int BH = B * H;

    const size_t numel       = static_cast<size_t>(BH) * L * D;
    const size_t delta_numel = static_cast<size_t>(BH) * L;

    std::vector<ck_tile::bf16_t> o_h(numel);
    std::vector<ck_tile::bf16_t> do_h(numel);
    std::vector<float> delta_h(delta_numel, 0.0f);

    for(int bh = 0; bh < BH; ++bh)
    {
        for(int m = 0; m < L; ++m)
        {
            const size_t base = (static_cast<size_t>(bh) * L + m) * D;
            for(int d = 0; d < D; ++d)
            {
                o_h[base + d]  = ck_tile::bf16_t(make_o(bh, m, d));
                do_h[base + d] = ck_tile::bf16_t(make_do(bh, m, d));
            }
        }
    }

    ck_tile::bf16_t* o = nullptr;
    ck_tile::bf16_t* dout = nullptr;
    float* delta = nullptr;
    int32_t* seqlens = nullptr;

    HIP_CHECK(hipMalloc(&o, numel * sizeof(ck_tile::bf16_t)));
    HIP_CHECK(hipMalloc(&dout, numel * sizeof(ck_tile::bf16_t)));
    HIP_CHECK(hipMalloc(&delta, delta_numel * sizeof(float)));
    HIP_CHECK(hipMalloc(&seqlens, B * sizeof(int32_t)));

    HIP_CHECK(hipMemcpy(o, o_h.data(), numel * sizeof(ck_tile::bf16_t), hipMemcpyHostToDevice));
    HIP_CHECK(hipMemcpy(dout, do_h.data(), numel * sizeof(ck_tile::bf16_t), hipMemcpyHostToDevice));
    HIP_CHECK(hipMemset(delta, 0, delta_numel * sizeof(float)));

    int32_t h_seqlens[B] = {L};
    HIP_CHECK(hipMemcpy(seqlens, h_seqlens, sizeof(h_seqlens), hipMemcpyHostToDevice));

    auto kargs = Kernel::MakeKargs(o,
                                   dout,
                                   delta,
                                   seqlens,
                                   H,
                                   L,
                                   D,
                                   L * D,
                                   D,
                                   1,
                                   L * D,
                                   D,
                                   1,
                                   L,
                                   1,
                                   1.0f);

    const dim3 grid = Kernel::GridSize(B, H, L);
    constexpr dim3 block = Kernel::BlockSize();

    auto callable = ck_tile::make_kernel<Kernel::kBlockSize, Kernel::kBlockPerCu>(
        Kernel{}, grid, block, Kernel::GetSmemSize(), kargs);

    ck_tile::stream_config stream_cfg{nullptr, true, 0, 0, 1};
    callable(stream_cfg);
    HIP_CHECK(hipGetLastError());
    HIP_CHECK(hipDeviceSynchronize());
    HIP_CHECK(hipMemcpy(delta_h.data(), delta, delta_numel * sizeof(float), hipMemcpyDeviceToHost));

    const auto check_indices = make_check_indices(B, H, L);
    double max_abs = 0.0;
    double max_rel = 0.0;
    size_t worst_idx = 0;
    int bad = 0;
    constexpr double abs_tol = 2.5e-3;
    constexpr double rel_tol = 2.5e-3;

    for(size_t flat : check_indices)
    {
        int bh = static_cast<int>(flat / L);
        int m  = static_cast<int>(flat % L);
        double ref = cpu_ref_row(o_h, do_h, L, D, bh, m);
        double got = static_cast<double>(delta_h[flat]);
        double abs_err = std::abs(got - ref);
        double rel_err = abs_err / std::max(1.0, std::abs(ref));
        if(abs_err > max_abs)
        {
            max_abs = abs_err;
            max_rel = rel_err;
            worst_idx = flat;
        }
        if(abs_err > abs_tol && rel_err > rel_tol)
        {
            ++bad;
        }
    }

    for(int i = 0; i < warmup; ++i)
    {
        callable(stream_cfg);
    }
    HIP_CHECK(hipGetLastError());
    HIP_CHECK(hipDeviceSynchronize());

    hipEvent_t start, stop;
    HIP_CHECK(hipEventCreate(&start));
    HIP_CHECK(hipEventCreate(&stop));
    HIP_CHECK(hipEventRecord(start, nullptr));
    for(int i = 0; i < repeat; ++i)
    {
        callable(stream_cfg);
    }
    HIP_CHECK(hipEventRecord(stop, nullptr));
    HIP_CHECK(hipEventSynchronize(stop));

    float total_ms = 0.0f;
    HIP_CHECK(hipEventElapsedTime(&total_ms, start, stop));
    const double avg_ms = static_cast<double>(total_ms) / repeat;

    const double read_bytes = 2.0 * static_cast<double>(numel) * sizeof(ck_tile::bf16_t);
    const double write_bytes = static_cast<double>(delta_numel) * sizeof(float);
    const double total_bytes = read_bytes + write_bytes;
    const double seconds = avg_ms * 1.0e-3;

    const double gbps = total_bytes / seconds / 1.0e9;
    const double g_elem_s = static_cast<double>(delta_numel) * D / seconds / 1.0e9;
    const double rows_s = static_cast<double>(delta_numel) / seconds / 1.0e6;

    const int worst_bh = static_cast<int>(worst_idx / L);
    const int worst_m = static_cast<int>(worst_idx % L);

    std::printf("%s\n", name);
    std::printf("  B=%d H=%d BH=%d L=%d D=%d BLOCK_M=%d\n", B, H, BH, L, D, 64);
    std::printf("  grid=(%u,%u,%u) block=(%u,%u,%u) warmup=%d repeat=%d\n",
                grid.x, grid.y, grid.z, block.x, block.y, block.z, warmup, repeat);
    std::printf("  correctness=%s checked=%zu max_abs=%.8g max_rel=%.8g worst=(bh=%d,m=%d) bad=%d\n",
                bad == 0 ? "PASS" : "FAIL", check_indices.size(), max_abs, max_rel,
                worst_bh, worst_m, bad);
    std::printf("  avg_time_ms=%.6f\n", avg_ms);
    std::printf("  bandwidth_GBps=%.3f  bytes_per_iter=%.0f\n", gbps, total_bytes);
    std::printf("  throughput_Gelem_s=%.3f  rows_M_s=%.3f\n", g_elem_s, rows_s);

    HIP_CHECK(hipEventDestroy(start));
    HIP_CHECK(hipEventDestroy(stop));
    HIP_CHECK(hipFree(o));
    HIP_CHECK(hipFree(dout));
    HIP_CHECK(hipFree(delta));
    HIP_CHECK(hipFree(seqlens));
    return bad == 0;
}

int main(int argc, char** argv)
{
    const std::string which = argc > 1 ? argv[1] : "wan2.1";
    const int warmup = argc > 2 ? std::atoi(argv[2]) : 20;
    const int repeat = argc > 3 ? std::atoi(argv[3]) : 200;

    bool ok = true;
    std::printf("Jenga HCU CK dO*O preprocess benchmark + correctness\n");
    if(which == "wan2.1" || which == "all")
    {
        ok &= run_one("wan2.1", 18048, warmup, repeat);
    }
    if(which == "wan2.2" || which == "all")
    {
        ok &= run_one("wan2.2", 75648, warmup, repeat);
    }
    if(which != "wan2.1" && which != "wan2.2" && which != "all")
    {
        std::fprintf(stderr, "Usage: %s [wan2.1|wan2.2|all] [warmup] [repeat]\n", argv[0]);
        return 2;
    }
    return ok ? 0 : 1;
}
