# K32结构化B CacheSwizzle适配（DD，2026-10-14）

## 实现与安全边界

在原较快的K32寄存器bank流水上新增structured_b32.hpp：pow2 stride+row remainder，每次K32加载将common=(kk&~31)*2放入descriptor低48位pointer，保留高位stride/cache；checked index为逻辑token row，offset为row remainder+(kk&31)*2。bit62 OFF/ON两候选，其余数据/加载节奏相同；另各配producer fixed pair+A soffset，检查组合效果。

有效phase的record count为剩余row；unused terminal phase置0，所有invalid row/terminal数据为零。终末在消费当前bank前明确VM0，以免masked future load的完成行为影响current的等待；startup、原NT future等待、末尾bank live及VM drain、两barrier与LDS保护保留。BF16输入输出、FP32 K累加顺序和CK RNE不变。

默认H3_PHASE_STRUCT_B32=0；新路径只接受K<8192以保证14-bit stride编码，不泛化任意shape/stride；未改共享CK、03或H3。关闭新模式后原22ef和新可选cache dispatch de1c重编译逐byte一致。

## 正确性

CPU地址模型138240项通过。硬件raw int16位模式（非量化）1620组/3240次：M1/3/63/192/256，K64/192/320/3584/5376/7168/8128，首/中/末和terminal K32片段，lane0/24，3seed×2repeat×cache0/1；exact、输入不变和prefix/tail guard通过。另540次GPU descriptor回读确认pointer、14-bit stride、bit62开关、bit63保持0、logical row/terminal bounds。

四完整GEMM各126组/378次，合计504组/1512次候选launch，含odd K与四真实N/K小M尾部，和原有效22ef逐位一致；FP32误差、finite、guard、输入不变与确定性通过。参考额外launch不计入。

## 同轮四shape

HCU4 / ck_yyc / gfx936 / DTK25.10.0-0 / CK5c8ea51e；M18880；5预热、6轮轮换×10 HIP events，中位数，profiler关闭。正数为比同轮对应库更快。

| shape | candidate | ms | TFLOPS | time reduction vs BASE | vs latest optional | wins/6 vs latest |
|---|---|---:|---:|---:|---:|---:|
| fc2 | BASE K32 raw | 4.866444 | 299.004 | +0.000% | -0.502% | 0 |
| fc2 | old producer dispatch | 4.839948 | 300.641 | +0.544% | +0.045% | 3 |
| fc2 | validated cache dispatch | 4.842141 | 300.505 | +0.499% | +0.000% | 0 |
| fc2 | struct K32 cache0 | 4.891101 | 297.497 | -0.507% | -1.011% | 0 |
| fc2 | struct K32 cache1 | 4.435877 | 328.027 | +8.848% | +8.390% | 6 |
| fc2 | struct K32 cache0 + producer | 4.905300 | 296.636 | -0.798% | -1.304% | 0 |
| fc2 | struct K32 cache1 + producer | 4.380205 | 332.196 | +9.992% | +9.540% | 6 |
| qkv | BASE K32 raw | 7.093163 | 307.709 | +0.000% | -0.009% | 3 |
| qkv | old producer dispatch | 7.092202 | 307.751 | +0.014% | +0.004% | 3 |
| qkv | validated cache dispatch | 7.092491 | 307.738 | +0.009% | +0.000% | 0 |
| qkv | struct K32 cache0 | 7.141363 | 305.632 | -0.680% | -0.689% | 0 |
| qkv | struct K32 cache1 | 6.535467 | 333.967 | +7.862% | +7.854% | 6 |
| qkv | struct K32 cache0 + producer | 7.124010 | 306.377 | -0.435% | -0.444% | 0 |
| qkv | struct K32 cache1 + producer | 6.567971 | 332.314 | +7.404% | +7.395% | 6 |
| out_proj | BASE K32 raw | 2.544446 | 285.934 | +0.000% | -2.193% | 0 |
| out_proj | old producer dispatch | 2.534494 | 287.057 | +0.391% | -1.794% | 0 |
| out_proj | validated cache dispatch | 2.489838 | 292.205 | +2.146% | +0.000% | 0 |
| out_proj | struct K32 cache0 | 2.565142 | 283.627 | -0.813% | -3.024% | 0 |
| out_proj | struct K32 cache1 | 2.265374 | 321.158 | +10.968% | +9.015% | 6 |
| out_proj | struct K32 cache0 + producer | 2.546702 | 285.681 | -0.089% | -2.284% | 0 |
| out_proj | struct K32 cache1 + producer | 2.206350 | 329.750 | +13.288% | +11.386% | 6 |
| fc1 | BASE K32 raw | 9.494305 | 306.518 | +0.000% | -0.002% | 3 |
| fc1 | old producer dispatch | 9.496633 | 306.443 | -0.025% | -0.026% | 3 |
| fc1 | validated cache dispatch | 9.494145 | 306.523 | +0.002% | +0.000% | 0 |
| fc1 | struct K32 cache0 | 9.550105 | 304.727 | -0.588% | -0.589% | 0 |
| fc1 | struct K32 cache1 | 8.796034 | 330.851 | +7.355% | +7.353% | 6 |
| fc1 | struct K32 cache0 + producer | 9.534489 | 305.226 | -0.423% | -0.425% | 0 |
| fc1 | struct K32 cache1 + producer | 8.816530 | 330.082 | +7.139% | +7.137% | 6 |

## 实际资源 / ISA

| candidate | VGPR | SGPR | LDS | scratch | spill | structured B load static |
|---|---:|---:|---:|---:|---:|---:|
| struct K32 cache0 | 233 | 44 | 65536 | 0 | 0 | 20 |
| struct K32 cache1 | 233 | 38 | 65536 | 0 | 0 | 20 |
| struct K32 cache0 + producer | 233 | 44 | 65536 | 0 | 0 | 20 |
| struct K32 cache1 + producer | 233 | 38 | 65536 | 0 | 0 | 20 |

## 判定

有候选/shape胜过已验证可选库，需独立复测和C偏移2-byte审计，不自动采用。
descriptor真实开启bit62不保证完整流水更快；不能把资源、静态指令数或cache标记直接当性能收益。静态ISA包括互斥body，非动态耗时比例。当前330T目标未完成；保留22ef默认和de1c可选，不是INT8/H3 E2E测试，不commit/push。

