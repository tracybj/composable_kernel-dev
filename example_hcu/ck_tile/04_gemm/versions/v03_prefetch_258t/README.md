# v03_prefetch_258t：交错预取与有界输出

将未来 B 的加载插入当前 A 的 LDS 读取中段，去掉逐相位终末预取分支，同时保持最终排空与边界；输出改用有界 buffer store。

本目录由实测 SHA 对应的冻结源码提取，不是用最新源码关闭宏来近似重建。提交日期为归档日期，历史性能记录不改写成此次新跑的结果。

原候选：`rolephase_m1_bank_asmb_always_mid_bufferepi_reg768`；原库 SHA256：`a0e23fd8c4751c2f7843e782ab7e717dcd82c1a2c021973ad78e807745a2bc8b`。

入口：[src/rolephase.cpp](src/rolephase.cpp)；完整编译宏、源码 SHA、原始路径及验证信息：[manifest.json](manifest.json)。

| shape | M/N/K | 历史 ms | 历史 TFLOPS |
|---|---|---:|---:|
| fc2 | [18880, 5376, 7168] | 5.694236 | 255.537 |
| qkv | [18880, 10752, 5376] | 8.449162 | 258.325 |
| out_proj | [18880, 5376, 3584] | 2.982150 | 243.966 |
| fc1 | [18880, 14336, 5376] | 11.263696 | 258.368 |

原始证据：[results.json](../../records/results/stage_au_bounded_buffer_output/results.json)。若原文件较大，见 records/PRESERVATION_MANIFEST.json 对应的 .gz 文件。

从 04_gemm 目录执行：

```bash
python3 build.py --version v03_prefetch_258t
HIP_VISIBLE_DEVICES=4 OMP_NUM_THREADS=4 python3 bench.py --versions v03_prefetch_258t --tag new_v03_prefetch_258t
```
