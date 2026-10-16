# v05_static_301t：双 tile 展开与静态 wave 分组

显式展开两个 K64 tile，保留奇数尾；把早/晚两组 wave 的循环体编译期分开，在第一条 MMAC 后提升计算 priority，减少运行时分支及调度阻碍。

本目录由实测 SHA 对应的冻结源码提取，不是用最新源码关闭宏来近似重建。提交日期为归档日期，历史性能记录不改写成此次新跑的结果。

原候选：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768`；原库 SHA256：`b13a73b09afbc30fb75563ffa896981b0e260ab5478d40735708047257ae2c7f`。

入口：[src/rolephase.cpp](src/rolephase.cpp)；完整编译宏、源码 SHA、原始路径及验证信息：[manifest.json](manifest.json)。

| shape | M/N/K | 历史 ms | 历史 TFLOPS |
|---|---|---:|---:|
| fc2 | [18880, 5376, 7168] | 4.902676 | 296.795 |
| qkv | [18880, 10752, 5376] | 7.250154 | 301.046 |
| out_proj | [18880, 5376, 3584] | 2.570974 | 282.984 |
| fc1 | [18880, 14336, 5376] | 9.707593 | 299.783 |

原始证据：[results.json](../../records/results/stage_bm_static_wave_independent/results.json)。若原文件较大，见 records/PRESERVATION_MANIFEST.json 对应的 .gz 文件。

从 04_gemm 目录执行：

```bash
python3 build.py --version v05_static_301t
HIP_VISIBLE_DEVICES=4 OMP_NUM_THREADS=4 python3 bench.py --versions v05_static_301t --tag new_v05_static_301t
```
