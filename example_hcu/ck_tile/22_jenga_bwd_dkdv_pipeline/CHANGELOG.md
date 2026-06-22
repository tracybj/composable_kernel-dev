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
