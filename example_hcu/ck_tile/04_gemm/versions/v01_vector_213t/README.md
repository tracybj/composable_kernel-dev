# v01_vector_213t：恢复连续 K 向量读取

在转置操作数组织后显式恢复连续 K 的向量化，避免尾部声明使 B 加载退化为标量；保留大 tile、LDS padding、CTA 分组与寄存器池设置。

本目录由实测 SHA 对应的冻结源码提取，不是用最新源码关闭宏来近似重建。提交日期为归档日期，历史性能记录不改写成此次新跑的结果。

原候选：`large_pad16_g8_trans_groupn_vecb_pad_m_reg768`；原库 SHA256：`cfddb1994814a1b26e7f203a379293f873b516a48718844aa53beb3d57d17d75`。

入口：[src/gemm.cpp](src/gemm.cpp)；完整编译宏、源码 SHA、原始路径及验证信息：[manifest.json](manifest.json)。

| shape | M/N/K | 历史 ms | 历史 TFLOPS |
|---|---|---:|---:|
| fc2 | [18880, 5376, 7168] | 7.058747 | 206.140 |
| qkv | [18880, 10752, 5376] | 10.265665 | 212.615 |
| out_proj | [18880, 5376, 3584] | 3.582574 | 203.079 |
| fc1 | [18880, 14336, 5376] | 13.695590 | 212.490 |

原始证据：[results.json](../../records/results/stage_af_native_m18880/results.json)。若原文件较大，见 records/PRESERVATION_MANIFEST.json 对应的 .gz 文件。

从 04_gemm 目录执行：

```bash
python3 build.py --version v01_vector_213t
HIP_VISIBLE_DEVICES=4 OMP_NUM_THREADS=4 python3 bench.py --versions v01_vector_213t --tag new_v01_vector_213t
```
