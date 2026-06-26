# Jenga bwd dK/dV Pipeline 优化记录

本文档用于记录 `example_hcu/ck_tile/22_jenga_bwd_dkdv_pipeline` 的每次关键提交、优化思路、测试命令和测试结果，方便后续回溯。

## 2026-06-26 - q_t-only XOR LDS swizzle 实验（已提交）

### 实验内容

- 新增 `MakeXorSimpleLdsDescriptor` / `MakeXorTransposedLdsDescriptor`，用 `KPack=8` 做 LDS 物理 XOR remap。
- 最终只对 dK GEMM 的 `q_t` LDS tile 启用 XOR 物理布局与 transposed descriptor。
- `Q/K/V/p_t/ds_t/do_t` 保持线性 LDS 布局，避免把复杂 XOR 寻址扩散到 softmax/dS 或 DP/DV 热路径。
- 对比尝试过的完整 selective XOR（`do_t + q_t`）版本：正确但性能退化明显，因此没有采用 `do_t` XOR。

### 验证结果

在 `ck_yyc` 中绑定 GPU 4：

```bash
HSA_VISIBLE_DEVICES=4 /workspace/composable_kernel-dev-github/build/bin/tile_example_jenga_bwd_dkdv \
  -b=1 -h=2 -n_q=256 -n_kv=256 -d=128 -m0=64 -n0=64 \
  -mask_type=random -k_active=10 \
  -warmup=0 -repeat=1 -timer=gpu -v=1
```

结果正确：

```text
Average time: 0.676164 ms
dK cosine=0.999995
dV cosine=0.999995
Validation: PASS
```

大 case 性能：

```bash
HSA_VISIBLE_DEVICES=4 /workspace/composable_kernel-dev-github/build/bin/tile_example_jenga_bwd_dkdv \
  -b=1 -h=40 -n_q=18048 -n_kv=18048 -d=128 -m0=64 -n0=64 \
  -mask_type=random -k_active=93 \
  -warmup=5 -repeat=10 -timer=gpu -v=0
```

```text
Average time: 92.2857 ms
```

完整 selective XOR（`do_t + q_t`）对照：

```text
Correctness: PASS
Average time: 206.347 ms
```

### 结论

- `q_t-only XOR` 数值正确，但相对 GPU 4 baseline 约 `86.9 ms` 仍偏慢。
- `do_t` 不适合启用 XOR：同一 LDS tile 同时服务 DV 的转置读和 DP 的普通读，启用 XOR 后两个 GEMM descriptor 都变复杂，代价远大于 bank conflict 收益。
- 本提交保留 q_t-only XOR 版本，用于后续结合 profiler 继续分析 bank conflict、VGPR/scratch 与 scheduler stall 的权衡。

## 2026-06-26 - BlockN=32 split-KV full-tile 实验（未采纳）

### 实验内容

- 新增 `BlockN=32` 的 full-tile specialization，尝试把原始 `64x64` KV block 拆成两个 `64x32` half block。
- `start_n` 按 32 递增，reverse LUT / text block 判断仍映射回原始 64-block mask。
- 目标是降低最大 LDS footprint，缓解 profiler 中观察到的 `thread group limit LDS = 1` 与 `Insufficient CU LDS` 问题。

### 验证结果

在 `ck_yyc` 中绑定 GPU 4：

```bash
HSA_VISIBLE_DEVICES=4 /workspace/composable_kernel-dev-github/build/bin/tile_example_jenga_bwd_dkdv \
  -b=1 -h=2 -n_q=256 -n_kv=256 -d=128 -m0=64 -n0=64 \
  -mask_type=random -k_active=10 \
  -warmup=0 -repeat=1 -timer=gpu -v=1
```

结果正确：

```text
Average time: 0.635202 ms
dK cosine=0.999995
dV cosine=0.999995
Validation: PASS
```

大 case 性能：

```bash
HSA_VISIBLE_DEVICES=4 /workspace/composable_kernel-dev-github/build/bin/tile_example_jenga_bwd_dkdv \
  -b=1 -h=40 -n_q=18048 -n_kv=18048 -d=128 -m0=64 -n0=64 \
  -mask_type=random -k_active=93 \
  -warmup=5 -repeat=10 -timer=gpu -v=0
```

```text
Average time: 127.331 ms
```

### 结论

- 该实验数值正确，但性能明显慢于 `BlockN=64` baseline（GPU 4 上约 `86.9 ms`）。
- 虽然 `BlockN=32` 理论上能降低 Q/K LDS footprint，但原始 mask 分块是 `64x64`，拆成两个 half block 会让 workgroup 数量翻倍，并重复执行 Q/dO 加载、softmax/dS sweep 与部分 LDS/GEMM 流程。
- 重复计算和调度开销超过了 LDS footprint 降低的潜在收益，因此该实验已回退，不采纳。

## 2026-06-26 - LDS descriptor swizzle 与 K/V register staging 实验（未采纳）

### 测试口径

在 `ck_yyc` 中绑定 GPU 4，性能数据统一使用历史对齐参数：

```bash
HSA_VISIBLE_DEVICES=4 /workspace/composable_kernel-dev-github/build/bin/tile_example_jenga_bwd_dkdv \
  -b=1 -h=40 -n_q=18048 -n_kv=18048 -d=128 -m0=64 -n0=64 \
  -mask_type=random -k_active=93 \
  -warmup=5 -repeat=10 -timer=gpu -v=0
```

当前干净 baseline：

```text
Average time: 86.8992 ms
```

### LDS descriptor XOR swizzle 实验

- 将 `MakeSimpleLdsDescriptor` 改为基于 `KPack=8` 的 XOR transform descriptor。
- `MakeTransposedLdsDescriptor` 复用 swizzled base descriptor 后再交换维度。
- 目标是降低 profiler 中观察到的 LDS bank conflict。

结果：

```text
Correctness: FAIL
Average time: 254.771 ms
dK cosine=0.00171289
dV cosine=0.138595
```

结论：该 descriptor 破坏了现有 LDS 写入布局与 GEMM 读取描述之间的一致性，数值错误且性能严重退化，已回退。

### Full-tile K/V register staging 实验

- 在 `FullTile=true` 路径中，将当前 `kv_block` 的 K/V tile 用 `int4` 预先加载到寄存器数组。
- active q-block loop 内复用寄存器值写入 LDS，减少 K/V global load 次数。

结果：

```text
Correctness: PASS
Average time: 88.5636 ms
```

结论：虽然正确，但比 baseline `86.8992 ms` 慢。推测增加的 VGPR/live range 与调度压力抵消了 global load 减少收益，已回退。

### Full-tile K-only register staging 实验

- 只对 K tile 做 `int4` register staging，V 保持 loop 内直接 global load。
- 目标是降低 staging 寄存器压力，验证 K 复用是否单独有收益。

结果：

```text
Correctness: PASS
Average time: 88.0981 ms
```

结论：K-only 仍慢于 baseline，说明当前瓶颈不优先在 K/V 重复 global load；该实验已回退。

### 当前判断

- 继续保留现有 full-tile specialization baseline。
- 短期不再优先做 K/V register staging。
- LDS conflict 优化不能只替换 descriptor，需要同步设计 LDS 写入布局与 GEMM 读取 descriptor。

## 2026-06-26 - LDS Padding 性能优化扫参实验（未采纳）

### 实验内容

- 为了消除 Jenga 反向传播 dK/dV 算子在 LDS 转置读取时的 32 路 Bank 冲突，在 `do_t` 和 `q_t` 缓冲区引入了 LDS Padding 机制。
- 通过在 `LdsStorage` 中将行宽度步长从 128 填充到 `128 + Pad`，对 `Pad` 尺寸进行了多配置扫参（Sweep）测试：`Pad = 0, 2, 4, 6, 8, 12, 16, 24, 32`。

### 验证结果

正确性通过，但大 case 运行时间（Wan2.1 配置下）全部出现明显性能退化：

- `Pad = 0 (基线)` -> **87.77 ms**
- `Pad = 2` -> **131.18 ms** (非 8 字节对齐，指令退化)
- `Pad = 4` -> **135.32 ms** (非 8 字节对齐，指令退化)
- `Pad = 6` -> **131.18 ms** (非 8 字节对齐，指令退化)
- `Pad = 8` -> **113.18 ms** (非 2 的幂次 Stride)
- `Pad = 12` -> **135.32 ms** (非 8 字节对齐，指令退化)
- `Pad = 16` -> **113.17 ms** (非 2 的幂次 Stride)
- `Pad = 24` -> **113.19 ms** (非 2 的幂次 Stride)
- `Pad = 32` -> **118.21 ms** (非 2 的幂次 Stride)

### 结论与根因分析

- **不应采纳 LDS Padding**。本算子中保持 128 元素（2 的幂次 Stride）不加 padding 性能最佳。
- **根因一**：当 Pad 不是 8 的倍数（非 16 字节对齐）时，编译器无法生成 128 位向量化 LDS 读写指令（`ds_write_b128` / `ds_read_b128`），退化为多条小宽度标量访存指令，导致耗时暴增（~135 ms）。
- **根因二**：当 Pad 即使是 8 的倍数（满足 16 字节对齐）时，由于 Stride 变为非 2 的幂次（如 136, 144 等），编译器无法再使用极高效的二进制左移运算（如 `<< 7`）来自动算地址，且破坏了 `ck_tile` 内部默认的高效 LDS Swizzle（地址交错映射）对称性，导致整体运行效率明显慢于 baseline（113 ms vs 87.7 ms）。
- **后续动作**：该实验已完全回退，代码已无缝还原至 `Pad = 0` 的最优基线状态。

## 2026-06-25 - Full-tile specialization，编译期裁剪 boundary/scalar 分支

### 改动内容

- 为 `JengaBwdDkdvPipeline` 增加 `FullTile` 模板参数。
- Host 侧在满足以下条件时选择 full-tile kernel：
  - `B == 1`
  - `N_Q == N_KV`
  - `N_Q` / `N_KV` 都按 `64` 对齐
  - `D == 128`
  - Q/K/V/dO/dK/dV 的 stride 和指针对齐满足现有 `int4` 128bit fast path 条件
- `FullTile=true` 时，编译期裁剪掉：
  - Q/K/V/dO/Q reload 的 boundary scalar fallback
  - softmax/dS sweep 中的 `row_valid` / `n < seqlen` masking
  - dK/dV writeback 的 boundary scalar fallback
- 保留默认 `FullTile=false` kernel，非完整 tile 或未满足 128bit fast path 条件时仍走原通用路径。
- 注意：host 侧无法直接检查 device `seqlens_ptr` 是否全为满长。当前 full-tile 选择只用于本 example 的 `B=1` 满长 case；若后续接入通用 varlen API，需要额外传入 host-side full-seqlen flag 或单独预检查，不能把 fallback 通用 pipeline 编进 full-tile 热 kernel，否则会明显拉高资源压力并退化性能。

### 正确性验证

在 `ck_yyc` 中绑定 GPU 7：

```bash
HSA_VISIBLE_DEVICES=7 /workspace/composable_kernel-dev-github/build/bin/tile_example_jenga_bwd_dkdv \
  -b=1 -h=2 -n_q=256 -n_kv=256 -d=128 -m0=64 -n0=64 \
  -mask_type=random -k_active=10 \
  -warmup=0 -repeat=1 -timer=gpu -v=1
```

结果：

```text
full_tile=1
Average time: 0.558882 ms
dK max_diff=7.62939e-06 mean_diff=6.9046e-07 cosine=0.999995
dV max_diff=0.00012207 mean_diff=2.54243e-05 cosine=0.999995
Validation: PASS
```

### 性能结果

在 `ck_yyc` 中绑定 GPU 7：

```bash
HSA_VISIBLE_DEVICES=7 /workspace/composable_kernel-dev-github/build/bin/tile_example_jenga_bwd_dkdv \
  -b=1 -h=40 -n_q=18048 -n_kv=18048 -d=128 -m0=64 -n0=64 \
  -mask_type=random -k_active=93 \
  -warmup=5 -repeat=10 -timer=gpu -v=0
```

三次结果：

```text
Average time: 89.7658 ms
Average time: 89.7464 ms
Average time: 89.7387 ms
```

fallback 小样本验证：

```bash
HSA_VISIBLE_DEVICES=7 /workspace/composable_kernel-dev-github/build/bin/tile_example_jenga_bwd_dkdv \
  -b=2 -h=1 -n_q=256 -n_kv=256 -d=128 -m0=64 -n0=64 \
  -mask_type=random -k_active=10 \
  -warmup=0 -repeat=1 -timer=gpu -v=1
```

结果：

```text
full_tile=0
Average time: 0.711992 ms
Validation: PASS
```

### 结论

该优化正确性通过，性能相对上一版约 `99.8 ms` 提升到约 `89.75 ms`，保留。

## 2026-06-25 - P/dS row-major LDS + transposed descriptor 实验（未采纳）

### 实验内容

- 将 P 和 dS 的 LDS 写入从 `[N,M]` 转置布局改成 row-major `[M,N]`：
  - 原写入：`buf[n_rel * BlockM + m_rel]`
  - 实验写入：`buf[m_rel * BlockN + n_rel]`
- dV/dK GEMM 输入通过 `MakeTransposedLdsDescriptor<BlockN, BlockM>()` 继续呈现为 `[N,M]`。
- 目标是改善 P/dS 生成阶段的 LDS 写入连续性，不增加 LDS footprint，不改变 GEMM shape。

### 验证结果

正确性通过：

```text
Average time: 0.576471 ms
dK cosine=0.999995
dV cosine=0.999995
Validation: PASS
```

大 case 性能退化：

```text
Average time: 117.496 ms
```

### 结论

该实验虽然数值正确，但性能从约 `99.8 ms` 退化到 `117.5 ms`。推测原 `[N,M]` LDS 布局虽然写入时更像转置散写，但更匹配后续 dV/dK GEMM 的 LDS 读取模式。因此该实验已回退，不采纳。

## 2026-06-25 - dK AReg 与 WT16x32 warp tile 探索（未采纳）

### 背景

继续评估 `22_jenga_bwd_dkdv_pipeline` 的后续优化方向，重点关注：

- 是否能把当前 `ASmemBSmem` 的某个 operand 改成 register tile，减少 LDS traffic。
- dV/dK 是否能从 `WT16x16x64` 改成更宽的 `WT16x32x64`，让 N/D 方向覆盖更贴近 128bit vector load/store 和 64B cacheline。
- 保持现有 Q/K/V/dO/dQ reload/DK/DV write 的 `int4` 128bit 连续访问快路径，不引入破坏 16B 对齐的 LDS/global access pattern。

### dK AReg 实验结论

- 尝试只把 dK 的 A operand `dS^T` 从 LDS 改成 register tile，保留 Q 作为 BSmem。
- 当前 `dp_out/qk_out` 的分布是 `[M,N]`，而 dK A 需要 `[N,M]`。这不是同一线程内简单换索引，原 LDS 路径实际承担了跨 lane/跨 warp 的转置重排。
- 直接使用 `transpose_tile2d(ds_t_reg, ds_mn_reg)` 编译失败，原因是输入/输出 distributed tensor 的 replication 和 hidden shape 不匹配。

编译失败特征：

```text
transpose_tile2d: rs_lengths / hs_lengthss mismatch
pack expansion contains parameter packs with different lengths
```

结论：dK AReg 方向不能只改 block gemm typedef。若继续推进，需要写专用 register shuffle/preshuffle，把 `[M,N]` 的 dS 正确重排到 dK A 的 `[N,M]` AReg distribution；这会增加 VGPR 和实现风险。

### WT16x32x64 普通 MMAC 实验结论

- 尝试把 dV/dK 从局部 `WT16x16x64_MR1NR1MI1NI1` 改成 `WT16x32x64_MR1NR1MI1NI2`。
- 编译在 `load_tile` 的 LDS 读取处失败。

编译失败特征：

```text
static assertion failed due to requirement 'd % load_store_traits::ScalarPerVector == 0'
```

结论：普通 NI2 形状会让部分 B operand LDS load 落到不满足 vector load 对齐的位置，和当前芯片 128bit 加载/64B cacheline 的优化目标冲突。因此不能只通过 policy 切换到 `WT16x32x64_NI2`。

### WT16x32x64 TransC 实验结论

- 尝试把 dV/dK 改成 CK 现有 alias `WarpGemmMmacBF16BF16F32_WT16x32x64_MR1NR2MI1NI1_TRANSC`。
- 编译在 `MmacBlockGemmASmemBSmemCRegV1::MakeOuputLayout` 处失败，出现负长度 distributed tensor。

编译失败特征：

```text
thread_buffer<float, -8>
thread_buffer<float, -2>
static_for<0, -1, 1>
```

结论：该 TransC alias 不能直接和当前 `ASmemBSmem` block gemm/output layout helper 组合。若继续推进，需要配套专用 block gemm、BReg/preshuffle，或重写 output layout。

### 当前建议

短期保留当前 `WT16x16x64 + ASmemBSmem` 方案。下一轮更值得尝试的方向：

- 优先保持现有 `int4` 128bit global load/store 路径，减少 boundary 分支进入概率或扩大 fast path 覆盖。
- 针对 dK AReg 只做专用 shuffle 原型，先用小 kernel 验证 distribution 正确性和 VGPR 增量，再接入主 pipeline。
- 如果继续探索 `WT16x32`，应同时设计 B operand 的 register/preshuffle 路径，而不是只替换 warp tile。

验证状态：失败实验均已回退，当前 baseline 可重新编译通过：

```bash
docker exec ck_yyc cmake --build /workspace/composable_kernel-dev-github/build --target tile_example_jenga_bwd_dkdv -j 4
```

## 2026-06-25 - 复用 Softmax 寄存器结果与 dO LDS，减少重复计算/加载

### 改动内容

- 将 QK sweep 中第一次计算得到的 Softmax `P` 原地写回 `qk_out` 寄存器 tile。
- dS sweep 直接复用 `qk_out` 中的 `P`，不再第二次执行 `exp2(qk - lse)`。
- 调整 `LdsStorage` 中 dV/DP 阶段的 LDS overlay：
  - `do_t` 固定放在前半段 LDS，dV 阶段加载一次后保留到 DP 阶段复用。
  - `p_t` 和 `v` 在后半段 LDS 中串行复用；dV GEMM 结束后，`p_t` 不再需要，可被 `v` 覆盖。
- DP 阶段不再从 global memory 第二次加载 `dO`，只加载当前 `kv_block` 的 `V` tile。
- 最大 LDS footprint 没有增加，仍保持在原有 Q/K 或 dO/V overlay 的 32KB 级别。

### 正确性验证

在 `ck_yyc` 中绑定 GPU 7：

```bash
HSA_VISIBLE_DEVICES=7 /workspace/composable_kernel-dev/build/bin/tile_example_jenga_bwd_dkdv \
  -b=1 -h=2 -n_q=256 -n_kv=256 -d=128 -m0=64 -n0=64 \
  -mask_type=random -k_active=10 \
  -warmup=0 -repeat=1 -timer=gpu -v=1
```

结果：

```text
Average time: 0.557444 ms
dK max_diff=7.62939e-06 mean_diff=6.9046e-07 cosine=0.999995
dV max_diff=0.00012207 mean_diff=2.54243e-05 cosine=0.999995
Validation: PASS
```

### 性能结果

在 `ck_yyc` 中绑定 GPU 7：

```bash
HSA_VISIBLE_DEVICES=7 /workspace/composable_kernel-dev/build/bin/tile_example_jenga_bwd_dkdv \
  -b=1 -h=40 -n_q=18048 -n_kv=18048 -d=128 -m0=64 -n0=64 \
  -mask_type=random -k_active=93 \
  -warmup=5 -repeat=10 -timer=gpu -v=0
```

两次结果：

```text
Average time: 99.8423 ms
Average time: 99.8171 ms
Average time: 99.782 ms
```

资源元数据：

```text
vgpr_count: 256
sgpr_count: 104
vgpr_spill_count: 14
sgpr_spill_count: 0
amdhsa_private_segment_fixed_size: 60
```

### 结论

该优化正确性通过，性能相对 `64x64 向量化无冲突写入` 版本记录的 `105.03 ms` 提升到约 `99.8 ms`。代价是出现轻微 VGPR spill（14），但没有 SGPR spill，且实测性能稳定，因此保留。

## 2026-06-22 - 移除 LUT 驱动循环中的冗余 block mask 检查

### 改动内容

- 移除了 `P` 和 `ds` sweep 循环中逐元素访问 `block_mask_ptr` 的逻辑。
- 当前 kernel 的内层循环由 reverse LUT 驱动，`qb` 能出现在某个 `kv_block` 的 reverse LUT 中，已经说明 `(qb, kv_block)` 是 active pair。因此在每个元素上重新检查 block mask 是冗余的。
- 保留 partial tile 边界判断：
  - full tile: `valid = true`
  - boundary tile: `valid = row_valid && n < seqlen`

### 正确性验证
小规模 random mask：

```bash
/workspace/composable_kernel-dev/build/bin/tile_example_jenga_bwd_dkdv \
  -b=1 -h=1 -n_q=256 -n_kv=256 -m0=64 -n0=64 \
  -text_blocks=0 -text_amp=0.0 -mask_type=random -k_active=2 \
  -warmup=0 -repeat=1 -timer=cpu -v=1
```

结果：

```text
Validation: PASS
dK cosine=0.999995
dV cosine=0.999995
```

### Python binding benchmark 结果

将当前 example kernel 同步到 `heyi_test/jenga_sla_test/hcu_extensions` 的既有 `ck_tile_jenga` binding 后测试：

```bash
cd /workspace/heyi_test/jenga_sla_test
python3 benchmark_kernels_ck.py --size short --heads 1 --warmup 1 --repeat 2
python3 benchmark_kernels_ck.py --size wan2.1 --heads 40 --warmup 3 --repeat 10
```

小规模 sanity 结果：

| 项目 | Triton | CK Tile | 加速比 |
| :--- | ---: | ---: | ---: |
| Total backward | 1.01 ms | 0.91 ms | 1.11x |
| Isolated dK/dV | 0.24 ms | 0.37 ms | 0.65x |

Wan2.1 / 18K / H=40 结果：

| 项目 | Triton | CK Tile | 加速比 |
| :--- | ---: | ---: | ---: |
| Total backward | 365.50 ms | 244.35 ms | 1.50x |
| Isolated dK/dV | 268.14 ms | 148.33 ms | 1.81x |

正确性：

| Tensor | max_diff | mean_diff | cos_sim |
| :--- | ---: | ---: | ---: |
| dQ | 0.000000e+00 | 0.000000e+00 | 1.00000024 |
| dK | 3.906250e-03 | 1.238873e-04 | 0.99999279 |
| dV | 2.929688e-03 | 1.237167e-04 | 0.99999285 |

### 结论

该优化正确性通过，性能收益明显，保留。

## 2026-06-22 - P 复用实验：避免第二次 exp2（未采纳）

### 实验内容

- 尝试将第一次计算得到的 `P^T` 常驻 LDS，在后续计算 `ds = P * (dp - delta)` 时复用，避免第二次 `exp2(qk - lse)`。
- 为了让 `P^T` 不被后续 Q/dO/V 临时 LDS 覆盖，需要在 `LdsStorage` 中额外保留一块 `BlockN * BlockM` 的 bf16 LDS，约 8KB。

### 正确性验证

该实验版本正确性可以通过：

```text
Validation: PASS
dK cosine=0.999994
dV cosine=0.999994 ~ 0.999995
```

### 性能结果

Wan2.1 / 18K standalone random mask：

```bash
/workspace/composable_kernel-dev/build/bin/tile_example_jenga_bwd_dkdv \
  -b=1 -h=40 -n_q=18048 -n_kv=18048 -m0=64 -n0=64 \
  -text_blocks=0 -text_amp=0.0 \
  -mask_type=random -k_active=28 \
  -warmup=5 -repeat=20 -timer=gpu -v=0
```

| 版本 | standalone 耗时 |
| :--- | ---: |
| 移除冗余 block mask 检查后 | 50.25 ms |
| P 常驻 LDS 复用实验 | 106.93 ms |

### 结论

该实验虽然减少了一次 `exp2`，但额外 LDS 占用和访问路径导致性能明显退化，因此不采纳。当前代码已回退该实验，仅保留本文档记录，避免后续重复尝试同一路径。

## 2026-06-22 - K/V 常驻 LDS 实验（编译失败，未采纳）

### 实验内容

- 当前每个 CTA 固定处理一个 `kv_block`，因此 K/V tile 在整个 active query block 循环中保持不变。
- 尝试在 active loop 外一次性将 K 和 V tile 加载到 LDS，循环内复用，避免每个 active query block 都重复从 global memory 加载 K/V。

### 编译结果

该实验无法通过编译：

```text
local memory (73728) exceeds limit (65536)
```

原因是常驻 K/V 需要额外 LDS：

```text
K tile: 64 * 128 * bf16 = 16 KB
V tile: 64 * 128 * bf16 = 16 KB
```

叠加现有 Q、P/dO、ds/q、acc staging 等 LDS 需求后，总 LDS 达到约 72 KB，超过当前 gfx936 kernel 的 64 KB local memory 限制。

### 结论

该路径不可行，代码已回退。后续如果要减少 K/V 重复加载，需要考虑更细粒度的方式，例如：

- 只常驻 K 或只常驻 V，而不是同时常驻；
- 缩小某些临时 LDS buffer；
- 改变计算顺序，避免同时保留多个大 tile；
- 或通过更激进的 split pipeline 重新设计 LDS 生命周期。

## 2026-06-22 - 仅 K 常驻 LDS 实验（未采纳）

### 实验内容

- 在 K/V 同时常驻 LDS 超出 64KB 限制后，进一步尝试只将 K tile 常驻 LDS。
- K tile 在每个 CTA 中随 `kv_block` 固定，理论上可以在 active query block 循环外加载一次，然后供所有 QK GEMM 复用。
- V tile 仍保持原逻辑，在 DP 阶段按 active query block 加载。

### 正确性验证

synthetic mask 和 random mask 小规模测试均通过：

```text
Validation: PASS
dK cosine=0.999995
dV cosine=0.999994 ~ 0.999995
```

### 性能结果

Wan2.1 / 18K standalone random mask：

```bash
/workspace/composable_kernel-dev/build/bin/tile_example_jenga_bwd_dkdv \
  -b=1 -h=40 -n_q=18048 -n_kv=18048 -m0=64 -n0=64 \
  -text_blocks=0 -text_amp=0.0 \
  -mask_type=random -k_active=28 \
  -warmup=5 -repeat=20 -timer=gpu -v=0
```

| 版本 | standalone 耗时 |
| :--- | ---: |
| 移除冗余 block mask 检查后 | 50.25 ms |
| 仅 K 常驻 LDS | 70.89 ms |

### 结论

该实验正确性通过，但性能明显退化，因此不采纳。原因推测是额外常驻 K tile 增加 LDS 压力，降低 occupancy 或 LDS 调度效率，收益无法抵消资源压力。代码已回退。

## 2026-06-22 - full tile / boundary tile 分支拆分实验（未采纳）

### 实验内容

- 在移除冗余 block mask 检查后，内层 sweep 中仍保留了：

```cpp
valid = !is_boundary || (row_valid && n < seqlen)
```

- 尝试将 full tile 和 boundary tile 拆成两个独立分支：
  - full tile 路径直接计算，不再生成 `valid` 条件；
  - boundary tile 路径保留 `row_valid && n < seqlen`。

### 正确性验证

synthetic mask 和 random mask 小规模测试均通过：

```text
Validation: PASS
dK cosine=0.999995
dV cosine=0.999994 ~ 0.999995
```

### 性能结果

Wan2.1 / 18K standalone random mask 两次测试波动较大：

| 版本 | standalone 耗时 |
| :--- | ---: |
| 移除冗余 block mask 检查后 | 50.25 ms |
| full/boundary 拆分，第 1 次 | 48.73 ms |
| full/boundary 拆分，第 2 次 | 94.70 ms |

### 结论

该实验正确性通过，但性能不稳定，有明显退化风险，因此不采纳。代码已回退。后续如果要优化 full tile fast path，需要结合编译资源、寄存器使用、occupancy 和汇编差异进一步分析，而不是简单复制两套 sweep 代码。

## 2026-06-22 - `aec2a85` - 将 query tile 调整到 BlockM=64

### 改动内容

- 将主实例从 `BlockM=128, BlockN=64` 调整为 `BlockM=64, BlockN=64`，对齐 Triton/Jenga 的 `64x64` block mask 粒度。
- 更新默认参数和 traits：
  - `m0=64`
  - `jenga_bwd_dkdv_traits.block_m=64`
- block-level GEMM shape 改为跟随 `Problem::BlockM`：
  - QK / DP 的 query 维使用 `Problem::BlockM`
  - dV / dK 的 reduction 维使用 `Problem::BlockM`
- dV / dK block GEMM 使用局部 `WT16x16x64` warp-GEMM alias，对齐新的 `BlockM=64` reduction 长度。
- Q/dO/K/V 的 vectorized load loop 和转置 M 维 unroll 从固定值改为依赖 `BlockM`、`BlockN`、`HeadDim` 和 `ThreadsPerBlock`。
- 移除旧版将 64 粒度 query mask 映射到 128 粒度 query block 的兼容路径。

### 正确性验证

```bash
/workspace/composable_kernel-dev/build/bin/tile_example_jenga_bwd_dkdv \
  -b=1 -h=1 -n_q=128 -n_kv=128 -m0=64 -n0=64 \
  -text_blocks=0 -text_amp=0.0 -mask_radius=1 \
  -warmup=0 -repeat=1 -timer=cpu -v=1
```

结果：

```text
Validation: PASS
dK cosine=0.999995
dV cosine=0.999994
```

额外小规模验证：

```bash
/workspace/composable_kernel-dev/build/bin/tile_example_jenga_bwd_dkdv \
  -b=1 -h=2 -n_q=256 -n_kv=256 -m0=64 -n0=64 \
  -text_blocks=0 -text_amp=0.0 -mask_radius=1 \
  -warmup=0 -repeat=1 -timer=cpu -v=1
```

结果：

```text
Validation: PASS
dK cosine=0.999995
dV cosine=0.999995
```

### Python binding benchmark 结果

同步该 kernel 到 `hcu_extensions` 后测试：

```bash
cd /workspace/heyi_test/jenga_sla_test
python3 benchmark_kernels_ck.py --size wan2.1 --heads 40 --warmup 1 --repeat 3
```

| 项目 | Triton | CK Tile | 加速比 |
| :--- | ---: | ---: | ---: |
| Total backward | 365.33 ms | 279.59 ms | 1.31x |
| Isolated dK/dV | 268.09 ms | 184.24 ms | 1.46x |

正确性：

| Tensor | cos_sim |
| :--- | ---: |
| dQ | 1.00000024 |
| dK | 0.99999279 |
| dV | 0.99999285 |

## 2026-06-22 - `88bae3d` - Hoist LSE 和 Delta 全局加载

### 改动内容

- 将内层 sweep 循环中重复读取的 `LSE` 和 `Delta` 提升到外层 active query tile 循环，并缓存到线程寄存器中。
- K/V 预加载、Q/dO staging 等更激进的寄存器缓存实验会导致 scratch spill 或性能退化，因此没有采纳。

### 性能结果

Wan2.1 standalone random mask：

```bash
/workspace/composable_kernel-dev/build/bin/tile_example_jenga_bwd_dkdv \
  -b=1 -h=40 -n_q=18048 -n_kv=18048 -m0=128 -n0=64 \
  -text_blocks=0 -text_amp=0.0 \
  -mask_type=random -k_active=93 \
  -warmup=5 -repeat=20 -timer=gpu -v=0
```

| 版本 | 机制 | VGPR | Scratch bytes | standalone 耗时 |
| :--- | :--- | ---: | ---: | ---: |
| Baseline | 原始实现 | 238 | 0 | 377.56 ms |
| 优化版 | LSE/Delta hoisting | 246 | 0 | 263.48 ms |

结论：LSE/Delta hoisting 没有引入 scratch spill，并带来约 `1.43x` 加速。

## 2026-06-23 - LDS 写入 Bank 冲突优化与全向量化写入

### 改动内容

- 优化了 Phase 2 (`do_t`) 和 Phase 4 (`q_t`) 写入 LDS 的布局，将原本以 16-bit 为单位的转置写入，改为了以原生行优先（Row-Major，`BlockM x HeadDim`）布局的写入。
- 实现了使用 128位向量指令 `int4` (`ds_write_b128`) 将 `do_t` 和 `q_t` 一次性向量化写入 LDS，完全消除了 LDS 写入时的 Bank 冲突。
- 引入了转置视图描述符生成器 `MakeTransposedLdsDescriptor<K, M>()`。利用 `ck_tile` 编译期的维度转置映射，使得 `dv_gemm` 和 `dk_gemm` 在读取 LDS 时自动按转置坐标进行，完美兼容原先的 GEMM 模块。

### 正确性验证

在 `-m0=64 -n0=64` 配置下：
```bash
/workspace/composable_kernel-dev/build/bin/tile_example_jenga_bwd_dkdv \
  -b=1 -h=2 -n_q=256 -n_kv=256 -d=128 -m0=64 -n0=64 -v=1 -mask_type=random -k_active=10
```

结果：
```text
Validation: PASS
dK cosine=0.999995
dV cosine=0.999995
```

### 性能结果

针对 Wan2.1 尺度，`k_active=93` (实际迭代93次) 的性能对比：

```bash
/workspace/composable_kernel-dev/build/bin/tile_example_jenga_bwd_dkdv \
  -b=1 -h=40 -n_q=18048 -n_kv=18048 -d=128 -m0=64 -n0=64 -v=0 -warmup=5 -repeat=10 -mask_type=random -k_active=93
```

| 版本 | 机制 | VGPR | SGPR | Scratch bytes | standalone 耗时 | 相比 baseline 加速比 |
| :--- | :--- | :---: | :---: | :---: | :---: | :---: |
| 64x64 baseline (LSE Hoisted) | 原版转置写入布局 | 194 | 86 | 0 | 160.91 ms | 1.00x |
| **64x64 向量化无冲突写入** | **Row-Major 写入 + 编译期转置视图** | **194** | **86** | **0** | **105.03 ms** | **1.53x (提升 35%)** |

### 结论

该优化大幅提升了 LDS 写入带宽并彻底解决了 Bank 冲突，在不增加额外 VGPR 寄存器负担（0 字节 Scratch 溢出）的前提下，实现 35% 性能提速，决定采纳并合入。
