# v02_banked_240t：K32 双寄存器 bank 与完成等待

转为 8 compute + 4 producer waves 的私有流水：A 权重经过双槽 LDS，B 激活使用 K32 phase 的 VGPR 双 bank，并在消费前明确等待对应加载完成。

本目录由实测 SHA 对应的冻结源码提取，不是用最新源码关闭宏来近似重建。提交日期为归档日期，历史性能记录不改写成此次新跑的结果。

原候选：`rolephase_m1_bank_asmb_reg768`；原库 SHA256：`d89f4906cd47339c8ccb9d920f3d22c5d05c1bf8479e7d26bd8cffa59cb207fc`。

入口：[src/rolephase.cpp](src/rolephase.cpp)；完整编译宏、源码 SHA、原始路径及验证信息：[manifest.json](manifest.json)。

| shape | M/N/K | 历史 ms | 历史 TFLOPS |
|---|---|---:|---:|
| fc2 | [18880, 5376, 7168] | 6.086108 | 239.084 |
| qkv | [18880, 10752, 5376] | 9.101346 | 239.814 |
| out_proj | [18880, 5376, 3584] | 3.269438 | 222.529 |
| fc1 | [18880, 14336, 5376] | 12.135063 | 239.815 |

原始证据：[results.json](../../records/results/stage_ak_banked_wait_schedule/results.json)。若原文件较大，见 records/PRESERVATION_MANIFEST.json 对应的 .gz 文件。

从 04_gemm 目录执行：

```bash
python3 build.py --version v02_banked_240t
HIP_VISIBLE_DEVICES=4 OMP_NUM_THREADS=4 python3 bench.py --versions v02_banked_240t --tag new_v02_banked_240t
```
