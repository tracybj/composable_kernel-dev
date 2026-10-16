# K64结构化加载与descriptor bit62配对实验（CZ，2026-10-14）

## 做了什么

仅04_gemm新增structured_b64.hpp，未改共享CK。B激活按逻辑row分record，stride取不大于K*2的最大2次幂，余数row*(K*2-stride)保留在voffset。每K64 bank只更新一次descriptor的低48位pointer，保留stride/cache高位；两K32子片段用指令immediate0/64，同row两片后再下一row。

Cache0/1唯一配置差别为descriptor第62位；两者使用相同结构化寻址、record count、K64 rebase、发射和等待。另成对比较phase0 VM0和phase0 VM1/phase1 VM4。后者按row-major消费第1/第3payload的完成关系，不是随意删等待。

有效tile的record count为剩余token行数；unused terminal K64 bank置record count0，使无效row/terminal全零，避免shifted pointer越界。原startup、terminal VM drain、pending bank live、两barrier、A/LDS保护、数学顺序和CK BF16 RNE保留。私有接口只用于本实验完整K64域，不宣称通用任意stride支持。

## 正确性

CPU模型225280项：地址等价、尾row判定、物理末端和高descriptor位保持。硬件raw int16位模式（非量化）2640组/5280次：剩余M1/3/63/192/256，K64/192/320/3584/5376/7168，首/中/末及terminal tile，两K32半块、lane0/24，3seed×2repeat×cache0/1；exact、输入不变、prefix/tail guard全部通过。

四完整GEMM候选各126组/378次，共504组/1512次candidate launch，含odd K和真实N/K小M尾部；输出与原有效22ef逐位一致、FP32误差、finite、guard及确定性通过。参考及旧库额外launch不计入。关闭新宏的BASE和CY row parent兼容重编译逐byte不变。

## 四类真实shape计时

HCU4 / ck_yyc / gfx936 / CK5c8ea51e / DTK25.10.0-0。M18880，BF16输入输出、FP32累加，5预热、6轮轮换×10 HIP event，轮均值中位数；profiler关闭。Δtime正数为较同轮BASE慢；不使用历史筛选最低值。

| shape | candidate | ms | TFLOPS | Δtime vs BASE |
|---|---|---:|---:|---:|
| fc2 | BASE K32 raw | 4.872612 | 298.626 | +0.000% |
| fc2 | validated shape dispatch | 4.844668 | 300.348 | -0.573% |
| fc2 | K64 raw / wait0 | 4.949076 | 294.012 | +1.569% |
| fc2 | K64 raw / wait1/4 | 4.933316 | 294.951 | +1.246% |
| fc2 | struct K64 cache0 / wait0 | 5.509636 | 264.099 | +13.074% |
| fc2 | struct K64 cache1 / wait0 | 5.004804 | 290.738 | +2.713% |
| fc2 | struct K64 cache0 / wait1/4 | 5.513556 | 263.911 | +13.154% |
| fc2 | struct K64 cache1 / wait1/4 | 4.897420 | 297.113 | +0.509% |
| qkv | BASE K32 raw | 7.083850 | 308.114 | +0.000% |
| qkv | validated shape dispatch | 7.084170 | 308.100 | +0.005% |
| qkv | K64 raw / wait0 | 7.257962 | 300.722 | +2.458% |
| qkv | K64 raw / wait1/4 | 7.234074 | 301.715 | +2.121% |
| qkv | struct K64 cache0 / wait0 | 8.125361 | 268.620 | +14.703% |
| qkv | struct K64 cache1 / wait0 | 7.399978 | 294.951 | +4.463% |
| qkv | struct K64 cache0 / wait1/4 | 8.142089 | 268.068 | +14.939% |
| qkv | struct K64 cache1 / wait1/4 | 7.419330 | 294.182 | +4.736% |
| out_proj | BASE K32 raw | 2.550038 | 285.307 | +0.000% |
| out_proj | validated shape dispatch | 2.538086 | 286.651 | -0.469% |
| out_proj | K64 raw / wait0 | 2.605774 | 279.205 | +2.186% |
| out_proj | K64 raw / wait1/4 | 2.593894 | 280.483 | +1.720% |
| out_proj | struct K64 cache0 / wait0 | 2.866566 | 253.803 | +12.413% |
| out_proj | struct K64 cache1 / wait0 | 2.490614 | 292.114 | -2.330% |
| out_proj | struct K64 cache0 / wait1/4 | 2.871318 | 253.383 | +12.599% |
| out_proj | struct K64 cache1 / wait1/4 | 2.494270 | 291.686 | -2.187% |
| fc1 | BASE K32 raw | 9.498273 | 306.390 | +0.000% |
| fc1 | validated shape dispatch | 9.498529 | 306.382 | +0.003% |
| fc1 | K64 raw / wait0 | 9.724777 | 299.254 | +2.385% |
| fc1 | K64 raw / wait1/4 | 9.681441 | 300.593 | +1.928% |
| fc1 | struct K64 cache0 / wait0 | 10.848640 | 268.253 | +14.217% |
| fc1 | struct K64 cache1 / wait0 | 9.891577 | 294.207 | +4.141% |
| fc1 | struct K64 cache0 / wait1/4 | 10.874776 | 267.608 | +14.492% |
| fc1 | struct K64 cache1 / wait1/4 | 9.899112 | 293.984 | +4.220% |

## bit62本轮配对效果

以下正数表示打开bit62后耗时下降；这是本轮结构化流水中的效果，不是相对raw/rocBLAS的收益。

| shape | wait0 bit62 time reduction | wait1/4 bit62 time reduction |
|---|---:|---:|
| fc2 | +9.163% | +11.175% |
| qkv | +8.927% | +8.877% |
| out_proj | +13.115% | +13.132% |
| fc1 | +8.822% | +8.972% |

## 实际资源和指令

| candidate | VGPR | SGPR | LDS | scratch | spill | structured B load static | immediate64 static |
|---|---:|---:|---:|---:|---:|---:|---:|
| struct K64 cache0 / wait0 | 240 | 32 | 65536 | 0 | 0 | 24 | 12 |
| struct K64 cache1 / wait0 | 240 | 32 | 65536 | 0 | 0 | 24 | 12 |
| struct K64 cache0 / wait1/4 | 240 | 32 | 65536 | 0 | 0 | 24 | 12 |
| struct K64 cache1 / wait1/4 | 240 | 32 | 65536 | 0 | 0 | 24 | 12 |

静态指令含互斥wave-group body，不能解释为每tile次数、带宽或时间占比。idxen offen证明结构化指令进入实际二进制，但bit62状态要由冻结helper和编译配置共同核实，不是仅看该opcode。

## 结论

本轮没有四shape整体超过有效库的候选，不替换原有效22ef和已验证可选分流库d2e。
bit62配对结果仅对本轮shape/布局/流水有效；不能把完整候选相对raw的差距全部归因于该位。若某shape微小差异，先独立复测，不当作稳定收益。
约330TFLOPS目标尚未完成；没有H3/INT8/E2E测试，未commit/push，未改03_gemm/include/toolchain/driver。

重放：在同容器04_gemm运行bash run_stage_cz.sh（拒绝覆盖已有证据）；报告python3 analyze_struct64.py仅CPU。

