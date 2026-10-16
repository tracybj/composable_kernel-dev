# A预取窗口 / K64 B加载顺序及immediate实验（CX/CY，2026-10-13）

以约330TFLOPS接近rocBLAS为目标。当前原有效库SHA22ef及已验证shape dispatch d2e不自动替换；不能把家族内部改善写成胜过有效库。

## CX：窗口控制

保留CW last-use跨相位读取和ACK1前LGKM保护，仅预取[0:4)、[0:8)、[12:16)、[8:16)；phase1常规读取剩余片段，无遗漏/重复、K累加顺序及RNE不变。默认begin0/count16重编译与原全窗口ad15逐byte一致，关闭stream后仍22ef。CPU last-use/8192元素覆盖证明通过，actual寄存器数见下表而非按窗口大小推断。

## CY：根据实际native参考片段对照

捕获的rocBLAS ISA先加载同一row/index v6两个K32半块（v9及v9+32），再加载v6+16的两个半块；启动目标B192/196/200/204。这是该捕获kernel的证据，不泛化所有rocBLAS kernel。仅借鉴数据流，没有调用rocBLAS来冒充CK性能。

旧K64 B issue为p0j0/p0j1/p1j0/p1j1；新row-major为p0j0/p1j0/p0j1/p1j1，数学consume的逻辑bank[slot][phase][j]不变。mode1只换issue顺序，mode2另用template instruction immediate0/64共享row/lane voffset。每K64 pointer+bound rebase保留，未用unchecked scalar offset、未扩大resource物理末端；startup/terminal VM0及pending八向量live不变。

分相位wait候选必须修正phase0 vmcnt2→1：新phase0要消费第1和第3个载荷，留2个pending会不安全；phase1发出4个future之后vmcnt4仍完成older current。CPU FIFO证明通过，不把减少等待当任意优化；GPU另外验证真实正确性。其它两候选phase0仍vmcnt0，与未分相位parent对照。

独立raw int16位模式probe（不是量化）M1/3/63/192/256，K64/192/320/7168，首/中/末/unused terminal K64 bank、phase0/1、lane0/24、3seed、2repeat，共840组/1680次。输入随机guard行、输出prefix/tail guard、所有有效与masked尾部值及输入不变通过。terminal中间row可读取下一真实row（未消费payload）、last row masked；不错误要求所有terminal值全零。

## 验证 / 计时契约

七GEMM候选各126组/378次原版逐位审计，odd K tile及真实N/K的小M尾部、输入/guard/finite/确定性/FP32误差通过；合计882组/2646次候选launch，不含参考/原版额外launch。CY关闭新模式后两个K64 parent（45163df、abc5d329）及原22ef全部逐byte一致。

HCU4 / ck_yyc / gfx936 / DTK25.10.0-0 / CK5c8ea51e；M18880，BF16输入输出/FP32累加，5预热、6轮轮换×10 HIP event、中位数，profiler关闭。Δtime>0表示较同轮BASE慢。

## CX：部分A预取窗口

| shape | candidate | ms | TFLOPS | Δtime vs BASE |
|---|---|---:|---:|---:|
| fc2 | BASE | 4.864660 | 299.114 | +0.000% |
| fc2 | shape dispatch | 4.843692 | 300.409 | -0.431% |
| fc2 | A full16 | 5.231012 | 278.166 | +7.531% |
| fc2 | A window 0 + 4 | 4.987228 | 291.763 | +2.520% |
| fc2 | A window 0 + 8 | 5.126596 | 283.831 | +5.384% |
| fc2 | A window 12 + 4 | 5.044932 | 288.426 | +3.706% |
| fc2 | A window 8 + 8 | 5.405444 | 269.189 | +11.117% |
| qkv | BASE | 7.094035 | 307.671 | +0.000% |
| qkv | shape dispatch | 7.091107 | 307.798 | -0.041% |
| qkv | A full16 | 7.790794 | 280.155 | +9.822% |
| qkv | A window 0 + 4 | 7.376042 | 295.908 | +3.975% |
| qkv | A window 0 + 8 | 7.671970 | 284.494 | +8.147% |
| qkv | A window 12 + 4 | 7.551098 | 289.048 | +6.443% |
| qkv | A window 8 + 8 | 7.840626 | 278.375 | +10.524% |
| out_proj | BASE | 2.544230 | 285.958 | +0.000% |
| out_proj | shape dispatch | 2.534822 | 287.020 | -0.370% |
| out_proj | A full16 | 2.644422 | 275.124 | +3.938% |
| out_proj | A window 0 + 4 | 2.572982 | 282.763 | +1.130% |
| out_proj | A window 0 + 8 | 2.584702 | 281.481 | +1.591% |
| out_proj | A window 12 + 4 | 2.566278 | 283.502 | +0.867% |
| out_proj | A window 8 + 8 | 2.634566 | 276.153 | +3.551% |
| fc1 | BASE | 9.495777 | 306.471 | +0.000% |
| fc1 | shape dispatch | 9.491640 | 306.604 | -0.044% |
| fc1 | A full16 | 10.359816 | 280.910 | +9.099% |
| fc1 | A window 0 + 4 | 9.881305 | 294.513 | +4.060% |
| fc1 | A window 0 + 8 | 10.137249 | 287.077 | +6.755% |
| fc1 | A window 12 + 4 | 9.975817 | 291.723 | +5.055% |
| fc1 | A window 8 + 8 | 10.414896 | 279.424 | +9.679% |

| candidate | VGPR | SGPR | LDS bytes | scratch | spill |
|---|---:|---:|---:|---:|---:|
| BASE | 225 | 31 | 65536 | 0 | 0 |
| shape dispatch / _ZN11h3_original18h3_original_kernelEPKtS1_Ptiii | 225 | 31 | 65536 | 0 | 0 |
| shape dispatch / _ZN11h3_selected18h3_selected_kernelEPKtS1_Ptiii | 225 | 31 | 65536 | 0 | 0 |
| A full16 | 241 | 31 | 65536 | 0 | 0 |
| A window 0 + 4 | 225 | 31 | 65536 | 0 | 0 |
| A window 0 + 8 | 225 | 31 | 65536 | 0 | 0 |
| A window 12 + 4 | 225 | 31 | 65536 | 0 | 0 |
| A window 8 + 8 | 225 | 31 | 65536 | 0 | 0 |

未得到四shape整体净收益，原有效库保持。

## CY：B row-major / instruction immediate

| shape | candidate | ms | TFLOPS | Δtime vs BASE |
|---|---|---:|---:|---:|
| fc2 | BASE | 4.864237 | 299.140 | +0.000% |
| fc2 | shape dispatch | 4.841596 | 300.539 | -0.465% |
| fc2 | K64 parent wait0 | 4.951093 | 293.892 | +1.786% |
| fc2 | K64 parent wait2/4 | 4.927596 | 295.294 | +1.303% |
| fc2 | row-only / wait0 | 4.951764 | 293.852 | +1.799% |
| fc2 | row+imm64 / wait0 | 4.950852 | 293.907 | +1.781% |
| fc2 | row+imm64+wait1/4 | 4.936588 | 294.756 | +1.487% |
| qkv | BASE | 7.088683 | 307.904 | +0.000% |
| qkv | shape dispatch | 7.089291 | 307.877 | +0.009% |
| qkv | K64 parent wait0 | 7.270922 | 300.186 | +2.571% |
| qkv | K64 parent wait2/4 | 7.236323 | 301.622 | +2.083% |
| qkv | row-only / wait0 | 7.261771 | 300.565 | +2.442% |
| qkv | row+imm64 / wait0 | 7.257979 | 300.722 | +2.388% |
| qkv | row+imm64+wait1/4 | 7.233763 | 301.728 | +2.047% |
| out_proj | BASE | 2.543502 | 286.040 | +0.000% |
| out_proj | shape dispatch | 2.534062 | 287.106 | -0.371% |
| out_proj | K64 parent wait0 | 2.604486 | 279.343 | +2.398% |
| out_proj | K64 parent wait2/4 | 2.585406 | 281.404 | +1.647% |
| out_proj | row-only / wait0 | 2.603022 | 279.500 | +2.340% |
| out_proj | row+imm64 / wait0 | 2.601310 | 279.684 | +2.273% |
| out_proj | row+imm64+wait1/4 | 2.591126 | 280.783 | +1.872% |
| fc1 | BASE | 9.498537 | 306.381 | +0.000% |
| fc1 | shape dispatch | 9.498857 | 306.371 | +0.003% |
| fc1 | K64 parent wait0 | 9.734169 | 298.965 | +2.481% |
| fc1 | K64 parent wait2/4 | 9.673577 | 300.838 | +1.843% |
| fc1 | row-only / wait0 | 9.730009 | 299.093 | +2.437% |
| fc1 | row+imm64 / wait0 | 9.726681 | 299.195 | +2.402% |
| fc1 | row+imm64+wait1/4 | 9.681641 | 300.587 | +1.928% |

| candidate | VGPR | SGPR | LDS bytes | scratch | spill |
|---|---:|---:|---:|---:|---:|
| BASE | 225 | 31 | 65536 | 0 | 0 |
| shape dispatch / _ZN11h3_original18h3_original_kernelEPKtS1_Ptiii | 225 | 31 | 65536 | 0 | 0 |
| shape dispatch / _ZN11h3_selected18h3_selected_kernelEPKtS1_Ptiii | 225 | 31 | 65536 | 0 | 0 |
| K64 parent wait0 | 240 | 31 | 65536 | 0 | 0 |
| K64 parent wait2/4 | 240 | 31 | 65536 | 0 | 0 |
| row-only / wait0 | 240 | 31 | 65536 | 0 | 0 |
| row+imm64 / wait0 | 237 | 31 | 65536 | 0 | 0 |
| row+imm64+wait1/4 | 237 | 31 | 65536 | 0 | 0 |

未得到四shape整体净收益，原有效库保持。

## ISA证据与限制

| candidate | B load immediate64 static | adjacent shared-Vaddr pair static |
|---|---:|---:|
| bbase_reg768 | 0 | 0 |
| nb64p1_late_base64_atwob1_reg768 | 0 | 0 |
| nb64p1_late_base64_atwob1_pwait_reg768 | 0 | 0 |
| nb64p1_late_base64_atwob1_brow1_reg768 | 0 | 0 |
| nb64p1_late_base64_atwob1_brow2_reg768 | 12 | 12 |
| nb64p1_late_base64_atwob1_pwait_brow2_reg768 | 12 | 12 |

所有数字为whole static kernel（含互斥body），不是每tile动态次数或耗时占比。immediate64是真实MUBUF编码，shared Vaddr只指记录的相邻指令operand相同，不能当字节流量或cache命中收益。
没有H3/INT8/E2E实验；03_gemm/include未修改，未commit/push。GPU审计是正常C对齐；若有候选值得采用再补2-byte输出与独立复测，不声称所有数据分布/NaN质量无条件通过。330目标仍未完成，当前有效22ef及可选d2e不替换，负结果和冻结构建源码保留。

