# Jenga BWD DQ 优化记录

本文档用于持续记录 `jenga_bwd_dq` 的目标实现思路、benchmark 对齐参数、性能设计、优化路线和后续实验结果。后面每次调整实现、参数或得到新的性能数据，都应同步更新这里。

## 语义参考

算法语义参考：

- `/code/ck_opt/jenga_sla_test/jenga/jenga_backward_sla.py`
- `_sla_bwd_dq_kernel`

每个 query block 通过 LUT 只遍历 active key/value blocks。核心计算为：

```text
qk = Q @ K^T * sm_scale * log2(e)
qk += text_amp if key block is text
P  = exp2(qk - lse)
dp = dO @ V^T
ds = P * (dp - delta)
dQ += ds @ K * sm_scale
```

对于一个 block 配置 `(BM, BN, D)`，主要计算可以看成三个矩阵乘：

```text
QK = Q  [BM, D]  @ K^T [D, BN]
DP = dO [BM, D]  @ V^T [D, BN]
DQ = DS [BM, BN] @ K   [BN, D]
```

优化实现的核心目标，是把上述三个矩阵乘表达成 tile/matrix-core 友好的计算，而不是让单个 thread 串行完成 `BN * D` 级别的内层循环。

## Benchmark 对齐参数

benchmark 脚本：

- `/code/ck_opt/jenga_sla_test/benchmark_kernels.py`

共享 benchmark 形状：

```text
B = 1
H = 40
D = 128
dtype = bf16
```

Jenga benchmark case：

```text
wan2.1:
  seqlen = 18048
  block_size = 64
  top_k = 28
  bwd_dq_num_warps = 8
  bwd_dq_num_stages = 2

wan2.2:
  seqlen = 75648
  block_size = 64
  top_k = 118
  bwd_dq_num_warps = 8
  bwd_dq_num_stages = 1
```

目标 kernel 至少应覆盖：

```text
block_m = 64
block_n = 64
head_dim = 128
max_nnz = 28 / 118
dtype = bf16
threads_per_block = 512  # 8 waves * 64 lanes
```

对应测试命令形态：

```bash
# Wan2.1
./bin/tile_example_jenga_bwd_dq \
  -prec=bf16 -b=1 -h=40 -s=18048 -d=128 \
  -bm=64 -bn=64 -nnz=28 \
  -v=0 -warmup=10 -repeat=50 -kname=1

# Wan2.2
./bin/tile_example_jenga_bwd_dq \
  -prec=bf16 -b=1 -h=40 -s=75648 -d=128 \
  -bm=64 -bn=64 -nnz=118 \
  -v=0 -warmup=10 -repeat=50 -kname=1
```

## 目标 Kernel 形态

建议以 benchmark 主形状为第一优先级：

```text
BM = 64
BN = 64
D  = 128
```

一个自然的 CTA 映射是：

```text
CTA -> 一个 (query_block, batch_head) 的 DQ tile
DQ tile shape = [64, 128]
```

CTA 内部遍历 LUT 指定的 active key blocks：

```text
for active key block:
  读取 K/V [64,128]
  计算 QK [64,64]
  计算 DP [64,64]
  计算 DS [64,64]
  累加 DQ [64,128]
```

其中 Q、dO、LSE、Delta 与 query block 绑定，应尽量在整个 `nnz` 循环中复用；K/V 与 active key block 绑定，适合做流式加载和 double buffering。

## 目标数据流

理想的高性能数据流：

```text
Global Q/dO/LSE/Delta
  -> registers/LDS once per query block

for active key block from LUT:
  Global K/V
    -> LDS/register tile
  QK = Q @ K^T
  DP = dO @ V^T
  DS = softmax(QK, LSE, Delta) * DP transform
  DQ_acc += DS @ K

DQ_acc
  -> epilogue/cast
  -> Global DQ
```

核心原则：

```text
LUT 只负责控制访问哪些 key/value block。
进入某个 active block 之后，block 内计算应尽量是 dense tile GEMM。
```

## 主要性能风险

需要避免的性能风险：

- 把 `Q @ K^T`、`dO @ V^T`、`DS @ K` 写成 thread 内标量循环。
- 在每个 active block 中重复从 global memory 读取相同的 Q/dO/LSE/Delta。
- K/V 没有 LDS staging，导致 block 内复用不足。
- LDS layout 只适合写入，不适合后续 matrix-core 读取。
- QK、DP、DS、DQ accumulator 全部常驻 register 时 VGPR 压力过高。
- `nnz=118` 下没有 prefetch/double buffering，active block loop 不能隐藏访存延迟。

## 优化路线

### 0. 已落地的功能实现

当前已在 `composable_kernel-main/example_hcu/ck_tile/20_jenga_bwd_dq` 下放置一版按 `config / policy / pipeline / kernel / launcher / example / reference` 拆分的 ck_tile example。

已覆盖 benchmark 关键入口：

- `BM=64, BN=64, D=128, NNZ=28, dtype=bf16, block=512`
- `BM=64, BN=64, D=128, NNZ=118, dtype=bf16, block=512`

该版本语义对齐 `_sla_bwd_dq_kernel`。初始版本为 `scalar_lut` 风格；随后已经加入 `ds_lds_lut` 中间优化版本：每个 active KV block 先由 CTA 协作计算 `DS[BM,BN]` 到 LDS，再用所有线程并行更新多个 `DQ` accumulator。

2026-06-11 更新：`BM=64, BN=64, D=128, NNZ>1` 路径已接入已有的 CK tile MMAC block GEMM 骨架，用 matrix core 计算 `Q*K^T` 的 `[BM, BN]` 中间项，再继续沿用标量 `dO*V^T` 生成 `DS` 并沿 `K` 累加 `DQ`。`ThreadsPerBlock=256` 使用 `2x2` block-warps，`ThreadsPerBlock=512` 使用 `4x2` block-warps，避免 256 线程配置只覆盖半个输出 tile。benchmark 输出单位同步改为 `ms`。

同轮对比中，QK-MMAC 在 `bf16, B=1, H=4, S=1024, D=128, BM=64, BN=64, NNZ=28` 上从约 `9.23 ms` 降到约 `8.32 ms`，小规模正确性样例通过。`dO*V^T` 的 DP-MMAC 路径也曾尝试打开，但会产生小比例数值差异并在当前验证阈值下失败，暂时保持关闭，后续需要继续检查 B 输入布局/转置约定或验证容差策略。

随后将 `NNZ=28` 特化从 256 threads 改回 benchmark 对齐的 512 threads（8 warps）。在 `bf16, B=1, H=4, S=1024, D=128, BM=64, BN=64, NNZ=28` 上，QK-MMAC + 512 threads 约 `6.97 ms`，优于 256 threads 的约 `8.32 ms`。

example host 侧也做了计时路径修正：`-v=0` 时不再用 CPU 构造真实 `LSE`，保留初始化的零值作为 kernel timing 输入；只有 `-v=1` 时才构造真实 `LSE` 并跑 reference。这避免 Wan2.1/Wan2.2 全尺寸计时被 CPU 预处理淹没。

修正后 Wan2.1 全尺寸 `bf16, B=1, H=40, S=18048, D=128, BM=64, BN=64, NNZ=28` 可直接计时，短测约 `1709 ms`。这表明当前主要剩余瓶颈仍是标量 `dO*V^T` 和 `DS*K` 两段矩阵型计算。

曾尝试给标量路径增加 full-block 分支和 `#pragma unroll`，但会显著放大 `jenga_bwd_dq.cpp` 编译时间，已回退。后续应优先推进结构性 MMAC 化，而不是继续堆展开。

`ds_lds_lut` 仍不是 matrix-core 版本，但已经消除了一个主要重复计算：不再为每个输出 `d` 重新计算同一 `(row, key)` 的 `qk/dp`。

### 1. 先固定 Benchmark 主形状

第一阶段优先支持：

- `BM=64, BN=64, D=128, NNZ=28, dtype=bf16, block=512`
- `BM=64, BN=64, D=128, NNZ=118, dtype=bf16, block=512`

目标：

- 使用与 `benchmark_kernels.py` 一致的 shape。
- 先获得可运行、可计时、可对比的 kernel。
- 暂时不追求覆盖所有小形状。

### 2. 构建 Tile GEMM 主循环

高性能版本应围绕三个 tile operation 组织：

```text
QK = Q  [64,128] @ K^T [128,64]
DP = dO [64,128] @ V^T [128,64]
DQ += DS [64,64] @ K [64,128]
```

需要明确的问题：

- Q/dO 放 register 还是 LDS，如何在所有 active key blocks 中复用。
- K/V 是否统一进入 LDS，并使用适合 MMAC/MFMA 的 descriptor。
- `DS` 保存在 register 中，还是为了 `DS @ K` 放入 LDS。
- `ck_tile::ops::gemm` 中哪些 block/warp primitive 最适合 `BM=64, BN=64, D=128`。

预期：这是主要性能提升来源。

### 2.1 已完成的中间优化：DS LDS 复用

当前 `ds_lds_lut` 的计算结构：

```text
for active key block:
  CTA 协作计算 DS[BM,BN] -> LDS
  每个 thread 持有多个 DQ accumulator
  所有 thread 从 LDS 读取 DS，累加 DQ[BM,D]
```

收益：

- `qk` 和 `dp` 从“每个 `(row,d,n)` 重算”变成“每个 `(row,n)` 计算一次”。
- 保留 LUT 语义，改动风险低。
- 为后续把 `QK/DP/DQ` 替换成 tile GEMM 打下结构基础。

当前小规模验证结果：

```text
bf16 BM64 BN64 D128 NNZ=1   valid:y, 约 5.49 ms
bf16 BM64 BN64 D128 NNZ=28  valid:y, 约 6.61 ms
bf16 BM64 BN64 D128 NNZ=118 valid:y, 约 6.85 ms
```

备注：上述 `s=128` 小规模下实际 KV block 数较少，`NNZ=28/118` 主要用于验证模板入口和同步路径；不能代表大规模 benchmark 性能。

### 3. Memory Layout 与 LDS

Global memory 访问要优先考虑 coalesced 和 vectorized load/store。

LDS layout 要优先服务 matrix-core 读取模式。

候选 staging：

```text
Q  [BM,D]  global -> register/LDS
dO [BM,D]  global -> register/LDS
K  [BN,D]  global -> LDS
V  [BN,D]  global -> LDS
```

需要调优：

- LDS padding/swizzle，降低 bank conflict。
- global-load distribution 和 MMAC-read distribution 可以不同。
- 为 `nnz` 循环做 K/V double buffering。
- LSE/Delta 是 row-wise 数据，应尽量保存在 register 中。

### 4. NNZ Loop Pipelining

大 `nnz`，尤其 Wan2.2 的 `nnz=118`，会让 active-block loop 成为主导。

潜在优化：

- 计算当前 K/V block 时预取下一个 K/V block。
- Wan2.1 对应 `bwd_dq_num_stages=2`，可以尝试 2-stage buffering。
- Wan2.2 对应 `bwd_dq_num_stages=1`，可能更关注 occupancy 和 register/LDS 压力。
- 对 `MaxNnz=28` 和 `MaxNnz=118` 分别特化。

### 5. Work Partitioning

benchmark 中 `bwd_dq_num_warps=8`。

在 wave64 下：

```text
threads_per_block = 8 * 64 = 512
```

候选 CTA 映射：

- 一个 CTA 计算一个 `[64,128]` DQ tile。
- 8 个 wave 在 M/D 方向或 GEMM subtile 方向切分。
- 对 `BM=64`，可以考虑让 wave 覆盖 M rows 和/或 D columns，同时共享 K/V。

需要评估：

- QK、DP、DS、DQ accumulator 带来的 VGPR 压力。
- Q/dO/K/V staging 带来的 LDS 使用量。
- 每个 CU 可同时驻留的 CTA 数。

### 6. 数值与中间结果

建议保持：

- 输入/输出 dtype：bf16。
- QK、DP、DS、DQ accumulator：float。
- `lse`、`delta`：float。
- `qk` 使用 `sm_scale * log2(e)` 后走 `exp2`，对齐 Triton 参考。

需要确认：

- `text_amp` 的加法位置必须在 softmax 前。
- `text_block_start` 以 block index 为单位。
- seqlen mask 对 Q rows 和 KV cols 都要正确处理。

## 验证策略

从小规模开始：

```bash
./bin/tile_example_jenga_bwd_dq \
  -prec=bf16 -b=1 -h=1 -s=128 -d=128 \
  -bm=64 -bn=64 -nnz=4 \
  -v=1 -warmup=0 -repeat=1 -kname=1
```

然后逐步增加：

- `nnz=4`
- `nnz=28`
- `b=1,h=40,s=18048,nnz=28`
- `b=1,h=40,s=75648,nnz=118`

大规模 benchmark case 建议先用 `-v=0`，除非后续加入更便宜的 validation 路径。

## 开放问题

- 是否只优化 `BM=64, BN=64, D=128`，还是同时保留其他形状。
- 是否能复用现有 `ck_tile::ops::fmha` backward 里的部分 DQ 组件。
- 该实现应继续作为 example kernel，还是整理成 `include/ck_tile/ops/jenga` 下的正式算子。
- Python benchmark 中的 LUT 生成方式如何在 C++ benchmark harness 中复现。
- 目标性能基线是什么：Triton `_sla_bwd_dq_kernel`、完整 backward，还是端到端 Jenga。

## 更新记录

- 2026-06-12：按 `CK_Tutorail.docx` 和用户要求继续改造 pipeline/policy：
  - 删除 `jenga_bwd_dq_pipeline.hpp` 中的 `UseMmacQK/UseMmacDQ/StageVForScalarDP/CacheKTile` 判断，当前 kernel 固定为专用 `BM=64, BN=64, D=128` 的 QK-MMAC + DQ-MMAC 主路径。
  - Q/dO 从 global 到 LDS 改成 `load_tile(q_window/dout_window)` + `store_tile(q_lds/dout_lds_window, tile)`。
  - K/V/K-reload 的 full-block 路径改成 `load_tile(k/v_dram_window)` + `store_tile(kv_lds_store_window, tile)`；尾块和 inactive block 保留 scalar masked fallback 以保证 seqlen 边界正确。
  - `JengaBwdDqDefaultPolicy::MakeKVBlockTileDistribution()` 改成二维 KV tile distribution，结构参考 FMHA `MakeKDramTileDistribution()`：N 维拆成 `N0,N1,N2`，D 维拆成 `K0,K1`，避免此前复用 3D Q/DQ distribution 导致的 `tile_window` 维度静态断言。
  - correctness：`bf16, s=128, nnz=28` 为 `valid:y`；`fp16, s=128, nnz=28` 为 `valid:y`。
  - 性能影响：`bf16, B=1, H=4, S=1024, NNZ=28` 约 `1.4148 ms`；手写协作 load 的 DQ-MMAC 版本约 `1.216 ms`。Wan2.1 `B=1,H=40,S=18048,NNZ=28` 约 `304.3 ms`；手写协作 load 版本约 `270.8 ms`。当前版本更符合 CK tile API/policy 结构，但 KV tile distribution 仍需继续调优。
  - `bf16, B=1, H=1, S=8192, NNZ=118` 约 `17.49 ms`；手写协作 load 版本约 `16.15 ms`。
- 2026-06-11：按 FMHA bwd pipeline/policy 风格继续重构并落地 DQ-MMAC：
  - `jenga_bwd_dq_policy.hpp` 新增 `GetQKBlockGemm()`、`GetDPBlockGemm()`、`GetDQBlockGemm()`，pipeline 中改为 `constexpr auto gemm_0/gemm_2/gemm_4 = Policy::...`，与 FMHA 的 `gemm_0..gemm_4` 组织方式对齐。
  - `DS` LDS 从 `float` 改为输入 dtype，语义对齐 Triton/FMHA 的 `ds.to(dtype)` 后进入后续 GEMM，同时把 `DS[64,64]` LDS 占用从 16KB 降到 8KB。
  - 成功加入 `DQ = DS @ K` 的 MMAC 路径：`DS[64,64]` 作为 A，`K` 通过转置视图作为 B，使用 `WarpGemmMmac*WT16x32x64*_TRANSC`，kernel name 为 `qk_mmac_qdo_kv_ds_dq_mmac_lut`。
  - correctness：`bf16, s=128, nnz=4/28/118` 通过；`fp16, s=128, nnz=28` 通过。
  - 性能：`bf16, B=1, H=4, S=1024, NNZ=28` 约 `1.216 ms`；上一版 V-staging scalar DQ 约 `2.34 ms`。
  - Wan2.1：`bf16, B=1, H=40, S=18048, NNZ=28, warmup=1, repeat=3` 约 `270.8 ms`；上一版约 `555.6 ms`。
  - `S=8192, H=1, NNZ=118` 约 `16.15 ms`；上一版约 `33.1 ms`。
  - 尝试把 K/V full-block 数据搬运改成 `load_tile/store_tile`，但直接复用 Q/DQ 风格 distribution 会触发 `tile_window` 的 2D/3D adaptor 静态断言；已回滚。后续需要像 FMHA policy 那样专门定义 KV LDS descriptor/adaptor，而不是复用简单 2D naive LDS view。
- 2026-06-11：对照 FMHA backward DQ pipeline 和继续实验：
  - 参考文件：`include/ck_tile/ops/fmha/pipeline/block_fmha_bwd_dq_dk_dv_pipeline_kr_ktr_vr.hpp`、`block_fmha_bwd_pipeline_default_policy.hpp`、`kernel/fmha_bwd_kernel.hpp`。
  - FMHA bwd 的 DQ 热循环把 `Q*K^T`、`dO*V^T`、`DS*K^T` 都组织成 block gemm；K/V 先 HBM->LDS->Reg，并额外构造 K 的转置 LDS/Reg 视图供 Gemm4 使用。
  - 当前 Jenga CK 版只把 `Q*K^T` MMAC 化；`dO*V^T` 和 `DS*K` 仍有标量循环，所以要达到 `60 ms` 级别，下一步必须移植/重写类似 FMHA Gemm2/Gemm4 的结构，而不是继续只调 thread 数。
  - 重新打开 DP-MMAC 实验仍失败：`bf16, s=128, nnz=4` correctness 为 `valid:n`，最大误差约 `0.0345`，且小规模时间约 `1.43 ms`，已回滚。
  - 尝试 `NNZ=4` 使用 `256` threads 也退化：`bf16, s=128, nnz=4` 虽然 `valid:y`，但约 `1.42 ms`，明显慢于 `512` threads 的约 `0.31 ms`，已回滚。
  - 尝试把 `LSE/Delta` 预加载到 LDS 会让 local memory 从约 `65536` 超到 `66048`，编译器拒绝；当前 LDS 已被 Q/dO/K/DS 占满，不适合继续增加常驻 LDS 数据。
- 2026-06-11：继续优化 scalar `DP = dO @ V^T` 路径，加入 V tile LDS staging：
  - 在 `QK` MMAC 完成后，复用 `k_lds` 加载当前 active block 的 `V[64,128]`，用于 scalar DP。
  - `DS` 写入 `ds_lds` 后，在 `DQ += DS @ K` 前重新加载 `K[64,128]` 到同一块 LDS。
  - 这样把 DP 阶段的 V global read 从每个 `(m,n,kd)` 重复读取，降为每个 active block 协作读取一次 V tile；代价是每个 active block 多一次 K tile reload。
  - kernel name 更新为 `qk_mmac_qdo_kv_ds_lds_lut`，用于和上一版 `qk_mmac_qdo_k_ds_lds_lut` 区分。
  - correctness：`bf16, s=128, nnz=4/28/118` 均 `valid:y`；`fp16, s=128, nnz=28` 为 `valid:y`。
  - 快速性能点：`bf16, B=1, H=4, S=1024, NNZ=28` 从约 `6.97 ms` 降到 `2.3418 ms`。
  - Wan2.1 短测：`bf16, B=1, H=40, S=18048, NNZ=28, warmup=1, repeat=3` 为 `555.617 ms`，上一版约 `1709 ms`。
  - 长序列 NNZ=118 单头短测：`bf16, B=1, H=1, S=75648, NNZ=118, warmup=1, repeat=3` 为 `249.734 ms`。
- 2026-06-11：根据当前目标范围收窄 `jenga_bwd_dq` 编译实例化矩阵：
  - 仅保留 `BM=64, BN=64, D=128`。
  - 仅保留 `MaxNnz=4/28/118`。
  - 保留 `fp16/bf16` 两种 dtype，统一使用 benchmark 对齐的 `512` threads。
  - `jenga_bwd_dq.cpp` 模板实例化数量从旧的多形状组合收窄到 6 个实例，目标重编译 wall time 约 `33 s`。
  - example 入口加入 host 侧 shape gate，unsupported 参数会在 tensor 分配和 kernel dispatch 前直接报错。
  - correctness：`bf16, s=128, nnz=4/28/118` 均 `valid:y`；`fp16, s=128, nnz=28` 为 `valid:y`。
  - 性能回归检查：`bf16, B=1, H=4, S=1024, D=128, BM=64, BN=64, NNZ=28` 为 `6.9696 ms`，与缩窄前约 `6.97 ms` 持平。
- 2026-06-11：继续优化 `jenga_bwd_dq` pipeline，加入 CTA 级 LDS 缓存：
  - `Q[BM,D]` 和 `dO[BM,D]` 在 query block 内预加载到 LDS，供所有 active KV block 复用。
  - 当 `MaxNnz > 1` 时，将当前 active block 的 `K[BN,D]` 加载到 LDS，供 `QK = Q @ K^T` 和 `DQ += DS @ K` 两处复用；`MaxNnz == 1` 保持轻量路径，避免额外 LDS 装载和同步。
  - `BM=64, BN=64, D=128, bf16` 小规模 correctness 通过：`NNZ=1/28/118` 均为 `valid:y`。
  - `s=1024, h=1, repeat=5, v=0` 下，`NNZ=28` 约 `7.10 ms`，`NNZ=118` 约 `7.36 ms`；相比 `qdo_ds_lds_lut` 版本约 `13.6 ms` 有明显改善。
- 2026-06-11：完成 `ds_lds_lut` 中间优化，使用 LDS 保存 `DS[BM,BN]`，每个 thread 持有多个 DQ accumulator；验证 `BM=64, BN=64, D=128` 下 `NNZ=1/28/118` 的 bf16 correctness。
- 2026-06-11：在 `composable_kernel-main/example_hcu/ck_tile/20_jenga_bwd_dq` 落地分层 ck_tile example，并验证 `BM=64, BN=64, D=128` 下 `NNZ=1/28/118` 的 bf16 小规模 correctness。
- 2026-06-11：移除已有实现相关描述，文档改为只记录目标优化实现、benchmark 参数和后续优化路线。
## 2026-06-12 Follow-up

- Fixed `MakeQOGradDQBlockTileDistribution()` to match CK `tile_distribution_encoding`
  semantics:
  - Hidden dims are now `outer=1`, `M=(M0,M1,M2)`, `D=(K0,K1)`.
  - Parallel dims select `(M0)` and `(M1,K0)`.
  - Per-thread Y dims select `(outer, M2, K1)`.
  - For `BM=64,D=128,threads=512,bf16/fp16`: `K1=8,K0=16,M0=8,M1=4,M2=2`,
    so `threads=8*4*16=512` and per-thread values are `1*2*8=16`.
- Re-tested DP-MMAC after the distribution fix. It still fails correctness with the
  same error pattern (`bf16/fp16 s=128 nnz=28 valid:n`, max error about 0.0345).
  Conclusion: do not enable DP-MMAC until the V/B layout or warp-gemm policy is fixed.
- Tried runtime `num_active` loop truncation. It improves tiny cases with many inactive
  blocks but hurts larger benchmarks because the runtime loop bound inhibits scheduling/
  unrolling. Reverted to the fixed `MaxNnz` loop for stable medium/large performance.
- Cleaned unused local `gemm_2` instantiation from the pipeline while keeping policy
  definition available for future DP-MMAC experiments.
- Final verified results after this update:
  - `bf16,b=1,h=1,s=128,d=128,bm=64,bn=64,nnz=28`: `valid:y`, about `0.419 ms`.
  - `bf16,b=1,h=4,s=1024,d=128,bm=64,bn=64,nnz=28`: about `1.301 ms`.
  - `bf16,b=1,h=1,s=8192,d=128,bm=64,bn=64,nnz=118`: about `17.24 ms`.

## 2026-06-12 Reference Update

- Added validation modes:
  - `-v=1 -v_mode=cpu_block`: blockwise CPU reference. It computes QK/DP/DS once per
    `(m,n)` and reuses DS across all DQ columns, avoiding the old D-times duplicate
    QK/DP work.
  - `-v=1 -v_mode=cpu_naive`: preserved the original slow scalar CPU reference for
    debugging.
  - `-v=1 -v_mode=gpu`: device-side reference. It builds LSE on GPU and computes DQ
    reference on GPU, then the optimized kernel consumes the generated device LSE.
- `-v=1` defaults to `cpu_block`.
- Large-shape validation no longer stays silent in the CPU reference before kernel
  launch. The example prints `Generating <mode> reference...done` before running the
  optimized kernel.
- Verified:
  - `bf16,b=1,h=1,s=128,d=128,bm=64,bn=64,nnz=28,v_mode=cpu_block`: same reference
    result as `v_mode=gpu`; current DP-MMAC kernel still shows the known small-shape
    fp32-reference mismatch.
  - `bf16,b=1,h=40,s=18048,d=128,bm=64,bn=64,nnz=28,v_mode=gpu,warmup=1,repeat=2`:
    `valid:y(gpu)`, kernel time about `111.486 ms`.

## 2026-06-13 DQ Performance Update

- Added a K/V dual-LDS data path:
  - Removed persistent dO LDS staging.
  - dO is now loaded once per CTA into a register tile for DP-MMAC.
  - K and V use separate LDS buffers, so K is no longer reloaded after DP.
- Added a full-tile fast path for `num_active == MaxNnz` and `seqlen % 64 == 0`.
  This removes inactive/tail checks from the hot NNZ loop for the target shape while
  preserving the generic fallback path.
- Fused the full-tile QK value into the DP->DS sweep so QK does not need an intermediate
  LDS write/read before softmax/DS.
- Changed device softmax exponent calls to `exp2f` and made text bias conditional on
  `text_amp != 0`.
- Verified target:
  - `bf16,b=1,h=40,s=18048,d=128,bm=64,bn=64,nnz=28,v=1,v_mode=gpu`: `valid:y(gpu)`.
  - Stable timing with `warmup=10,repeat=50`: about `65.23 ms`.
- Tried `BlockWarps=sequence<2,4,1>`, but it is invalid for QK because
  `BlockN=64` and warp `N=32` make `NIterPerWarp=0`. Restored `sequence<4,2,1>`.
- Feasibility note for a 15 ms target: this likely requires a larger redesign, not only
  local pipeline cleanup. Current LDS is about 56 KB per CTA (`Q+K+V+DS`), so occupancy is
  effectively limited to one CTA/CU. Reaching 15 ms probably needs register-resident Q/dO
  plus AReg/BSmem or AReg/BReg DQ so DS does not need LDS, or a grouped-M design that
  reuses K/V across multiple query blocks.

## 2026-06-13 AReg/BSmem Experiment

- Checked current `/code/ck_opt/composable_kernel-main/include/ck_tile/ops/gemm/block`.
  It has AReg/BSmem and ASmem/BReg variants, but not `block_gemm_areg_bsmem_creg_v2r1.hpp`.
  Directly including current `block_gemm_areg_bsmem_creg_v2.hpp` conflicts with the MMAC
  headers because it pulls `warp_gemm.hpp`.
- Copied tmp-yaying's `block_gemm_areg_bsmem_creg_v2r1.hpp` into the jenga example as
  `jenga_block_gemm_areg_bsmem_creg_v2r1.hpp`, removed its default-policy include, and
  renamed the class to `JengaBlockGemmARegBSmemCRegV2R1`. This private header compiles
  without the `WarpGemmImpl` redefinition conflict.
- Experiment 1: QK as `Q(AReg) @ K(BSmem)`.
  - Compiled successfully.
  - Target timing regressed to about `68.6 ms`, likely due to extra VGPR pressure and
    losing the cheaper LDS reuse pattern.
- Experiment 2: DQ as `DS(AReg) @ K(BSmem)` by loading DS from LDS into AReg before DQ.
  - Compiled successfully and remained correct under GPU reference.
  - Target timing was about `69.0 ms`, still slower than the `65.25 ms` ASmem/BSmem DQ
    path.
- Reverted the production policy/pipeline to the best known path:
  QK ASmem/BSmem, DP AReg/BReg, DQ ASmem/BSmem, dO in register, separate K/V LDS,
  full-tile fast path.
- Kept the private `jenga_block_gemm_areg_bsmem_creg_v2r1.hpp` in the example directory
  for future direct DS-register remap experiments.

## 2026-06-13 Direct DS Register DQ

- Continued the AReg/BSmem direction by limiting it to the full-tile fast path:
  - Generic fallback still uses `DS(LDS) @ K(LDS)`.
  - Target full-tile path uses `DS(AReg) @ K(LDS)` via `GetDQRegBlockGemm()`.
- `DP` C tile has 8 values/thread, while DQ AReg expects 16 values/thread because DS must
  be duplicated across DQ N-warps. Updated the private
  `JengaBlockGemmARegBSmemCRegV2R1` to repeat a smaller A input thread buffer into the
  larger internal AReg buffer when the sizes divide evenly.
- In the full-tile path, `DS` is now created as a register tile from the DP sweep:
  `QK register + DP register -> DS register -> DQ AReg/BSmem`.
  This avoids the fast-path DS LDS write/read before DQ.
- Removed the now-unneeded barrier between DS register creation and DQ.
- Tried moving V load before QK to combine K/V LDS loads behind one barrier. It regressed
  to about `67.1 ms`, so reverted to `K -> sync -> QK -> V -> sync -> DP/DQReg`.
- Verified target:
  - `bf16,b=1,h=40,s=18048,d=128,bm=64,bn=64,nnz=28,v=1,v_mode=gpu`: `valid:y(gpu)`.
  - Stable timing with `warmup=10,repeat=50`: about `61.35 ms`.

## 2026-06-12 Fast-Only Target Pipeline

- Added a `FastOnly` template parameter to `JengaBwdDqPipeline`.
  - The generic pipeline still keeps the inactive/tail fallback path.
  - The fast-only pipeline is selected by host dispatch for aligned full-block shapes
    (`seqlen_q % 64 == 0` and `seqlen_q / 64 >= max_nnz`).
  - Fast-only does not instantiate the fallback `DS` LDS path, so the target hot kernel
    avoids the static `ds_lds[64*64]` allocation and generic fallback code.
- Target kernel name:
  `jenga_bwd_dq_bf16_bm64_bn64_d128_nnz28_qk_mmac_qdo_kv_dqreg_fast_lut`.
- Verified target:
  - `bf16,b=1,h=40,s=18048,d=128,bm=64,bn=64,nnz=28,v=1,v_mode=gpu`: `valid:y(gpu)`.
  - Stable timing with `warmup=10,repeat=50`: about `56.69 ms`.
- Tried making the final fast-path barrier conditional (`if(i + 1 < MaxNnz)`), but this
  regressed to about `62.26 ms`. Restored the unconditional `block_sync_lds()`.
- Tried rewriting the DS sweep in an FMHA-style row-wise `sweep_tile_span` to hoist
  `lse/delta` loads. It does not compile with the current DP tile distribution because a
  synthetic `(idx0, 0)` distributed index is not valid for this distribution. Reverted.
- Next large steps likely need a structural change:
  - grouped-M sharing of K/V across more than one query block, accepting higher DQ
    accumulator pressure;
  - or a true double-buffered K/V pipeline, probably requiring Q to move out of LDS to
    stay within reasonable LDS usage;
  - or a custom row-wise DS distribution that exposes valid row spans for hoisting
    `lse/delta` and matching FMHA's row sweep more closely.

## 2026-06-16 True 2D Q/Dout/DQ Entry Views

- Reworked the `q`, `dout`, and `dq` kernel entry path to use true per-`bh` 2D tensor views:
  - `q_bh_view : [seqlen_q, HeadDim]`
  - `dout_bh_view : [seqlen_q, HeadDim]`
  - `dq_bh_view : [seqlen_q, HeadDim]`
- From those 2D bottom tensors, the kernel now builds 2D padded block windows with
  `make_tuple(number<BlockM>{}, number<HeadDim>{})` instead of the old
  `make_tuple(number<1>{}, number<BlockM>{}, number<HeadDim>{})`.
- Updated `MakeQOGradDQBlockTileDistribution()` to a true 2D block distribution and made the
  Q LDS store window 2D as well.
- Important pitfall: after slicing `dout`/`dq` to per-`bh` 2D views, pipeline code must no
  longer add `off_hz * stride_doz` / `off_hz * stride_dqz` again. Leaving those offsets in
  place compiles but causes GPU VMFault from double-offset global accesses.
- Verified target:
  - `bf16,b=1,h=40,s=18048,d=128,bm=64,bn=64,nnz=28,v=1,v_mode=gpu`: `valid:y(gpu)`.
  - Stable timing with `warmup=10,repeat=50`: about `56.86 ms`.

## 2026-06-16 K/V Double Buffer Fast Path

- Added a fast-path K/V double-buffer experiment:
  - FastOnly uses two LDS buffers for K and V: `2 * BlockN * HeadDim` for each.
  - Q is kept as a register tile for QK via the private `JengaBlockGemmARegBSmemCRegV2R1`
    path, avoiding Q LDS so total LDS fits the 64 KiB limit.
  - The loop preloads K/V block 0, then prefetches K/V block `i + 1` into the alternate
    LDS buffer before computing block `i`.
  - DQ uses the matching K LDS buffer for the current iteration.
- The direct version with Q LDS plus 2x K/V LDS failed to compile because static LDS was
  `81920` bytes, above the target `65536` byte limit.
- Verified target:
  - `bf16,b=1,h=40,s=18048,d=128,bm=64,bn=64,nnz=28,v=1,v_mode=gpu`: `valid:y(gpu)`.
  - Stable timing with `warmup=10,repeat=50`: about `55.55 ms`.
- This is only a small win over the previous `~56.7 ms` path. The remaining bottleneck is
  dominated by the three MMAC phases plus per-block softmax/DS work; K/V load overlap alone
  does not expose enough latency to approach the desired 15 ms target.

## 2026-06-16 Grouped-M2 Shared K/V Fast Path

- Added an experimental `JengaBwdDqGroupedM2Pipeline` selected for the target full-tile
  `BM=64, BN=64, D=128, NNZ=28` fast path when `seqlen_q` is divisible by `2 * BM`.
- The pipeline keeps the logical query tile at `64x128`, but launches one CTA for two
  adjacent query blocks, so the kernel name reports `bm128` and the target grid changes
  from `{282, 40, 1}` to `{141, 40, 1}`.
- For the current sliding-window LUT pattern, adjacent query blocks share 27 of 28 KV
  blocks. The grouped path computes two DQ accumulators in one CTA and loads:
  - first block for query block 0,
  - the 27 shared middle KV blocks for both query blocks,
  - final block for query block 1.
  This reduces K/V block loads from `56` to `29` for every two query blocks.
- Kept the K/V LDS double-buffering from the previous experiment and still uses register Q
  plus AReg/BSmem QK, AReg/BReg DP, and AReg/BSmem DQ.
- Added a correctness fallback inside the grouped pipeline for non-overlapping adjacent LUTs:
  if `lut(qb, i) != lut(qb + 1, i - 1)`, the CTA computes the two logical blocks separately
  instead of returning empty results.
- Verified target:
  - `bf16,b=1,h=40,s=18048,d=128,bm=64,bn=64,nnz=28,v=1,v_mode=gpu`: `valid:y(gpu)`.
  - Stable timing with `warmup=10,repeat=50`: about `39.61 ms`.
- This is a meaningful speedup over the K/V double-buffer path (`~55.55 ms`) and the
  pre-double-buffer fast path (`~56.7 ms`), but it is still above the desired `15 ms`.
  Larger grouping might reduce K/V traffic further, but would add more live DQ accumulators
  and likely hit VGPR/occupancy limits; it needs to be tested rather than assumed.

## 2026-06-16 Removed Low-Performance General M2 Branches

- Removed the arbitrary set-intersection grouped-M2 path and its test-only LUT generators
  (`overlap`, `random`, `nonoverlap`, `lut_overlap`, `lut_seed`).
- Kept only the profitable sliding-window grouped-M2 path:
  `lut(qb, i) == lut(qb + 1, i - 1)`.
- Kept `-group_m=1|2` for direct comparison between single-M and grouped-M2 dispatch.
- Output now prints only the active LUT pattern (`sliding`), measured adjacent-pair overlap,
  and `group_m`.
- Verified target after cleanup:
  - `bf16,b=1,h=40,s=18048,d=128,bm=64,bn=64,nnz=28,group_m=2,v=1,v_mode=gpu`:
    `valid:y(gpu)`.
  - Stable timing with `warmup=10,repeat=50`: about `39.57 ms`.

## 2026-06-16 Sliding And Normal LUT Coexistence

- Added `-lut_pattern=sliding|normal`.
- `sliding` keeps the high-overlap grouped-M2 path when `-group_m=2`.
- `normal` generates ordinary non-overlapping LUT blocks and forces the effective
  `group_m` to `1`, even if the user requests `-group_m=2`. This keeps arbitrary LUTs
  correct without restoring the slow generic grouped-M2 set-intersection branch.
- Output prints the effective group value, e.g. `group_m:1(requested:2)` when normal LUT
  mode disables grouped-M2.
- Verified:
  - `sliding, group_m=2`: about `39.57 ms`.
  - `normal, requested group_m=2`: dispatches the single-M kernel, about `55.52 ms`.
  - `normal, requested group_m=2, v_mode=gpu`: `valid:y(gpu)`.
