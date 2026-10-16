# gfx936 BF16 GEMM：xprof 深度采集与下一轮方向

2026-10-13，`ck_yyc`，物理HCU4，FC2真实shape M18880/N5376/K7168，BF16输入/输出、FP32累加。不启动H3，不切换DTK或修改驱动/频率。当前最快AU库SHA `a0e23fd8c4751c2f7843e782ab7e717dcd82c1a2c021973ad78e807745a2bc8b`。

## 采集完成情况

工具是容器已安装的xprofiler 3.16.0，实际入口`/opt/rocm-6.3.3/bin/xprof`，未在默认PATH中。用户指定手册来自宿主机`/root/workspace/perf_tool/xprof-guide.pdf`和`xcompute-guide.pdf`；本地只读副本在`D:/yyc_workspace/h3_ck_gemm_optimization_20261012/xprof_docs`。

已完成两种采集：

1. `run_xprof_au.sh`：完整11个sections，并加入`--enable-sqtt --enable-raw`。手册要求raw开关配合SQTT才有意义；kernel过滤按名称子串`h3_ck_rolephase,Cijk_`，分别只采一次AU与rocBLAS实际FC2 GEMM。应用输出finite且参考误差检查通过。完整命令、help、工具版本、指标定义与源/库SHA已保留。
2. `run_xprof_metrics_au.sh`：明确命名的counter输出成文本，原AK、当前AU、rocBLAS各3个dispatch。`analyze_xprof_au.py`在CPU解析，验证九次过滤事件、物理gpu-id4和相同wave数量，按已冻结脚本的调用顺序和库对象归属分组，不仅按相同kernel名字猜候选。

原始目录（宿主机）：

- `/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/xprof_au_full_20261013`：约1.3GiB，`.perf`及两个GEMM各8个SE的原始SQTT `.out`，不删除。
- `/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/xprof_au_metrics_20261013`：`capture/results.txt`、log、命令、源/库hash及CPU生成`analysis.json`。

`.perf`是HYGPERF二进制文件，原始SQTT是hex words，不是已解析的opcode时间轴。当前容器仅发现xprof/xprof-dev，没有已安装的xcu离线分析入口；未安装XCompute或猜测私有二进制布局。因此没有声称已获得逐条MMAC/DS指令的精确latency或stall源代码行；这些可用XCompute打开`.perf`进一步查看。已完成的文本counter分析与未插桩基准可独立使用。

## 文本counter对照

以下为每个实现3次的均值；指令计数是本机工具报告值，不能直接当作wall-clock占比。

| 指标 | AK旧双bank | AU当前最快 | rocBLAS |
|---|---:|---:|---:|
| SQ_WAVES | 18,648 | 18,648 | 18,648 |
| SQ_INSTS_VALU | 258,277,656 | 238,414,680 | 191,041,872 |
| SQ_INSTS_SALU | 52,182,144 | 29,613,024 | 27,738,480 |
| SQ_INSTS_LDS | 44,556,288 | 44,556,288 | 44,556,288 |
| SQ_WAIT_INST_LDS | 156,069,759 | 173,698,409 | 52,924,577 |
| SQ_LDS_BANK_CONFLICT（emulated） | 0 | 0 | 0 |
| L2CacheHit（工具整数%） | 84 | 83 | 约78 |

AU相对rocBLAS的LDS发射等待聚合量约**3.28倍**。这是per-SIMD wave等待计数、单位4cycles，nondeterministic；不能写成“LDS占总时间328%”，不能与其他等待相加得到kernel总时间。AU比AK快，但此等待反而增加，恰好说明不能只按单个counter大小给性能排序。

AU减少了工具报告的SALU/VALU指令量，与移除热循环条件和改变输出地址计算的ISA方向一致；实际更快仍以未插桩HIP event证明。当前AU真实四shape为243.97–258.37TFLOPS，同轮参考326.77–330.40TFLOPS，**约330目标尚未完成**。该成绩不是xprof时间或纯MMAC算术探针。

## 必须保留的指标限制

- 本机xprof `gfx_metrics.xml`将SQ指令统计和LDS bank-conflict标注为**emulated**。0 bank冲突和整数`ALUStalledByLDS=0`不等于没有物理LDS瓶颈，不能据此排除LDS发射/依赖等待。
- xprof报告VGPR为分配粒度值224/256，而实际设备ELF是AU221、rocBLAS255；不能仅以数字大小宣称AU驻留率更好，双方LDS都是65536、threads768、无scratch。
- 已安装xprof `metrics.xml`让gfx936继承gfx9的FETCH_SIZE，仅包含TCC EA0；DTK rocprof的gfx936表达式另外计入EA1。因此这次xprof `FetchSize`尚未验证为完整HBM流量，暂不据它计算真实总HBM带宽、roofline或认定“库多读内存却更快”。
- 手册明确完整GC sections、replay中的内存申请/释放会扰动UTCL2。若下一步研究自然工作负载UTCL2，需单独只选`umc_statistics,utcl2_statistics`，而不是引用此次完整采集的命中率。
- SQTT和counter采集改变运行条件。性能排名只用原有6轮轮换、每轮10次HIP-event均值的中位数；不把replay duration混入最快成绩。

## 据此选择的下一轮实验

优先继续核查LDS发射与MMAC/B预取的交错，而不是宣称HBM已经打满，也不是盲目缩小LDS/增加驻留。新AV在现有完整GEMM中比较：未来B预取插入第4/8/12次DS读取的位置，以及上下两组计算wave分别在4/12读取后预取。保持数学、BF16 RNE、READY/ACK及终末vmcnt(0)不变。

实验源`rolephase.cpp`、配置`build.py`、启动`run_stage_av.sh`，原AU二进制保留作同轮控制。不启动H3，先正确性/finite/尾guard验收再计时。是否有效由`results/stage_av_xprof_guided_prefetch_points`实际结果决定，不预写收益。

### AV已完成：不采用更改后的预取点

| 真实M18880 / TFLOPS | AU控制（点8） | 点4 | 点12 | wave两组4/12 |
|---|---:|---:|---:|---:|
| FC2 | 255.457 | 247.038 | 246.941 | 241.013 |
| QKV | 258.201 | 250.164 | 250.467 | 244.107 |
| out_proj | 243.908 | 237.825 | 236.870 | 232.334 |
| FC1 | 258.039 | 250.245 | 250.595 | 244.260 |

所有四投影的精确/随机数值、finite、输出尾guard均通过，三个新候选库SHA均不同，并保存设备ISA。原AU库哈希仍为a0e23fd8...，同轮重复性能与AU首次测量一致，因此不是用不同版本baseline作比较。改变预取点反而慢约2.6%–6%，未采用；保留点8。这个负结果说明LDS等待方向值得研究，但简单错开预取位置不够，不能仅凭counter预判优化成功。

下一步需结合原生库与当前候选的实际指令安排，分别核查DS发射/消费交错、双相位math安排和输出转换的指令成本；必要时用XCompute打开已保留的SQTT确认具体空泡。当前最优独立复测仍约244–258TFLOPS，距离约330尚有差距。
