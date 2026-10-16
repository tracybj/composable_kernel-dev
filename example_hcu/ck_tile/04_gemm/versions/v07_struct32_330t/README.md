# v07_struct32_330t：K32 Cache Swizzle 与精确 shape 分发

结构化 B descriptor 使用 K32 公共 rebase、row index 与 descriptor bit62 CacheSwizzle；FC2/out 叠加 producer 固定槽 pair 与 A soffset，QKV/FC1 采用 plain structured 路径，其余尺寸保留 raw fallback。

本目录由实测 SHA 对应的冻结源码提取，不是用最新源码关闭宏来近似重建。提交日期为归档日期，历史性能记录不改写成此次新跑的结果。

原候选：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32dispatch_reg768`；原库 SHA256：`cbd4638fde724142b41314e7d2578e79fbbdc0a6bcd43d6e154716e768f5f6b9`。

入口：[src/struct32_dispatch.cpp](src/struct32_dispatch.cpp)；完整编译宏、源码 SHA、原始路径及验证信息：[manifest.json](manifest.json)。

| shape | M/N/K | 历史 ms | 历史 TFLOPS |
|---|---|---:|---:|
| fc2 | [18880, 5376, 7168] | 4.420853 | 329.142 |
| qkv | [18880, 10752, 5376] | 6.550875 | 333.182 |
| out_proj | [18880, 5376, 3584] | 2.206406 | 329.742 |
| fc1 | [18880, 14336, 5376] | 8.775322 | 331.632 |

原始证据：[results.json](../../records/results/stage_dg_struct32_dispatch_independent_20261014/results.json)。若原文件较大，见 records/PRESERVATION_MANIFEST.json 对应的 .gz 文件。

从 04_gemm 目录执行：

```bash
python3 build.py --version v07_struct32_330t
HIP_VISIBLE_DEVICES=4 OMP_NUM_THREADS=4 python3 bench.py --versions v07_struct32_330t --tag new_v07_struct32_330t
```

4096³ 的统一入口仍走 raw fallback。可用 `build.py --version v07_struct32_330t --variant producer` 构建已验证的直接优化分支，再用 `bench.py --versions v07_struct32_330t__producer --m 4096 --shapes square4096 --tag new_square4096` 测试。
