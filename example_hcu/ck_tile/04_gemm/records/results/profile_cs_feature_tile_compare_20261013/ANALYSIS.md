# 128-feature tile 回退：同轮 counter 对照

FC2 M18880/N5376/K7168；BASE SHA22ef3280...，f128 point6 SHA4f2da558...；HCU4，同轮BASE/候选/rocBLAS各3次，xprof文本和DTK等待counter分开采集。

| metric | BASE | f128 point6 | new/base |
|---|---:|---:|---:|
| SQ_WAVES | 18,648.00 | 37,296.00 | 2.000 |
| SQ_INSTS_VALU | 222,340,104.00 | 217,659,456.00 | 0.979 |
| SQ_INSTS_SALU | 27,661,200.00 | 49,752,864.00 | 1.799 |
| SQ_INSTS_LDS | 44,556,288.00 | 44,556,288.00 | 1.000 |
| SQ_INSTS_VMEM_RD | 11,163,936.00 | 16,758,336.00 | 1.501 |
| SQ_INSTS_VMEM_WR | 1,591,296.00 | 1,591,296.00 | 1.000 |
| SQ_WAIT_CNT_VM | 195,911,814.33 | 599,760,776.00 | 3.061 |
| SQ_WAIT_CNT_LGKM | 88,901,888.00 | 45,066,406.00 | 0.507 |
| SQ_WAIT_BARRIER | 367,747,052.00 | 887,673,397.67 | 2.414 |
| SQ_WAIT_INST_VMEM | 126,032,006.33 | 553,538,767.33 | 4.392 |
| SQ_WAIT_INST_LDS | 81,909,390.67 | 29,098,497.33 | 0.355 |
| L2CacheHit | 84.00 | 86.00 | 1.024 |
| VALUBusy | 68.00 | 40.00 | 0.588 |
| SALUBusy | 4.00 | 5.00 | 1.250 |

## 能确认的事实与解释边界

ELF VGPR225→129，LDS65536→32768，但CTA与总wave翻倍；每compute wave累加器128→64，单phase算术工作减少，matched barrier和循环控制仍须执行。上述指令/等待聚合变化与“控制及等待摊销变差”相容，但不能仅据counter分配精确回退毫秒。不是spill引起：两候选scratch/spill均0。

全sections采集保存在../xprof_cs_feature128_20261013；workload输出finite/error验证通过。.perf没有猜测解析，因此不能声称已实测occupancy翻倍。后续若尝试缩小feature tile，应同时调整compute/producer wave布局以保持每wave算术量，而不是原样保留8+4 wave。该后续尚未实现/验证。

## 限制

- Wait counters aggregate waves; they are not kernel ms/time fractions and are not additive.
- SQ instruction/bank-conflict counters are emulated in installed catalog; not literal physical ISA issue counts.
- L2/VALU/SALU percentages are integer-valued; replay is not uninstrumented steady-state.
- Full sections .perf is preserved, but no supported reader was used to infer occupancy from opaque binary.
- xprof explicitly reports physical gpu-id4; DTK CSV gpu-id12 is a driver enumeration and is not treated as another GPU.
- Profile timings are not performance ranking; CS HIP event results determine regression.

