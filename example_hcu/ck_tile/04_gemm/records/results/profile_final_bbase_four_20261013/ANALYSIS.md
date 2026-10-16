# 最终B rebase版本：四类GEMM counter对照

库SHA：`22ef3280c50201d0f66966e4fdfe6af75741f5d6dc4a2c46704bb4d7acd73025`。每种实现各3次，物理HCU4。当前版本确认为最终B rebase库；没有沿用BM中间版本指标。

| shape | 指标 | CK均值 | rocBLAS均值 | CK/参考 |
|---|---|---:|---:|---:|
| fc2 | SQ_WAIT_CNT_VM | 194,687,269 | 97,423,256 | 1.998 |
| fc2 | SQ_WAIT_CNT_LGKM | 88,714,084 | 157,875,464 | 0.562 |
| fc2 | SQ_WAIT_BARRIER | 366,409,957 | 472,800,338 | 0.775 |
| fc2 | SQ_WAIT_INST_VMEM | 124,124,141 | 169,871,819 | 0.731 |
| fc2 | SQ_WAIT_INST_LDS | 82,047,202 | 56,150,474 | 1.461 |
| qkv | SQ_WAIT_CNT_VM | 281,464,794 | 128,480,333 | 2.191 |
| qkv | SQ_WAIT_CNT_LGKM | 136,790,442 | 233,052,282 | 0.587 |
| qkv | SQ_WAIT_BARRIER | 525,323,025 | 764,109,419 | 0.687 |
| qkv | SQ_WAIT_INST_VMEM | 161,040,969 | 271,803,046 | 0.592 |
| qkv | SQ_WAIT_INST_LDS | 124,057,770 | 95,811,261 | 1.295 |
| out_proj | SQ_WAIT_CNT_VM | 268,813,846 | 45,438,549 | 5.916 |
| out_proj | SQ_WAIT_CNT_LGKM | 105,533,859 | 81,374,928 | 1.297 |
| out_proj | SQ_WAIT_BARRIER | 189,115,108 | 210,413,874 | 0.899 |
| out_proj | SQ_WAIT_INST_VMEM | 176,388,695 | 89,841,225 | 1.963 |
| out_proj | SQ_WAIT_INST_LDS | 40,851,895 | 32,701,841 | 1.249 |
| fc1 | SQ_WAIT_CNT_VM | 379,976,136 | 159,905,754 | 2.376 |
| fc1 | SQ_WAIT_CNT_LGKM | 181,710,790 | 311,801,274 | 0.583 |
| fc1 | SQ_WAIT_BARRIER | 703,174,435 | 1,001,739,090 | 0.702 |
| fc1 | SQ_WAIT_INST_VMEM | 218,518,434 | 352,654,344 | 0.620 |
| fc1 | SQ_WAIT_INST_LDS | 165,069,075 | 133,687,436 | 1.235 |

等待counter为wave聚合，不能相加或当作kernel毫秒占比。计时排名仍来自独立未插桩benchmark。SQ指令统计及bank-conflict部分为emulated。

每shape准确MNK、样本范围、原生kernel符号和事件归属在analysis.json。后续应结合实际ISA/SQTT解释暴露等待，不能把单一counter较高直接等同耗时根因。
