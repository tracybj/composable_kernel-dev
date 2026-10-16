# v06_rebase_308t：B 公共 K 偏移的标量 rebase

把每 phase 共同的 K 字节偏移移入 B resource base，并相应缩小范围，减少逐 lane 的重复地址计算；这是最终统一入口保留的 raw fallback。

本目录由实测 SHA 对应的冻结源码提取，不是用最新源码关闭宏来近似重建。提交日期为归档日期，历史性能记录不改写成此次新跑的结果。

原候选：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`；原库 SHA256：`22ef3280c50201d0f66966e4fdfe6af75741f5d6dc4a2c46704bb4d7acd73025`。

入口：[src/rolephase.cpp](src/rolephase.cpp)；完整编译宏、源码 SHA、原始路径及验证信息：[manifest.json](manifest.json)。

| shape | M/N/K | 历史 ms | 历史 TFLOPS |
|---|---|---:|---:|
| fc2 | [18880, 5376, 7168] | 4.870980 | 298.726 |
| qkv | [18880, 10752, 5376] | 7.092379 | 307.743 |
| out_proj | [18880, 5376, 3584] | 2.548230 | 285.510 |
| fc1 | [18880, 14336, 5376] | 9.494265 | 306.519 |

原始证据：[results.json](../../records/results/stage_bp_scalar_b_base_independent/results.json)。若原文件较大，见 records/PRESERVATION_MANIFEST.json 对应的 .gz 文件。

从 04_gemm 目录执行：

```bash
python3 build.py --version v06_rebase_308t
HIP_VISIBLE_DEVICES=4 OMP_NUM_THREADS=4 python3 bench.py --versions v06_rebase_308t --tag new_v06_rebase_308t
```
