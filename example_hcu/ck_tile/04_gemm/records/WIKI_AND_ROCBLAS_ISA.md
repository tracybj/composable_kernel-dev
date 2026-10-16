# KernelWiki 迁移思路与 rocBLAS 实际 kernel 对照

这是 `04_gemm` 的独立优化调查，未修改 `03_gemm`、共享 CK 头文件、SGLang 正式模型或系统库。目标仍为代表 shape 接近 rocBLAS 的约 330 TFLOPS；本报告不把调查或一个正确但慢的原型称为达标。

## 参考依据及迁移边界

KernelWiki 固定快照：`76d27b56f804e7e7295d4c570e1e5d7eef4b0a75`，知识截止 2026-04-27。以下页面对 NVIDIA 实现的描述为 `source-reported`；迁移到 gfx936 的判断须由本机编译、ISA、正确性和未插桩 HIP event 实测验证。

| Wiki ID / 文件 | 可借鉴的算法思路 | 海光上的边界与本次动作 |
|---|---|---|
| `technique-pipeline-stages` / `wiki/techniques/pipeline-stages.md` | 分阶段搬运、生产/消费缓冲、避免提前等待未来数据 | 不照搬 TMA/mbarrier；此前双 LDS 缓冲导致寄存器溢出或回退，阶段数必须受本机资源约束 |
| `technique-warp-specialization` / `wiki/techniques/warp-specialization.md` | 分离搬运与计算角色 | 不照搬 32-lane warp、TMEM 或单线程 tcgen05；本机 ISA 已确认 64-lane wave 分工，尝试 8 个计算 wave + 4 个加载 wave |
| `technique-swizzling` / `wiki/techniques/swizzling.md` | 按消费者 lane 访问重排共享存储，减少冲突 | NVIDIA 128B swizzle 不是本机必须条件；padding、pack-XOR、Kpack-major 已在 gfx936 实测，各 shape 最优不同 |
| `technique-register-budgeting` / `wiki/techniques/register-budgeting.md` | 核实累加器/操作数的存活区间和溢出 | 不直接移植 maxrregcount；读取实际 DTK 编译器资源和可用选项，禁止凭 NVIDIA 寄存器表推驻留率 |

原始参考：[CUTLASS efficient GEMM](https://docs.nvidia.com/cutlass/latest/media/docs/cpp/efficient_gemm.html)、[tcgen05 教程](https://gau-nernst.github.io/tcgen05/)、[固定版本教程代码](https://github.com/gau-nernst/learn-cuda/tree/3b90ac9b/02e_matmul_sm100/)、[CUTLASS Hopper warp-specialized 示例](https://github.com/NVIDIA/cutlass/blob/main/examples/48_hopper_warp_specialized_gemm/48_hopper_warp_specialized_gemm.cu)。本文不引用其 NVIDIA 性能数字作为海光收益。

Wiki 中 `if (warp_id == producer) ... else if (warp_id == consumer) ...` 的角色分工模式可以参考；其中 TMA 到达 mbarrier、TMEM 累加器和 tcgen05 指令不能直接用于 gfx936。不能把“增加流水阶段总是有益”的示意描述当作硬件保证，本次双缓冲回退就是反例。

## 实际 rocBLAS 二进制归属

环境：`ck_yyc`，HCU4 / gfx936，PyTorch 2.9.0，HIP 6.3.26113；小 FC2 `[M,N,K]=[18944,5376,7168]`，BF16 输入/权重/输出、FP32 累加。此前 API trace 确认 `rocblas_gemm_ex`，不是仅凭 `_BlasBackend.Cublas` 枚举名或加载 hipBLASLt 推断。

独立 `inspect_rocblas.py` 执行一次 F.linear，保存 `/proc/self/maps`。HSA 加载后 CO 不一定仍 mmap，因此再按已采集 dispatch 的精确 kernel 符号搜索文件：

```
/opt/dtk/lib/librocblas.so.4.3
/opt/dtk/lib/rocblas/library_gpu5/
  TensileLibrary_Type_BB_HPA_Contraction_l_Alik_Bljk_Cijk_Dijk_gfx936.co
```

CO SHA256：`415bf79aa50f44e603add9ceb458e90dd1294da9b2da1728277f6c558406b635`。精确 dispatch 对应 `...MT256x256x16...EPS1...OLDSR0_PGR1_PLR1_RK1...VAW1...WGM8`，完整符号见 `results/library_reference/attribution.json` 及等待计数 CSV。不是另外三个名字相似的候选。

可用解码器为 `/opt/dtk/aillvm/bin/llvm-objdump --mcpu=gfx936`。`dccobjdump --show-sass` 输出文件头不代表解码成功。该手写汇编的 ELF 符号 size=0，`--disassemble-symbols` 在局部标签处停止；脚本改用当前 kernel 起始地址至下一全局 kernel 的区间，保留其内部循环。目标地址范围 `0x1791300..0x17a2f00`，完整汇编见 `*.co.2.isa.txt`，具体终点以 attribution 中十进制地址为准。

## ISA 能直接确认的差异

1. **wave 分工**：`threadIdx/64 → s73`，`s_cmp_le_i32 s73,7` 跳转 `MmacWave`；其余 wave 执行 `Prefetch_Begin`。结合实际 dispatch 的 768 threads，可确认 8 个计算 wave 和 4 个搬运 wave，而不是仅从 AMAS3 名字猜测。
2. **A 直接搬到 LDS**：搬运分支包含 `buffer_load_dwordx4 ... lds`，不是通常的 global→VGPR→LDS 两段路径；用 `m0` 和偏移切换 LDS 区域。
3. **B 进入计算 wave 的寄存器**：计算分支有 `buffer_load_dwordx4 v[192:195] ... idxen offen`，并被后续 MMAC 用作第二操作数，省去这部分的 LDS 写入/读取。
4. **操作数复用及密集 MMAC**：一组 `ds_read_b128` 获取 A；同一 B fragment 被连续 16 条 `v_mmac_f32_16x16x16_bf16` 复用；多组累加器分布在 `v[0:127]`。可确认调度形式，不据此单独量化收益归因。
5. **显式调度/同步**：计算段有 `s_setprio 1/0`、`s_barrier`、`s_waitcnt`，A 的 LDS 偏移在两片区间切换。不是没有 barrier，而是让加载工作与 MMAC 在不同 wave 进行。

示意摘录（来自本机反汇编，不是可直接执行的完整 kernel）：

```asm
s_cmp_le_i32 s73, 7
s_cbranch_scc1 MmacWave
; loader wave
s_mov_b32 m0, s60
buffer_load_dwordx4 v224, s[8:11], 0 offen lds
; consumer wave
ds_read_b128 v[128:131], v242
s_waitcnt lgkmcnt(0)
v_mmac_f32_16x16x16_bf16 v[0:3], v[128:129], v[192:193], v[0:3]
s_setprio 1
v_mmac_f32_16x16x16_bf16 v[4:7], v[132:133], v[192:193], v[4:7]
```

CK 已有 `core/arch/hcu_buffer_addressing.hpp::hcu_async_buffer_load_asm_impl<T,N>`，支持 4/8/16 字节 `buffer_load_* lds`，因此可以沿用海光接口而不杜撰 NVIDIA 搬运指令。LLVM 的 [DMA 文档](https://www.llvm.org/docs/AMDGPUDMAOperations.html) 和 AMD 的 [direct-to-LDS 说明](https://rocm-handbook.amd.com/projects/amd-rocm-optimization-guide/en/latest/compiler-builtins/cross-arch/direct-to-lds-builtins.html) 可解释通用机制，但 AMD SKU 支持矩阵不是 gfx936 的证明；本机库 ISA 和 CK 海光头文件才是此处硬件可行性的证据。

## 非 emulated 等待计数对照

`results/stage_waits/metrics.csv`，同一 FC2、各 3 次 dispatch 平均。下面是跨 wave 汇总的工具原始等待量，**不是 ms，不可相加当墙钟时间，不可当 E2E 占比**。插桩时间不用于性能排序。

| 实现 | VM wait | LGKM wait | barrier wait | LDS issue wait |
|---|---:|---:|---:|---:|
| 原 CK base | 3,185,775,342 | 3,956,723,235 | 617,381,080 | 211,894,205 |
| wide_mr4_pad16_g8 | 69,459,502 | 333,834,962 | 491,659,309 | 200,458,923 |
| wide_kpack8_g8 | 345,332,872 | 329,204,137 | 1,604,984,875 | 785,639,751 |
| rocBLAS | 98,547,813 | 157,585,503 | 472,730,486 | 55,438,400 |

结论：当前较好的 CK 候选不再表现为原始版那样的巨大 VM wait，但仍有更高的 LDS 相关等待；wave 数、tile、指令工作分配不同，不能机械用表中数值推算性能倍数。此前 MMOP/LDS 指令及 bank conflict 条目在本机 catalog 标为 emulated，另外保留，但不把它们当独立测得的 bank stall 时间。

## 新原型与安全约束

`specialized.cpp` 采用 CK BF16 MMAC primitive，独立编写 8 消费 + 4 生产 wave、A 经 LDS/B 直接寄存器的原型。不是 rocBLAS 调用包装，也没有复制库二进制作为 CK 优化结果。

第一版 `specialized` 全部四种投影的计算检查通过，但只有约 4.6 TFLOPS，不能采用。ELF 资源确认 VGPR=84、spill=527、scratch=800 bytes；768-thread 的编译分配限制导致大量累加器溢出。仅改变分工不足以获得库性能。

O轮已核实 DTK 可用的 `--enable-num-vgprs-768=true` 编译选项和实际 ELF 分配：原型 VGPR=218、spill=0、scratch=0，吞吐恢复约132 TFLOPS，但仍低于已优化通用模板。`large_pad16_g8_reg768` 则得到 VGPR=202、spill=0、scratch=0，四投影正确性通过；FC2 7.7728ms /187.84 TFLOPS，QKV 11.8714ms /184.48，out_proj 4.4119ms /165.46，FC1 15.8417ms /184.33。同轮 rocBLAS 约326–333 TFLOPS。此前大tile慢主要是spill，不能据早期回退认定大tile本身不可行。

该选项由本机编译器 `--help-hidden` 报告“num of vgprs depend on hardware”，完整帮助保留在 `build/compiler_options.txt`；不手工伪造资源元数据，不改变全局运行库。原型使用不同 mainloop，所以它与大tile模板的速度不能只归因于寄存器数量。下一轮再验证 CK 现有 direct-to-LDS 接口、消费者 LDS 布局和分阶段 MMAC 调度。

P–S轮已完成：大tile更多布局/双缓冲、独立原型的padding/Kpack/direct-to-LDS，以及buffer-load和s_setprio，均未胜过约188TFLOPS的通用候选。direct-to-LDS原型完整四投影检查通过，但约117–123TFLOPS，不能宣称“沿用同类指令便复现了库性能”。S轮的BN512不适用FC2/out_proj，已明确排除这些shape的计时；初次失败记录见STAGE_S_INITIAL_FAILURE.md。

T轮只做了编译器调度选项验证，两候选在DTK Machine Instruction Scheduler内崩溃，**没有GPU性能结果**；见STAGE_T_COMPILER_FAILURE.md。保留默认调度器，不升级工具链。

最快候选追加非emulated等待采集，`results/stage_waits_large/metrics.csv`，各3次平均：

| 实现 | VM wait | LGKM wait | barrier wait | LDS issue wait |
|---|---:|---:|---:|---:|
| large_pad16_g8_reg768 | 13,282,499 | 105,639,714 | 410,851,457 | 243,471,555 |
| 同轮rocBLAS | 100,370,900 | 157,794,363 | 475,346,627 | 54,916,435 |

它的VM/LGKM/barrier汇总值已不比库大，但LDS issue wait明显更高；这只是定位线索，不能将原始wave等待计数当墙钟占比。ISA同时确认大tile候选的一组B fragment已经被复用到多个M片段，不能仅看到源码内嵌load_tile就认定每次都重复执行LDS load。下一步优先研究有效LDS访问/发射流水与计算wave配置，不盲目继续增加global预取阶段。

U轮补做真实 M=18880：256×256候选开启CK原生M padding后，四投影计算及输出guard通过，FC2约187.4TFLOPS；不同M分开计分。384×256/12数学wave候选因B搬运均匀分区整数截断、漏覆盖而失败，未计时；04私有policy已增加整除断言。见STAGE_U_CORRECTNESS.md。

所有性能数字仍需最终独立复测及 H3 容器确认；330TFLOPS目标尚未达成。通用 CK 模板更易维护；如果需要显式 wave 分工的私有 mainloop 才能接近库性能，要明确实现复杂度和适用 shape，不能把固定 shape 实验称为通用正式后端。
