# Jenga bwd dK/dV Pipeline Change Log

This file records functional changes, optimization notes, and validation results for
`example_hcu/ck_tile/22_jenga_bwd_dkdv_pipeline`.

## 2026-06-22 - `aec2a85` - Tune Jenga dK/dV pipeline for 64 query blocks

### Changes

- Changed the main CK Tile problem instance from `BlockM=128, BlockN=64` to
  `BlockM=64, BlockN=64` to match the Triton/Jenga `64x64` block-mask granularity.
- Updated default example arguments and traits:
  - `m0=64`
  - `jenga_bwd_dkdv_traits.block_m=64`
- Parameterized block-level GEMM tile shapes so QK/DP use `Problem::BlockM` on
  the query dimension and dV/dK use `Problem::BlockM` on the reduction dimension.
- Switched dV/dK block GEMM from `WT16x16x128` to a local `WT16x16x64` warp-GEMM
  alias, matching the new `BlockM=64` reduction length.
- Made vectorized Q/dO/K/V load loop counts and transposed M unroll counts depend
  on `BlockM`, `BlockN`, `HeadDim`, and `ThreadsPerBlock` instead of hard-coded
  values for `BlockM=128`.
- Removed the legacy compatibility path that mapped 64-granularity query-mask
  blocks into a 128-granularity query block.

### Validation

Standalone CK example correctness:

```bash
/workspace/composable_kernel-dev/build/bin/tile_example_jenga_bwd_dkdv \
  -b=1 -h=1 -n_q=128 -n_kv=128 -m0=64 -n0=64 \
  -text_blocks=0 -text_amp=0.0 -mask_radius=1 \
  -warmup=0 -repeat=1 -timer=cpu -v=1
```

Result:

```text
Validation: PASS
dK cosine=0.999995
dV cosine=0.999994
```

Additional small-shape validation:

```bash
/workspace/composable_kernel-dev/build/bin/tile_example_jenga_bwd_dkdv \
  -b=1 -h=2 -n_q=256 -n_kv=256 -m0=64 -n0=64 \
  -text_blocks=0 -text_amp=0.0 -mask_radius=1 \
  -warmup=0 -repeat=1 -timer=cpu -v=1
```

Result:

```text
Validation: PASS
dK cosine=0.999995
dV cosine=0.999995
```

Python binding benchmark after syncing this kernel into `hcu_extensions`:

```bash
cd /workspace/heyi_test/jenga_sla_test
python3 benchmark_kernels_ck.py --size wan2.1 --heads 40 --warmup 1 --repeat 3
```

Result:

| Metric | Triton | CK Tile | Speedup |
| :--- | ---: | ---: | ---: |
| Total backward | 365.33 ms | 279.59 ms | 1.31x |
| Isolated dK/dV | 268.09 ms | 184.24 ms | 1.46x |

Correctness:

| Tensor | cos_sim |
| :--- | ---: |
| dQ | 1.00000024 |
| dK | 0.99999279 |
| dV | 0.99999285 |

## 2026-06-22 - `88bae3d` - Hoist LSE and Delta global loads

### Changes

- Hoisted repeated LSE and Delta loads out of the innermost output sweep loops and
  cached them in per-thread registers for each active query tile.
- Kept K/V and Q/dO in their previous load path after register-preload experiments
  showed scratch spills and worse runtime.

### Validation

Wan2.1 standalone benchmark with random mask:

```bash
/workspace/composable_kernel-dev/build/bin/tile_example_jenga_bwd_dkdv \
  -b=1 -h=40 -n_q=18048 -n_kv=18048 -m0=128 -n0=64 \
  -text_blocks=0 -text_amp=0.0 \
  -mask_type=random -k_active=93 \
  -warmup=5 -repeat=20 -timer=gpu -v=0
```

Representative result:

| Version | Mechanism | VGPRs | Scratch bytes | Standalone time |
| :--- | :--- | ---: | ---: | ---: |
| Baseline | Original implementation | 238 | 0 | 377.56 ms |
| Optimized | LSE and Delta hoisting | 246 | 0 | 263.48 ms |

Conclusion: LSE/Delta hoisting provided about `1.43x` speedup without introducing
scratch spills.
