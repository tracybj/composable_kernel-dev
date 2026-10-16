# BF16 成对输出写回实验：CQ / CR（2026-10-13）

结论：四个新候选均不采用。相邻feature打包减少store指令，但交换、打包及对齐回退等成本抵消收益；有效库与已验证shape分流库不变，约330TFLOPS目标未完成。

## 实现与边界

每lane将两行输出转换为CK语义BF16，与lane XOR1交换相邻feature。偶/奇lane分别负责两行，使用32-bit写出两个相邻BF16。aligned路径每wave动态store128→64，输出字节不变；2-byte对齐走两个16-bit bounded store，未缩窄原输出指针域。CQ逐word选择对齐路径，CR将选择移到完整epilogue外。没有改变FP32累加顺序或BF16舍入。

DPP采用compiler-aware __builtin_amdgcn_mov_dpp；DS采用ds_swizzle_b32 offset:0x80b1并保留LGKM完成等待。整个wave执行交换，无奇偶lane发散的store mask。

## 正确性

四个候选每个分别在正常对齐和C偏移一个BF16下完成102组/306次候选launch；合计816组/2448次。每组含3次重复；FP32参考、原版逐位一致、输入不变、输出prefix/tail guard、有限值与确定性检查通过。原版比较和参考额外launch不计入此数。独立位模式probe覆盖72组/216次，包括两个交换后端、64/256线程、3seed、16×16输出裁剪及两种对齐。

## 四类真实shape同轮性能

ck_yyc / HCU4 / gfx936 / DTK25.10.0-0；M18880，BF16输入输出/FP32累加；5次预热、6轮轮换顺序×10次HIP event，取轮均值中位数。profiler关闭。表内Δtime>0表示比本轮BASE更慢；不是H3 E2E收益。

### stage_cq_paired_feature_output_builtin

| shape | candidate | ms | TFLOPS | Δtime vs BASE |
|---|---|---:|---:|---:|
| fc2 | reg768 | 4.871348 | 298.703 | +0.000% |
| fc2 | shapedispatch_reg768 | 4.844053 | 300.386 | -0.560% |
| fc2 | pairout1_reg768 | 4.910196 | 296.340 | +0.797% |
| fc2 | pairout2_reg768 | 4.945261 | 294.239 | +1.517% |
| qkv | reg768 | 7.090963 | 307.805 | +0.000% |
| qkv | shapedispatch_reg768 | 7.089059 | 307.887 | -0.027% |
| qkv | pairout1_reg768 | 7.170131 | 304.406 | +1.116% |
| qkv | pairout2_reg768 | 7.236019 | 301.634 | +2.046% |
| out_proj | reg768 | 2.547918 | 285.544 | +0.000% |
| out_proj | shapedispatch_reg768 | 2.538262 | 286.631 | -0.379% |
| out_proj | pairout1_reg768 | 2.586822 | 281.250 | +1.527% |
| out_proj | pairout2_reg768 | 2.625966 | 277.058 | +3.063% |
| fc1 | reg768 | 9.497586 | 306.412 | +0.000% |
| fc1 | shapedispatch_reg768 | 9.494122 | 306.524 | -0.036% |
| fc1 | pairout1_reg768 | 9.553850 | 304.608 | +0.592% |
| fc1 | pairout2_reg768 | 9.638361 | 301.937 | +1.482% |
### stage_cr_paired_output_alignment_hoist

| shape | candidate | ms | TFLOPS | Δtime vs BASE |
|---|---|---:|---:|---:|
| fc2 | reg768 | 4.865421 | 299.067 | +0.000% |
| fc2 | shapedispatch_reg768 | 4.847268 | 300.187 | -0.373% |
| fc2 | pairout1_reg768 | 4.904564 | 296.680 | +0.805% |
| fc2 | pairout3_reg768 | 4.886149 | 297.799 | +0.426% |
| fc2 | pairout4_reg768 | 4.932093 | 295.024 | +1.370% |
| qkv | reg768 | 7.088963 | 307.892 | +0.000% |
| qkv | shapedispatch_reg768 | 7.089963 | 307.848 | +0.014% |
| qkv | pairout1_reg768 | 7.167507 | 304.518 | +1.108% |
| qkv | pairout3_reg768 | 7.180291 | 303.975 | +1.288% |
| qkv | pairout4_reg768 | 7.232283 | 301.790 | +2.022% |
| out_proj | reg768 | 2.545958 | 285.764 | +0.000% |
| out_proj | shapedispatch_reg768 | 2.535606 | 286.931 | -0.407% |
| out_proj | pairout1_reg768 | 2.584046 | 281.552 | +1.496% |
| out_proj | pairout3_reg768 | 2.564398 | 283.709 | +0.724% |
| out_proj | pairout4_reg768 | 2.596342 | 280.219 | +1.979% |
| fc1 | reg768 | 9.491258 | 306.616 | +0.000% |
| fc1 | shapedispatch_reg768 | 9.492034 | 306.591 | +0.008% |
| fc1 | pairout1_reg768 | 9.569169 | 304.120 | +0.821% |
| fc1 | pairout3_reg768 | 9.600169 | 303.138 | +1.147% |
| fc1 | pairout4_reg768 | 9.652641 | 301.490 | +1.700% |

## 失败记录与修正

1. 初始DS 0x841f和重试0x00b1均为本实验编码错误；分别生成错误quad映射和bitmask映射。不能据此称海光不支持DS交换。正确quad模式标志为0x8000，邻居交换0x80b1通过独立probe。失败文件不删除。
2. 正确DS probe之后，raw inline DPP的完整GEMM仍在首组M1/N256/K64失败，未进入性能测试。ISA显示DPP源生产与DPP间仅1条间隔；raw asm隐藏了后端hazard语义。改compiler builtin后全部审计通过。不通过的库已冻结为failed_gemm.so。
3. 前两次失败probe的SHA分别656ceccc...与a32d0fc...；raw DPP失败GEMM e7aa3dcd...；正式结果使用不同已审计SHA，见JSON。失败运行不计入完成阶段。

依据：[LLVM DS模式编码](https://llvm.org/doxygen/SIDefines_8h.html)、[LLVM swizzle语法](https://llvm.org/docs/AMDGPUModifierSyntax.html)、[LLVM DPP hazard识别](https://llvm.org/doxygen/GCNHazardRecognizer_8cpp_source.html)。通用ISA资料不替代gfx936实测，以上结论同时有实际probe和GEMM审计支撑。

## ISA与资源解释

inventory记录whole static kernel，包含两种对齐分支，不能把128 short+64 dword相加当动态执行192次store。CR也保留两条完整分支；以实际对齐路径解释。所有版本保留512条静态MMAC，具体VGPR/SGPR/LDS/scratch见inventory JSON。减少store条数没有带来整体收益，不能把短K回归截距当实际epilogue耗时。

正式H3及03_gemm/include未改；默认实验选项关闭，compat重编译与22ef3280...逐byte相同。没有commit/push，没有E2E/画质结论。

