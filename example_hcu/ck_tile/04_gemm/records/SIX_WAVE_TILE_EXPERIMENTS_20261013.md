# 128×256 Tile / 4 Compute + 2 Producer Wave（CT，2026-10-13）

## 实现

rolephase.cpp新增默认8/4的compute/producer wave配置；仅实验模式FW128/CW4/PW2/MW1开启。每CTA384线程，NT4/MT8，每compute lane仍128个FP32累加器，LDS32KiB；区别于上一轮CS的NT2/MT8、64个累加器及8+4 waves。两个producer wave分两次调用原四逻辑组搬运（physical wave p负责logical p及p+2），保持原LDS phys双射、所有VM/LGKM等待与matched barriers。

stagger以wave2分两组；B每phase4个向量、future也4个，因此vmcnt4完成所有较老current载荷；startup/terminal仍vmcnt0。terminal四个unused future向量都保持live，防止pending寄存器被提前复用。每C的K累加顺序和BF16 RNE不改。

原库256×256/8+4路径默认不变，compat逐byte SHA22ef；reg768名称指沿用编译器VGPR pool选项，不代表此实验实际768线程（实际384）。没有替换rocBLAS、没有INT8、没有H3 E2E。

## 正确性

CPU验证2物理/4逻辑producer覆盖、LDS严格双射、C输出覆盖；257种K64 tile数量的matched barrier次数及B future FIFO等待条件。CPU顺序证明不替代GPU；两个候选各126组/378次（3seed、两种分布、3repeat、odd K tile、真实N/K小M尾部）与原库逐位一致，输入不变、guard/finite、确定性及FP32参考误差通过。

## 同轮计时

ck_yyc / HCU4 / gfx936 / DTK25.10.0-0，M18880，BF16输入输出/FP32累加；5次预热、6轮轮换×10 HIP events，取轮均值中位数，关闭profiler。Δtime>0表示更慢。

| shape | candidate | ms | TFLOPS | Δtime vs BASE |
|---|---|---:|---:|---:|
| fc2 | BASE | 4.867989 | 298.909 | +0.000% |
| fc2 | 已验证shape分流 | 4.851317 | 299.937 | -0.342% |
| fc2 | 8+4 wave / f128 | 8.121258 | 179.170 | +66.830% |
| fc2 | 4+2 wave / point4 | 7.889930 | 184.423 | +62.078% |
| fc2 | 4+2 wave / point6 | 7.888738 | 184.451 | +62.053% |
| qkv | BASE | 7.090675 | 307.817 | +0.000% |
| qkv | 已验证shape分流 | 7.094011 | 307.672 | +0.047% |
| qkv | 8+4 wave / f128 | 12.077903 | 180.713 | +70.335% |
| qkv | 4+2 wave / point4 | 11.768991 | 185.456 | +65.978% |
| qkv | 4+2 wave / point6 | 11.766768 | 185.491 | +65.947% |
| out_proj | BASE | 2.545838 | 285.778 | +0.000% |
| out_proj | 已验证shape分流 | 2.534766 | 287.026 | -0.435% |
| out_proj | 8+4 wave / f128 | 4.160125 | 174.885 | +63.409% |
| out_proj | 4+2 wave / point4 | 4.044877 | 179.868 | +58.882% |
| out_proj | 4+2 wave / point6 | 4.043341 | 179.936 | +58.822% |
| fc1 | BASE | 9.492945 | 306.562 | +0.000% |
| fc1 | 已验证shape分流 | 9.489705 | 306.667 | -0.034% |
| fc1 | 8+4 wave / f128 | 16.107420 | 180.673 | +69.678% |
| fc1 | 4+2 wave / point4 | 15.692964 | 185.445 | +65.312% |
| fc1 | 4+2 wave / point6 | 15.686973 | 185.515 | +65.249% |

## 设备资源

| candidate | VGPR | SGPR | LDS bytes | scratch | spill |
|---|---:|---:|---:|---:|---:|
| BASE | 225 | 31 | 65536 | 0 | 0 |
| 4+2 wave / point4 | 210 | 31 | 32768 | 0 | 0 |
| 4+2 wave / point6 | 210 | 31 | 32768 | 0 | 0 |

## 结论

未获得四shape整体改善，不替换有效库。新wave配置是否改善CS与是否胜过BASE分别看同轮表，不能混淆。
330TFLOPS目标仍未完成。03_gemm/include/H3未修改，无commit/push；下一步需依据ISA和实测决定，不因通过正确性就采用。

