# v04_cadence_274t：RNE 与 producer/consumer 同步节奏

先保留 CK BF16 舍入及 NaN payload 语义地减少输出转换控制流，再调整两组 compute waves 的屏障位置，让权重槽在最后 LDS 读取完成后即可供 producer 复用。

本目录由实测 SHA 对应的冻结源码提取，不是用最新源码关闭宏来近似重建。提交日期为归档日期，历史性能记录不改写成此次新跑的结果。

原候选：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_reg768`；原库 SHA256：`b44b845565a273e2986622fca58f599703511796779171aa0f0f7716ac16ac47`。

入口：[src/rolephase.cpp](src/rolephase.cpp)；完整编译宏、源码 SHA、原始路径及验证信息：[manifest.json](manifest.json)。

| shape | M/N/K | 历史 ms | 历史 TFLOPS |
|---|---|---:|---:|
| fc2 | [18880, 5376, 7168] | 5.359892 | 271.477 |
| qkv | [18880, 10752, 5376] | 7.960746 | 274.174 |
| out_proj | [18880, 5376, 3584] | 2.811238 | 258.798 |
| fc1 | [18880, 14336, 5376] | 10.653848 | 273.157 |

原始证据：[results.json](../../records/results/stage_bb_cadence_independent/results.json)。若原文件较大，见 records/PRESERVATION_MANIFEST.json 对应的 .gz 文件。

从 04_gemm 目录执行：

```bash
python3 build.py --version v04_cadence_274t
HIP_VISIBLE_DEVICES=4 OMP_NUM_THREADS=4 python3 bench.py --versions v04_cadence_274t --tag new_v04_cadence_274t
```
