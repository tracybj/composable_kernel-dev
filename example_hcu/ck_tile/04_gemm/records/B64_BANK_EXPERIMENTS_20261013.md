# 完整K64 B bank：一次rebase与分相位等待实验

本轮CL–CP共5阶段/12候选，全部原版逐位广审计通过，但**尚未胜过有效K32方案，也未达到约330T目标**。仅基于CK原始路径派生的04_gemm BF16单核，不启动H3；未commit/push。

## 实现与边界

- `raw_b_base64.hpp`将未来bank共同的K64 byte位移加到resource base并等量缩小bound，每bank只做一次。K32子相位0/32留在voffset；M-tail仍由raw resource限制，不扩大物理访问末端。
- producer/consumer的有效K64 tile为0..K/64-1，unused最后future为K/64。任一remaining token数>=1，所以common<=K*2<=bound，包括一行尾部也不会减法下溢；不用旧逐K32 rebase中的终末clamp及descriptor字段反复readfirstlane。
- 两基地址读取与原phys映射等价。预取点和共享startup不改变K累加顺序、匹配barrier和终末drain；共享startup既测全部bank捕获，也测只捕获不可变初始payload。
- CP仅在phase1发出4个future load的布局启用：phase0等待最早的两个当前向量（vmcnt2），phase1在四个更新的future之后等待全部更老的当前数据（vmcnt4）。初始4向量仍startup vmcnt0，最后全部bank仍vmcnt0 drain，不是省略必要数据依赖等待。

## 验证

12候选各17尺寸×3seed×2分布，共102组/306次，合计1224组/3672次。精确二进制分数、正常随机FP32参考误差、finite、输入不变、输出guard、三次重复determinism与原22ef输出逐位一致全部通过。含K64 tile数1/3/5/7/11/13及实际M18880四shape。关闭新startup路径的两个K64父版本compat重编译也逐byte同SHA。

## 性能：均为各阶段同轮HIP event

HCU4、gfx936、DTK25.10.0-0、CK SHA5c8ea51ed7ac03bdcc24aeb811d51acc57233b60；BF16 A/W/C、FP32累加，M18880。5次预热，6轮轮换顺序×每轮10次，轮均值中位数，profiler off/hot-cache。不要用跨轮最小值或counter推算排名。

### stage_cl_native_b64_single_rebase

| shape | K32控制 | 已验选择库 | nb64p1_late | nb64p1_late_base64 | nb64p1_endwait_base64 | F.linear |
|---|---:|---:|---:|---:|---:|---:|
| fc2 | 298.840 | 300.376 | 273.310 | 290.978 | 279.868 | 327.673 |
| qkv | 307.850 | 307.938 | 277.179 | 296.445 | 283.174 | 327.749 |
| out_proj | 285.679 | 286.979 | 260.298 | 277.441 | 265.811 | 331.608 |
| fc1 | 306.487 | 306.413 | 276.345 | 295.884 | 282.143 | 326.650 |

### stage_cm_native_b64_rebase_schedule

| shape | K32控制 | 已验选择库 | nb64p1_late_base64 | nb64p0_late_base64 | nb64p1_late_base64_np4 | nb64p1_late_base64_np12 | nb64p1_late_base64_atwob1 | F.linear |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| fc2 | 298.961 | 300.097 | 290.941 | 232.868 | 287.209 | 288.526 | 293.702 | 330.256 |
| qkv | 307.970 | 307.912 | 296.546 | 240.007 | 292.356 | 295.010 | 300.174 | 327.890 |
| out_proj | 285.769 | 287.074 | 277.637 | 229.068 | 274.465 | 275.235 | 279.122 | 332.524 |
| fc1 | 306.511 | 306.496 | 295.957 | 239.527 | 291.456 | 293.861 | 298.967 | 327.653 |

### stage_cn_native_b64_shared_startup

| shape | K32控制 | 已验选择库 | nb64p1_late_base64 | nb64p1_late_base64_atwob1 | nb64p1_late_base64_bhoist | nb64p1_late_base64_atwob1_bhoist | F.linear |
|---|---:|---:|---:|---:|---:|---:|---:|
| fc2 | 298.484 | 300.053 | 290.947 | 293.484 | 288.204 | 259.748 | 326.717 |
| qkv | 307.882 | 307.951 | 296.425 | 300.045 | 293.058 | 263.865 | 328.133 |
| out_proj | 285.312 | 286.616 | 277.418 | 279.147 | 275.376 | 248.298 | 332.543 |
| fc1 | 306.535 | 306.619 | 296.034 | 299.069 | 291.987 | 263.029 | 326.325 |

### stage_co_native_b64_immutable_initial

| shape | K32控制 | 已验选择库 | nb64p1_late_base64_atwob1 | nb64p1_late_base64_binit | nb64p1_late_base64_atwob1_binit | F.linear |
|---|---:|---:|---:|---:|---:|---:|
| fc2 | 299.150 | 300.503 | 293.817 | 291.269 | 260.112 | 329.576 |
| qkv | 307.623 | 307.585 | 300.242 | 296.242 | 263.901 | 327.898 |
| out_proj | 285.738 | 286.970 | 279.270 | 277.587 | 248.628 | 332.138 |
| fc1 | 306.613 | 306.679 | 299.097 | 296.242 | 262.985 | 326.557 |

### stage_cp_native_b64_phase_waits

| shape | K32控制 | 已验选择库 | nb64p1_late_base64 | nb64p1_late_base64_atwob1 | nb64p1_late_base64_pwait | nb64p1_late_base64_atwob1_pwait | F.linear |
|---|---:|---:|---:|---:|---:|---:|---:|
| fc2 | 298.460 | 300.199 | 290.822 | 293.573 | 290.419 | 295.263 | 325.189 |
| qkv | 307.906 | 307.864 | 296.440 | 300.012 | 296.249 | 301.506 | 327.881 |
| out_proj | 285.583 | 286.652 | 277.607 | 279.381 | 277.572 | 281.214 | 332.210 |
| fc1 | 306.451 | 306.491 | 295.917 | 298.908 | 295.593 | 300.765 | 327.356 |

## 结论及实际资源

CL相对同轮旧raw K64父方案耗时下降6.07%–6.60%，但该raw父方案本来没有终末clamp；不能把收益全部归因于‘删clamp’。该数据主要支持共同K偏移转入SGPR寻址的组合更高效，仍需要实际指令/等待分析。

| 实现 | VGPR | SGPR | spill | scratch bytes |
|---|---:|---:|---:|---:|
| bbase_reg768 | 225 | 31 | 0 | 0 |
| nb64p1_late_reg768 | 246 | 27 | 0 | 0 |
| bbase_nb64p1_late_reg768 | 242 | 28 | 0 | 0 |
| nb64p1_late_base64_reg768 | 244 | 31 | 0 | 0 |
| nb64p1_late_base64_atwob1_reg768 | 240 | 31 | 0 | 0 |
| nb64p1_late_base64_bhoist_reg768 | 256 | 36 | 3 | 16 |
| nb64p1_late_base64_atwob1_bhoist_reg768 | 256 | 36 | 1 | 8 |
| nb64p1_late_base64_binit_reg768 | 256 | 36 | 2 | 12 |
| nb64p1_late_base64_atwob1_binit_reg768 | 256 | 36 | 1 | 8 |
| nb64p1_late_base64_pwait_reg768 | 244 | 31 | 0 | 0 |
| nb64p1_late_base64_atwob1_pwait_reg768 | 240 | 31 | 0 | 0 |

两基地址把新K64 VGPR244→240并改善约1%；移动全/部分startup反而达到256VGPR并引入spill，且性能回退。不能仅凭spill数量量化全部回退，指令/等待安排也改变。CP两基地址+分相位等待小幅恢复到约281–302T，仍落后有效K32/选择库。
静态compute区域的readfirstlane由旧逐K32 rebase的14降到新K64 rebase的4；这些包括展开及startup/tail，不能当作每动态tile次数或等待毫秒。具体ISA/metadata在b64_bank_instruction_inventory_final_20261013。
原有效22ef库及d2e34...已验证可选选择库保持不变，不采用本轮K64方案；没有模型E2E数据。目标保持约330T，不将家族内部改善宣布为目标完成。

