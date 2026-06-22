# 22 Jenga bwd dK/dV pipeline notes

## Goal

Implement a Jenga sparse-attention backward dK/dV example under this directory using ck_tile-style
kernel/policy/pipeline files. The target math matches Triton `_sla_bwd_dkv_kernel`:

- `qk = dot(Q_m, K_n) * sm_scale * log2(e) + text_bias`
- `P = exp2(qk - lse_log2)`
- `dV += P^T @ dO`
- `dK += (P * (dO @ V^T - delta))^T @ Q * sm_scale`

## Current verified baseline

The current checked-in `jenga_bwd_dkdv_pipeline_pipeline.hpp` is a correctness baseline. It is split
into local `policy/kernel/pipeline` files under `22_jenga_bwd_dkdv_pipeline`, but its core math is
still scalar per output element. It is useful as the reference while replacing stages with ck_tile
BlockGemm/WarpGemm.

Verified in container `ck_yyc`:

```bash
docker exec ck_yyc cmake --build /tmp/yyc_ck_jenga_build_22 --target tile_example_jenga_bwd_dkdv_pipeline -j 4
docker exec ck_yyc /tmp/yyc_ck_jenga_build_22/bin/tile_example_jenga_bwd_dkdv_pipeline -b=1 -h=1 -n_q=128 -n_kv=128 -text_blocks=0 -text_amp=0.0 -mask_radius=1 -warmup=0 -repeat=1 -timer=cpu -v=1
docker exec ck_yyc /tmp/yyc_ck_jenga_build_22/bin/tile_example_jenga_bwd_dkdv_pipeline -b=1 -h=2 -n_q=256 -n_kv=384 -text_blocks=1 -text_amp=1.0 -mask_radius=1 -warmup=0 -repeat=1 -timer=cpu -v=1
```

Both cases passed with cosine 1 for dK and dV.

## Attempts already made

1. Directly called public `BlockFmhaBwdDQDKDVPipelineKRKTRVR` from the 22 kernel.
   - 128x128 internal tile exceeded LDS limit: about 164 KB > 64 KB.
   - 32x32 internal tile compiled but produced all-zero dK/dV.

2. Added a local sparse mask to force the public FMHA bwd pipeline to process only the current RLUT
   query block.
   - 128x128 still exceeded LDS.
   - 32x32 compiled but still produced all-zero dK/dV.

3. Tried swapping the FMHA bwd policy to MMAC-style warp/block gemm directly.
   - `warp_mmac_gemm.hpp` and `warp_gemm.hpp` define conflicting `WarpGemmImpl` templates when
     included together.
   - FMHA bwd default policy descriptor helpers expect MFMA warp attribute fields such as
     `kCM0PerLane/kCM1PerLane`, which MMAC attributes do not provide.

Conclusion: do not treat the public FMHA bwd pipeline as a black box for Jenga. The next route is a
Jenga-local block pipeline that reuses lower-level BlockGemm/WarpGemm pieces stage by stage.

4. Started a 22-local `JengaBwdDkdvBlockPipeline` using `BlockGemmARegBRegCRegV1` and CK's
   `WarpGemmMfmaDispatcher`.
   - The first compile exposed that the public dtype aliases live in the global namespace, not under
     `jenga_bwd_dkdv_pipeline`.
   - The next compile showed that default `PTFromGemm0CToGemm1A`/`SGradTFromGemm2CToGemm3A`
     cannot assign 32x32 C layouts directly into the transposed A layouts. Switched Gemm1/Gemm3
     warp tiles to 16x16x16 so the helper takes its explicit transpose path.
   - The first runtime validation produced all-zero dK/dV. One immediate mismatch was fixed next:
     the local BlockGemm shape uses `sequence<2,2,1>` = 4 warps = 128 threads, while the old kernel
     policy still launched 256 threads.
   - That diagnosis was incomplete on HCU/AMD because `get_warp_size()` is 64. Four warps require
     256 threads; launching 128 threads only wrote half of the distributed C tile. Keep
     `kBlockSize=256` for the current 4-warp BlockGemm shape.
   - QK direct-write debugging with MFMA stayed all-zero even after switching to CK `load_tile`.
     This matches the earlier public FMHA 32x32 all-zero result on gfx936/HCU, so the MFMA path is
     not a useful HCU tensorcore route here.

5. Rebuilt `jenga_bwd_dkdv_block_pipeline.hpp` as a local MMAC QK experiment.
   - Removed the dependency on `BlockFmhaBwdPipelineDefaultPolicy` to avoid `warp_gemm.hpp` /
     `warp_mmac_gemm.hpp` `WarpGemmImpl` redefinition conflicts.
   - Used `MmacBlockGemmASmemBSmemCRegV1` with `WarpGemmMmacBF16BF16F32_WT16x32x128_MR1NR1MI1NI2`
     and manual LDS staging for Q/K.
   - It compiles in `ck_yyc` and QK produces non-zero output on HCU, proving the MMAC tensorcore
     path is active.
   - Directly storing the MMAC C tile only covers 2048/16384 elements, even after
   `MakeOuputLayout`. This mirrors 03_gemm's need for a cshuffle epilogue/output shuffle before
   writing a full matrix tile. This was an intermediate state; the current 22 path is enabled and
   uses the NI1 MMAC shape plus a 22-local LDS materialization helper.

## Next replacement plan

Replace the scalar baseline in small, testable steps:

1. `QK = Q @ K^T` per sparse `(q_block, kv_block)` tile.
2. `DP = dO @ V^T` for the same tile.
3. Build `P` and `dS = P * (DP - delta)` elementwise.
4. `dV += P^T @ dO`.
5. `dK += dS^T @ Q * sm_scale`.

Immediate next step for the MMAC route:

1. Add a local 22 cshuffle/output epilogue for the MMAC QK C tile, or reuse CK's cshuffle epilogue
   pattern from `03_gemm`, so QK can be materialized as a complete 128x128 tile.
2. Once QK direct-write covers 16384 elements and matches a CPU QK probe, add DP as another MMAC
   stage.
3. Only then reconnect P/dS and final dV/dK accumulation.

## Current continuation

Continuing from the MMAC QK direct-write probe. Before adding cshuffle, first check the
`BlockWarps` setting: with wave64 and `kBlockSize=256`, the CTA has 4 waves. The previous
`sequence<8,4,1>` describes 32 logical warp slots, which is impossible for the launched block and
explains why only 2048 elements were materialized. Try a 4-wave layout first, then add a cshuffle
epilogue only if the output layout still does not cover the whole 128x128 tile.

Result: switching QK `BlockWarps` to `sequence<4,1,1>` compiled and QK direct-write covered
`16384/16384` elements. The earlier 2048/16384 coverage was caused by the impossible logical warp
layout, not by a missing cshuffle as the first-order issue. Keep this 4-wave shape while connecting
the next MMAC stages.

## 2026-06-15 continuation: 22-local output shuffle direction

User requested keeping the current 21 implementation intact and continuing 22 with a local
cshuffle/output epilogue direction. The next experiment is therefore:

1. Keep all new code under `example_hcu/ck_tile/22_jenga_bwd_dkdv_pipeline`.
2. Add a 22-local helper that materializes an MMAC `C` tile into row-major LDS before later stages
   consume it.
3. Use a 64 KB local LDS union so the same storage can be viewed as:
   - `Q[128,128] + K[128,128]` during QK,
   - `QK[128,128]` as float during output shuffle,
   - `P^T[128,128] + dO^T[128,128]` during dV.
4. First validate QK -> row-major LDS -> P^T -> dV. Only after dV is correct, add DP, dS, and dK.

Result:

- Added `StoreMmacOutputTileToLdsRowMajor` and a 64 KB LDS union in
  `jenga_bwd_dkdv_block_pipeline.hpp`.
- Routing QK through row-major LDS did not change dV cosine: still about `0.521244`.
- Routing dV output through the same LDS materialization also did not change cosine.
- Switching the MMAC warp from `WT16x32x128_MR1NR1MI1NI2` to
  `WT16x16x128_MR1NR1MI1NI1` moved the minimal dV case to `cosine=0.999971`.

Conclusion: the previous low dV cosine was caused by the NI2 output/interleave layout. Keep NI1 as
the correctness path while implementing DP, dS, and dK. A later optimization can restore NI2 by
copying the special N-interleave output reorder hinted in
`mmac_block_gemm_asmem_bsmem_creg_v1.hpp`.

Follow-up:

- Added DP and dK MMAC stages in the 22 block pipeline:
  - `DP = dO @ V^T`,
  - `dS^T = (P * (DP - delta))^T`,
  - `dK = dS^T @ Q * sm_scale`.
- First version recomputes QK scalar inside the dS stage to keep LDS use within 64 KB while
  validating the new MMAC stages.
- Minimal single-block validation passed in `ck_yyc`:

```bash
docker exec ck_yyc cmake --build /tmp/yyc_ck_jenga_build_22 --target tile_example_jenga_bwd_dkdv_pipeline -j 4
docker exec ck_yyc /tmp/yyc_ck_jenga_build_22/bin/tile_example_jenga_bwd_dkdv_pipeline -b=1 -h=1 -n_q=128 -n_kv=128 -text_blocks=0 -text_amp=0.0 -mask_radius=1 -warmup=0 -repeat=1 -timer=cpu -v=1
```

Observed:

- `dK cosine=0.999995`
- `dV cosine=0.999971`
- `Validation: PASS`

This first full-MMAC path initially handled only `rlut_ptr[0]`; the reverse-LUT accumulation section
below records the fix.

## 2026-06-15 reverse-LUT accumulation

Updated the block pipeline to match the Triton reverse-LUT loop:

- `dv_acc` and `dk_acc` are now MMAC C register tiles initialized once per KV block.
- The kernel iterates over `active_i in [0, max_nnz_r)`, skips entries beyond `num_active`, loads
  each active query block, and accumulates partial dV/dK into the register tiles.
- dV and dK are materialized to LDS and stored once after the active query-block loop.

Validation in `ck_yyc`:

```bash
docker exec ck_yyc /tmp/yyc_ck_jenga_build_22/bin/tile_example_jenga_bwd_dkdv_pipeline -b=1 -h=1 -n_q=128 -n_kv=128 -text_blocks=0 -text_amp=0.0 -mask_radius=1 -warmup=0 -repeat=1 -timer=cpu -v=1
docker exec ck_yyc /tmp/yyc_ck_jenga_build_22/bin/tile_example_jenga_bwd_dkdv_pipeline -b=1 -h=2 -n_q=256 -n_kv=384 -text_blocks=1 -text_amp=1.0 -mask_radius=1 -warmup=0 -repeat=1 -timer=cpu -v=1
docker exec ck_yyc /tmp/yyc_ck_jenga_build_22/bin/tile_example_jenga_bwd_dkdv_pipeline -b=1 -h=4 -n_q=512 -n_kv=640 -text_blocks=1 -text_amp=0.5 -mask_radius=2 -warmup=0 -repeat=1 -timer=cpu -v=1
```

All three passed. Representative cosines:

- 128/128: `dK=0.999995`, `dV=0.999971`
- 256/384 text case: `dK=0.999995`, `dV=0.999974`
- 512/640: `dK=0.999995`, `dV=0.99997`

Remaining cleanup/performance work:

- Replace scalar QK recompute in the dS stage with reuse of the already materialized QK/P path.
- Move K/V loading outside the active query-block loop.
- Restore the faster NI2 warp tile by implementing its special output reorder.

## Current optimization baseline to measure

Status before performance tuning:

- Correctness path is enabled through `kUseExperimentalBlockPipeline=true`.
- The implementation is a real 22-local ck_tile/MMAC pipeline for QK, dV, DP, and dK.
- It uses the NI1 MMAC warp tile (`WT16x16x128_MR1NR1MI1NI1`) because NI2 currently needs a
  special output reorder.
- Reverse-LUT accumulation is implemented in registers for dK/dV and stores once per KV block.
- The dS stage intentionally recomputes QK scalar-side for correctness isolation; this is the
  biggest known performance issue.

Planned optimization order:

1. Run GPU-timer performance baselines for representative sizes.
2. Reuse the already materialized QK/P data in dS instead of scalar QK recompute.
3. Hoist K/V staging out of the active query-block loop where possible.
4. Implement NI2 output reorder and switch back to the wider `WT16x32x128` warp tile.
5. Re-measure against the same baseline commands after each change.

Measured in `ck_yyc` with GPU timer, `warmup=5`, `repeat=20`, `v=0`:

```bash
docker exec ck_yyc /tmp/yyc_ck_jenga_build_22/bin/tile_example_jenga_bwd_dkdv_pipeline -b=1 -h=1 -n_q=128 -n_kv=128 -text_blocks=0 -text_amp=0.0 -mask_radius=1 -warmup=5 -repeat=20 -timer=gpu -v=0
docker exec ck_yyc /tmp/yyc_ck_jenga_build_22/bin/tile_example_jenga_bwd_dkdv_pipeline -b=1 -h=2 -n_q=256 -n_kv=384 -text_blocks=1 -text_amp=1.0 -mask_radius=1 -warmup=5 -repeat=20 -timer=gpu -v=0
docker exec ck_yyc /tmp/yyc_ck_jenga_build_22/bin/tile_example_jenga_bwd_dkdv_pipeline -b=1 -h=4 -n_q=512 -n_kv=640 -text_blocks=1 -text_amp=0.5 -mask_radius=2 -warmup=5 -repeat=20 -timer=gpu -v=0
docker exec ck_yyc /tmp/yyc_ck_jenga_build_22/bin/tile_example_jenga_bwd_dkdv_pipeline -b=2 -h=4 -n_q=512 -n_kv=640 -text_blocks=1 -text_amp=0.5 -mask_radius=2 -warmup=5 -repeat=20 -timer=gpu -v=0
docker exec ck_yyc /tmp/yyc_ck_jenga_build_22/bin/tile_example_jenga_bwd_dkdv_pipeline -b=1 -h=8 -n_q=1024 -n_kv=1024 -text_blocks=1 -text_amp=0.5 -mask_radius=2 -warmup=5 -repeat=20 -timer=gpu -v=0
```

Results:

| B | H | N_Q | N_KV | mask_radius | text_blocks | max_nnz_r | grid | avg gpu time |
|---|---|-----|------|-------------|-------------|-----------|------|--------------|
| 1 | 1 | 128 | 128 | 1 | 0 | 1 | `{1,1,1}` | 1.87666 ms |
| 1 | 2 | 256 | 384 | 1 | 1 | 2 | `{3,2,1}` | 3.73317 ms |
| 1 | 4 | 512 | 640 | 2 | 1 | 4 | `{5,4,1}` | 8.02026 ms |
| 2 | 4 | 512 | 640 | 2 | 1 | 4 | `{5,8,1}` | 8.20304 ms |
| 1 | 8 | 1024 | 1024 | 2 | 1 | 8 | `{8,8,1}` | 15.8671 ms |

Initial read:

- Runtime scales strongly with `max_nnz_r`, as expected, because each active query block currently
  repeats QK, dV, DP, dK and also scalar-recomputes QK for dS.
- Increasing independent `(B,H)` work improves occupancy; `B=2,H=4` at 512/640 is only slightly
  slower than `B=1,H=4`.
- The first optimization should be removing scalar QK recompute in dS, then hoisting K/V staging.

After each step, keep the existing CPU validation path and run the two commands listed above.
