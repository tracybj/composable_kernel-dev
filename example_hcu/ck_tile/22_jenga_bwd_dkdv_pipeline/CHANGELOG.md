# Jenga bwd dK/dV Pipeline 优化记录

本文档用于记录 `example_hcu/ck_tile/22_jenga_bwd_dkdv_pipeline` 的每次关键提交、优化思路、测试命令和测试结果，方便后续回溯。

## 2026-06-22 - 移除 LUT 驱动循环中的冗余 block mask 检查

### 改动内容

- 移除了 `P` 和 `ds` sweep 循环中逐元素访问 `block_mask_ptr` 的逻辑。
- 当前 kernel 的内层循环由 reverse LUT 驱动，`qb` 能出现在某个 `kv_block` 的 reverse LUT 中，已经说明 `(qb, kv_block)` 是 active pair。因此在每个元素上重新检查 block mask 是冗余的。
- 保留 partial tile 边界判断：
  - full tile: `valid = true`
  - boundary tile: `valid = row_valid && n < seqlen`

### 正确性验证

小规模 synthetic mask：

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
| BlockM=64 baseline | 62.59 ms |
| 移除冗余 block mask 检查 | 50.25 ms |

收益：约 `1.25x`。

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
