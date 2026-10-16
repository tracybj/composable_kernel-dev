# 可选三路分派：FC2原producer优化 + out_proj CacheSwizzle（DB/DC，2026-10-14）

## 精确分派，不全局开启cache

- N5376/K3584：使用独立CZ/DA复测有效的K64结构化B、bit62 ON、row-major immediate0/64、phase0 VM0路径。
- N5376/K7168：保持此前两轮已验证的producer fixed pair + A scalar offset。
- 其余原支持尺寸：使用原22ef K32 raw路径。

主入口cache_dispatch.cpp，build.py构建`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_cachedispatch_reg768`；仅04_gemm私有实现，不修改H3/03_gemm/include，不替换默认库。

分派库正常及C偏移2-byte各126组/378次逐位审计，合计252组/756次候选launch通过；包括四真实shape与小M尾、odd K、输入/guard/finite/FP32误差及重复确定性。独立原版和FP32额外launch不计入。

## 两轮独立四shape计时

HCU4 / gfx936 / DTK25.10.0-0，M18880，BF16输入输出/FP32累加，5预热、6轮轮换×10 HIP events、中位数，profiler关闭。time reduction正数为更快。

| round | shape | BASE TFLOPS | old dispatch TFLOPS | new dispatch TFLOPS | new ms | time reduction vs BASE | vs old dispatch | wins/6 vs old |
|---|---|---:|---:|---:|---:|---:|---:|
| DB | fc2 | 298.684 | 300.314 | 300.315 | 4.845213 | +0.543% | +0.000% | 3 |
| DB | qkv | 307.461 | 307.989 | 307.870 | 7.089451 | +0.133% | -0.038% | 1 |
| DB | out_proj | 285.747 | 286.595 | 292.003 | 2.491566 | +2.142% | +1.852% | 6 |
| DB | fc1 | 306.151 | 306.186 | 306.195 | 9.504329 | +0.014% | +0.003% | 4 |
| DC | fc2 | 299.132 | 300.483 | 300.339 | 4.844822 | +0.402% | -0.048% | 3 |
| DC | qkv | 307.736 | 307.768 | 307.469 | 7.098699 | -0.087% | -0.097% | 0 |
| DC | out_proj | 285.783 | 286.868 | 291.856 | 2.492814 | +2.081% | +1.709% | 6 |
| DC | fc1 | 306.507 | 306.429 | 306.436 | 9.496841 | -0.023% | +0.002% | 4 |

## 编译资源

| kernel | VGPR | SGPR | LDS | scratch | spill |
|---|---:|---:|---:|---:|---:|
| _ZN11h3_original18h3_original_kernelEPKtS1_Ptiii | 225 | 31 | 65536 | 0 | 0 |
| _ZN11h3_producer18h3_producer_kernelEPKtS1_Ptiii | 225 | 31 | 65536 | 0 | 0 |
| _ZN8h3_cache15h3_cache_kernelEPKtS1_Ptiii | 240 | 32 | 65536 | 0 | 0 |

## 判定

满足两轮out_proj超过旧分派0.3%、每轮至少5/6轮更快，且其他shape相对旧分派不回退超过0.3%；登记为新的已验证可选分派库，原默认22ef和旧可选d2e保留。
库SHA256：`de1cd4cc831fae0153f5cb0ef8dee163582501db7d83e776e192a33de05a5484`。

这是BF16独立算子收益，不是INT8或H3 E2E收益，不能套用attention/GEMM历史占比外推。当前尚未达到四shape约330T目标。

## Counter解释

本轮out_proj的bit62 ON相对相同结构化OFF，VM issue等待及barrier等待聚合下降，但VM完成等待聚合没有同步下降；相对原K32路径也有不同变化。不能把增益简单归结为VM完成等待或HBM带宽，完整配对指标见results/profile_struct64_out_proj_20261014/ANALYSIS.md。counter不是耗时占比，性能结论来自未插桩两轮计时。

重放：bash run_stage_cz.sh → bash run_stage_da.sh → bash run_stage_db.sh → bash run_stage_dc.sh（拒绝覆盖已有证据）；CPU报告python3 analyze_struct64.py、analyze_struct64_repeat.py、analyze_cache_dispatch.py。未commit/push。

