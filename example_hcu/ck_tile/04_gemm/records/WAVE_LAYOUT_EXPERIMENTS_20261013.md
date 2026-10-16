# CTA / Producer Wave 布局实验：CT / CU（2026-10-13）

以约330TFLOPS接近rocBLAS为目标，本轮测试算术量、producer资源和矩阵tile的配套变化。暂不替换默认22ef或已验证可选d2e；每个候选须依据独立原始计时/正确性决定，不自动采用跨轮最低值。

## 算法与边界

CT将128×256 tile配成4 compute+2 producer，384线程，MT8/NT4，每compute lane128个FP32累加器，与原256×256的128相同；stagger2+2。LDS32KiB，比上一轮CS的8+4/64累加器布局更匹配，但feature CTA仍翻倍，token载荷重复读取仍增加。

CU回到256×256 tile，8 compute+2 producer共640线程，MT16/NT2，原128个FP32累加器/LDS64KiB/feature CTA/B读取次数不变；stagger4+4。两个producer分别发射原logical group p和p+2全部搬运，权重payload不变。单测原producer循环和固定槽pair+A scalar offset组合。

保留完整startup/data wait、两匹配barrier/K64、terminal pending B所有向量live及VM drain；没有删同步、绕过guard、改BF16 RNE或K累加顺序。N%256/K%64与输入16-byte对齐原域不变，默认CW8/PW4与原库compat逐byte同SHA。reg768是编译pool选项名称，不代表新候选实际768线程。

## 计时契约

HCU4，ck_yyc，gfx936，DTK25.10.0-0，CK SHA5c8ea51ed7ac03bdcc24aeb811d51acc57233b60。M18880，BF16输入输出/FP32累加，5次预热、6轮轮换×10 HIP event，取轮均值中位数，profiler关闭。Δtime>0表示相对同轮BASE慢。

## CT / 128x256 / 4+2 waves

每候选126组/378次、含odd K tile及真实N/K的小M尾部，与原库逐位一致、输入/guard/finite/确定性/FP32参考误差全部通过；两stage共504组/1512次候选launch，不含对照及参考额外launch。

| shape | candidate | ms | TFLOPS | Δtime vs BASE |
|---|---|---:|---:|---:|---:|
| fc2 | BASE | 4.867989 | 298.909 | +0.000% |
| fc2 | shape dispatch | 4.851317 | 299.937 | -0.342% |
| fc2 | CS 8+4 / f128 | 8.121258 | 179.170 | +66.830% |
| fc2 | 4+2 / point4 | 7.889930 | 184.423 | +62.078% |
| fc2 | 4+2 / point6 | 7.888738 | 184.451 | +62.053% |
| qkv | BASE | 7.090675 | 307.817 | +0.000% |
| qkv | shape dispatch | 7.094011 | 307.672 | +0.047% |
| qkv | CS 8+4 / f128 | 12.077903 | 180.713 | +70.335% |
| qkv | 4+2 / point4 | 11.768991 | 185.456 | +65.978% |
| qkv | 4+2 / point6 | 11.766768 | 185.491 | +65.947% |
| out_proj | BASE | 2.545838 | 285.778 | +0.000% |
| out_proj | shape dispatch | 2.534766 | 287.026 | -0.435% |
| out_proj | CS 8+4 / f128 | 4.160125 | 174.885 | +63.409% |
| out_proj | 4+2 / point4 | 4.044877 | 179.868 | +58.882% |
| out_proj | 4+2 / point6 | 4.043341 | 179.936 | +58.822% |
| fc1 | BASE | 9.492945 | 306.562 | +0.000% |
| fc1 | shape dispatch | 9.489705 | 306.667 | -0.034% |
| fc1 | CS 8+4 / f128 | 16.107420 | 180.673 | +69.678% |
| fc1 | 4+2 / point4 | 15.692964 | 185.445 | +65.312% |
| fc1 | 4+2 / point6 | 15.686973 | 185.515 | +65.249% |

| candidate | VGPR | SGPR | LDS bytes | scratch | spill |
|---|---:|---:|---:|---:|---:|
| BASE | 225 | 31 | 65536 | 0 | 0 |
| 4+2 / point4 | 210 | 31 | 32768 | 0 | 0 |
| 4+2 / point6 | 210 | 31 | 32768 | 0 | 0 |

没有四shape整体改善，原库保留。

## CU / 256x256 / 8+2 waves

每候选126组/378次、含odd K tile及真实N/K的小M尾部，与原库逐位一致、输入/guard/finite/确定性/FP32参考误差全部通过；两stage共504组/1512次候选launch，不含对照及参考额外launch。

| shape | candidate | ms | TFLOPS | Δtime vs BASE |
|---|---|---:|---:|---:|---:|
| fc2 | BASE | 4.870716 | 298.742 | +0.000% |
| fc2 | shape dispatch | 4.854324 | 299.751 | -0.337% |
| fc2 | 8+2 / original loop | 5.021228 | 289.787 | +3.090% |
| fc2 | 8+2 / pair+soffset | 5.141852 | 282.989 | +5.567% |
| qkv | BASE | 7.088226 | 307.924 | +0.000% |
| qkv | shape dispatch | 7.088194 | 307.925 | -0.000% |
| qkv | 8+2 / original loop | 7.247026 | 301.176 | +2.240% |
| qkv | 8+2 / pair+soffset | 7.256722 | 300.774 | +2.377% |
| out_proj | BASE | 2.547710 | 285.568 | +0.000% |
| out_proj | shape dispatch | 2.537678 | 286.697 | -0.394% |
| out_proj | 8+2 / original loop | 2.583710 | 281.589 | +1.413% |
| out_proj | 8+2 / pair+soffset | 2.532646 | 287.266 | -0.591% |
| fc1 | BASE | 9.492992 | 306.560 | +0.000% |
| fc1 | shape dispatch | 9.494480 | 306.512 | +0.016% |
| fc1 | 8+2 / original loop | 9.708984 | 299.741 | +2.275% |
| fc1 | 8+2 / pair+soffset | 9.745560 | 298.616 | +2.661% |

| candidate | VGPR | SGPR | LDS bytes | scratch | spill |
|---|---:|---:|---:|---:|---:|
| BASE | 225 | 31 | 65536 | 0 | 0 |
| 8+2 / original loop | 225 | 31 | 65536 | 0 | 0 |
| 8+2 / pair+soffset | 225 | 31 | 65536 | 0 | 0 |

没有四shape整体改善，原库保留。

## 限制与目标状态

CPU验证为四逻辑producer覆盖、LDS/输出双射及257种K tile的握手次数，不能替代GPU。本轮广审计是正常输出对齐；新输出逻辑仍原bounded short store，若后续有收益才补2-byte C独立审计与复测；不凭这些测试声称所有输入布局/精度语义或NaN画质验收。

未启动H3、没有INT8/模型E2E收益、没有commit/push、03_gemm/include不变。降低线程或VGPR不等于已证明更高occupancy。本轮实际性能与330目标仍有差距，不将改善较慢CS的百分比混为改善BASE。源码、构建命令及哈希冻结在build/attempts；原库与旧候选保留。

