# 128-feature Tile GEMM 实验（CS，2026-10-13）

目标：减少每CTA累加器和LDS，尝试改善驻留/等待隐藏；不是INT8或模型端到端实验。

## 改动

rolephase.cpp新增H3_PHASE_FEATURE_TILE，默认256；新候选设128。计算wave仍8个、producer wave仍4个、768线程。MT16→8、NT仍2，每lane累加器128→64个FP32，双缓冲LDS64→32KiB。producer每wave权重load8→4、slot stride同步减半。feature方向CTA数加倍，token tile仍256；保持原K64 A/K32 B bank、matched barrier、静态stagger、FP32累加顺序、CK BF16 RNE和bounded输出。prefetch点分别4/6。

权衡：CTA变多、B token载荷被更多feature tile复用/重复请求；资源减小并不保证性能提升。只据真实测量判断，不凭资源推断已经达到更高occupancy。

API继续仅接受N整除256、K整除64及16-byte对齐输入；原输出2-byte语义保留。未拓宽输入域。新feature域静态限制只允许已选K32路径，未声称支持所有历史实验组合。

## 验证

CPU证明128/256的LDS地址为严格双射、producer覆盖、C输出覆盖及各35种grid形状无遗漏/重复。两候选各126组/378次，含奇数K tile、小M真实N/K、3seed、两种分布及重复执行；与原版逐位一致、输入/guard/finite/确定性检查通过。关闭新布局后的compat与原版逐byte同SHA。

首次构建仅编译失败，GPU未运行：DS_PROGRESS缺默认宏，未使用accumulator-bank helper假定宽度128。已补默认0及helper有效宽度上限；256域行为不变，失败源码/build.log冻结保留，不计入完整阶段。

## 同轮四shape

ck_yyc / HCU4 / gfx936 / DTK25.10.0-0，M18880；BF16输入输出/FP32累加。5预热、6轮轮换×10 HIP event，取轮均值中位数，profiler关闭。

| shape | candidate | ms | TFLOPS | Δtime vs 原库 |
|---|---|---:|---:|---:|
| fc2 | BASE | 4.871429 | 298.698 | +0.000% |
| fc2 | shape dispatch | 4.850341 | 299.997 | -0.433% |
| fc2 | f128 / point4 | 8.142170 | 178.710 | +67.141% |
| fc2 | f128 / point6 | 8.130986 | 178.956 | +66.912% |
| qkv | BASE | 7.091803 | 307.768 | +0.000% |
| qkv | shape dispatch | 7.093587 | 307.691 | +0.025% |
| qkv | f128 / point4 | 12.111383 | 180.213 | +70.780% |
| qkv | f128 / point6 | 12.078967 | 180.697 | +70.323% |
| out_proj | BASE | 2.547446 | 285.597 | +0.000% |
| out_proj | shape dispatch | 2.538086 | 286.651 | -0.367% |
| out_proj | f128 / point4 | 4.167741 | 174.566 | +63.605% |
| out_proj | f128 / point6 | 4.162277 | 174.795 | +63.390% |
| fc1 | BASE | 9.493489 | 306.544 | +0.000% |
| fc1 | shape dispatch | 9.493937 | 306.530 | +0.005% |
| fc1 | f128 / point4 | 16.154020 | 180.152 | +70.159% |
| fc1 | f128 / point6 | 16.106260 | 180.686 | +69.656% |

## 设备资源

| candidate | VGPR | SGPR | LDS bytes | scratch | spill |
|---|---:|---:|---:|---:|---:|
| BASE | 225 | 31 | 65536 | 0 | 0 |
| f128 / point4 | 129 | 31 | 32768 | 0 | 0 |
| f128 / point6 | 129 | 31 | 32768 | 0 | 0 |

## 结论

没有四shape均稳定更快的新候选，不替换原库；减少资源不等于净性能收益。
约330TFLOPS目标保持未完成。H3/03_gemm/include未修改，不commit/push。

## 同轮 counter 验证

已完成一次full sections原始.perf采集及BASE/point6/rocBLAS文本counter对照。point6总wave2倍、SALU指令聚合1.799倍、VMEM read指令聚合1.501倍；DTK VM等待聚合3.061倍、barrier等待2.414倍。它们支持控制/等待摊销变差的解释，但不等同于时间占比，不能据此分配精确毫秒。SQ指令/冲突统计部分是emulated；.perf未猜测解析，不宣称已实测occupancy翻倍。
详细原始counter、3次均值、限制在results/profile_cs_feature_tile_compare_20261013/ANALYSIS.md和analysis.json；full capture在results/xprof_cs_feature128_20261013，输出检查通过。下一布局若继续，应同步调整compute/producer wave数、保持每wave算术量，而不是只缩tile。该后续未实施，不计作已完成优化。

