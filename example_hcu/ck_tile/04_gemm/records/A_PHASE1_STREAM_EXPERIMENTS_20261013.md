# Phase1 A fragment streaming 实验（CW，2026-10-13）

结论：正确性通过但两个候选吞吐274–281TFLOPS，慢于有效库，不采用。不能因为新增预取或静态交错指令就声称加速。

## 算法 / 数据保护

原phase0/1先各读取16个A fragment。本实验在phase0最后一组j1-hi MMAC使用某个A fragment后，复用av[i]读取当前受保护K64 slot的phase1片段；group1逐个、group4每四个发射。phase1跳过重复读取，但在首次MMAC前保留LGKM完成等待：early组由原LGKM0+ACK1保证，late组显式LGKM0。phase0 ACK0期间producer只写另一slot；当前slot直到所有phase1读取完成、ACK1匹配后才可被tile+2覆盖。startup/VM等待/terminal drain不变，非删除同步。

只允许原FW256/CW8/PW4/MW1/MT16/NT2/WK64+K32 bank，未声称与历史其它重排组合兼容。FP32累加顺序/BF16 RNE/API域/输出store不变。macro H3_PHASE_A_PHASE1_PREFETCH默认0，compat逐byte同22ef。

CPU证明所有phase0 lo/hi、j0/j1使用已完成才复用av；phase18192元素覆盖与257种slot关系正确。两GPU候选各126组/378次原版逐位广审计，odd K、小M真实N/K、输入/guard/finite/确定性/FP32参考通过。CPU保护逻辑不替代GPU验证或支持任意未测组合。

## 四shape同轮计时

HCU4 / ck_yyc / gfx936 / DTK25.10.0-0；M18880，BF16输入输出/FP32累加，5预热+6轮轮换×10HIP event、中位数、profiler关闭。Δtime>0表示较BASE慢。

| shape | candidate | ms | TFLOPS | Δtime vs BASE |
|---|---|---:|---:|---:|
| fc2 | BASE | 4.871613 | 298.687 | +0.000% |
| fc2 | shape dispatch | 4.854533 | 299.738 | -0.351% |
| fc2 | stream1 | 5.267372 | 276.246 | +8.124% |
| fc2 | stream4 | 5.233588 | 278.029 | +7.430% |
| qkv | BASE | 7.086523 | 307.998 | +0.000% |
| qkv | shape dispatch | 7.088171 | 307.926 | +0.023% |
| qkv | stream1 | 7.808050 | 279.536 | +10.182% |
| qkv | stream4 | 7.796714 | 279.943 | +10.022% |
| out_proj | BASE | 2.548502 | 285.479 | +0.000% |
| out_proj | shape dispatch | 2.538630 | 286.589 | -0.387% |
| out_proj | stream1 | 2.651166 | 274.424 | +4.028% |
| out_proj | stream4 | 2.645142 | 275.049 | +3.792% |
| fc1 | BASE | 9.495090 | 306.493 | +0.000% |
| fc1 | shape dispatch | 9.494114 | 306.524 | -0.010% |
| fc1 | stream1 | 10.414705 | 279.430 | +9.685% |
| fc1 | stream4 | 10.367785 | 280.694 | +9.191% |

## 实际ISA / 资源

| candidate | VGPR | SGPR | LDS bytes | scratch | spill | static mixed DS |
|---|---:|---:|---:|---:|---:|---:|
| BASE | 225 | 31 | 65536 | 0 | 0 | 0 |
| stream1 | 241 | 31 | 65536 | 0 | 0 | 64 |
| stream4 | 241 | 31 | 65536 | 0 | 0 | 64 |

预期重排确实进入ISA：同一barrier区域内MMAC前后之间的DS读取静态0→64；VGPR225→241，没有spill。静态64包含多个控制body，不是每tile动态64或已证明的重叠时间。寄存器活跃范围增大且issue交错变化是需要关注的成本，但不能仅据此断言全部回退来源。

## FC2同轮 xprof / DTK 对照

| metric | BASE | stream4 | new/base |
|---|---:|---:|---:|
| SQ_WAVES | 18,648.00 | 18,648.00 | 1.000 |
| SQ_INSTS_VALU | 222,340,104.00 | 222,340,104.00 | 1.000 |
| SQ_INSTS_SALU | 27,661,200.00 | 27,661,200.00 | 1.000 |
| SQ_INSTS_LDS | 44,556,288.00 | 44,556,288.00 | 1.000 |
| SQ_INSTS_VMEM_RD | 11,163,936.00 | 11,163,936.00 | 1.000 |
| SQ_WAIT_CNT_VM | 193,894,617.67 | 222,979,061.33 | 1.150 |
| SQ_WAIT_CNT_LGKM | 89,184,588.33 | 47,340,827.33 | 0.531 |
| SQ_WAIT_BARRIER | 365,795,896.33 | 422,503,244.67 | 1.155 |
| SQ_WAIT_INST_VMEM | 123,394,293.67 | 156,969,489.00 | 1.272 |
| SQ_WAIT_INST_LDS | 82,129,702.67 | 103,030,264.33 | 1.254 |
| L2CacheHit | 83.33 | 76.00 | 0.912 |
| VALUBusy | 67.67 | 63.33 | 0.936 |

## 限制 / 后续

- 等待为wave聚合，不是毫秒或可相加的耗时占比；仅与同轮相同wave数实现比较。
- SQ指令/冲突部分为emulated；占比整值及replay不能精确还原自然未插桩执行。
- profiler耗时不用于排名；原始3样本xprof/DTK计数分开保留。
- HCU4以visibility及xprof gpu-id4确认；DTK CSV driver编号不等同其它物理卡。
- 没有推断隐藏的.perf或SQTT，也未声称occupancy增加。

当前A读取时机的重排无收益；若再迭代，应控制实际VGPR活跃范围/数学issue竞争，不能再原样重复“添加预取”的假设。约330TFLOPS目标仍未完成，原有效库/可选分流不替换；03_gemm/include/H3未改，无commit/push。

