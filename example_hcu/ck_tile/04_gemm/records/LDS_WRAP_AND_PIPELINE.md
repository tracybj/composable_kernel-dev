# 第二阶段：LDS wrap、分工流水与算术隔离调查

目标不变：在04_gemm中让实际BF16 GEMM接近rocBLAS约330TFLOPS。**截至本报告的V–Z轮，最好实际GEMM仍约188TFLOPS，不能用下述算术探针代替达标。**

环境：ck_yyc，物理HCU4，gfx936，DTK25.10.0-0，PyTorch2.9.0/HIP6.3.26113；BF16 A/W/C、FP32累加。实际GEMM四shape与RESULTS.md一致，M=18944筛选，另有上一轮M=18880通过记录。性能为5次预热、6轮轮换顺序×每轮10次HIP event的轮均值中位数，不用profiler时间。

## 1. 不猜m0 wrap规则，先做功能探针

`lds_wrap_probe.cpp`调用CK已有`hcu_async_buffer_load_asm_impl`，4个64-lane加载wave，将256×64个带唯一编号的uint16值写入LDS。分别不wrap/启用wave对应wrap，随后普通LDS读取到输出。不是GEMM、不用于性能宣称。

两种设置均保留全部16384个唯一值，无缺失、无重复，最大物理BF16索引16383。启用wrap后得到以下实测地址公式（单位为BF16元素）：

```text
p = row % 4
index = (row // 32) * 2048 + p * 512
      + (((row % 32) // 4) * 64 + col + p * 16) % 512
```

`verify_wrap_formula.py`将所有16384个(row,col)逐一与独立硬件探针结果比较，通过。不是仅用少量例子猜公式；vec8访问仍保持对齐，跨512元素边界的回绕也覆盖。原始映射及源/库SHA在`results/lds_wrap_probe`。

这说明rocBLAS的m0高位不是普通地址加法。之前仅启用direct-to-LDS而使用通用Kpack布局，未复现其wrap布局。该证据来自实际gfx936，不由NVIDIA/AMD其他SKU的bank规则推导。

## 2. V轮K64主循环

`role64.cpp`使用CK BF16 MMAC primitive、CK已有HCU direct-to-LDS；256×256输出tile，8个计算wave＋4个加载wave。B直接加载到计算寄存器；K64分为两个K32 MMAC阶段，后半A已经读入寄存器后才允许加载wave覆盖释放的LDS。

| FC2候选 | ms | TFLOPS | 状态 |
|---|---:|---:|---|
| K64单缓冲、不wrap | 17.6338 | 82.80 | 正确，慢 |
| K64单缓冲、wrap | 9.7587 | 149.61 | 正确 |
| K64双缓冲、wrap | 9.0078 | 162.08 | 正确，但未超过通用模板 |
| 同轮通用CK大tile | 7.7707 | 187.89 | 仍较快 |
| 同轮F.linear | 4.4715 | 326.52 | rocBLAS控制项 |

四种投影均通过二进制分数精确、普通随机误差及输出guard检查；不宣称任意输入bitwise无损。双缓冲占64KiB LDS，237 VGPR、无spill。两阶段同步中，不在READY提前等待未来A，而在后续ACK之前确认未来加载完成，以保护缓冲复用。

## 3. W轮：MMAC发射与初始化，不把同哈希当收益

- 用volatile inline asm发射同一MMAC指令，生成的库SHA与原始K64候选完全相同（`3f7b081...`）。明确排除重复计时，不作为新优化。
- ASM+立即数零累加器在本机汇编器报`invalid operand`，没有GPU测试。原失败冻结源/命令/日志保留。
- 首步用CK primitive返回值初始化累加器，正确性通过、无spill，但FC2约135.15TFLOPS，低于162.09TFLOPS对照。ISA寄存器排列有所改变，却不能因此称对齐更好便更快；初始化和调度合并变化不是严格的单一对齐归因实验。

## 4. 算术隔离探针：确认不是MMAC本身只能188

`mmac_micro.cpp`只加载一次A/B寄存器片段，在主循环重复MMAC，再输出FP32。**不是实际GEMM、不是模型推理，不能用于完成330TFLOPS目标。**

共同工作量：1554 blocks，每block 8个数学wave，每wave/iteration 64条16×16×16 BF16 MMAC，224次iteration；指令FLOPs=1,460,020,445,184。ISA确认主循环含64条MMAC，VGPR=202、spill=0、LDS=0。1/8输入产生全部FP32输出112.0的检查通过。

| 探针launch | ms | 算术指令有效TFLOPS | 解释 |
|---|---:|---:|---|
| 512 threads / 8计算wave | 3.16783 | 460.89 | 没有LDS主循环搬运，不是GEMM成绩 |
| 768 threads / 8计算＋4空闲wave | 3.62437 | 402.83 | 工作量相同，launch/资源结构不同 |

原始计时轮次、计算量、正确性、环境/源/库SHA在`results/mmac_arithmetic_probe/results.json`，完整ISA/metadata在`build/isa_mmac_micro_*`。不能以单一探针数字当SKU理论峰值或H3 MFU。

## 5. X轮：8个计算wave协作预取

数学wave分担direct-to-LDS搬运，取消4个专门加载wave。仍保留完整A/B计算与同步，采用wrap/不wrap、单/双缓冲对照。全部四投影正确性及guard通过，但FC2协作wrap双缓冲152.82TFLOPS、单缓冲155.33，不wrap85.09；没有超过162.10的分工原型，更没有超过187.77的通用模板。不能从算术探针512-thread较快直接外推GEMM也快。

## 6. Y/Z轮：结构化buffer cache swizzle与等待跟踪

在本机CK头文件发现`make_wave_buffer_resource(ptr,size,const_stride)`：以行数作为range、将byte stride放入descriptor高位并enable cache swizzle。与rocBLAS的`idxen offen`结构化路径一致。该机制与前述LDS wrap是**不同优化点**。

[LLVM 17 intrinsic定义](https://raw.githubusercontent.com/llvm/llvm-project/llvmorg-17.0.6/llvm/include/llvm/IR/IntrinsicsAMDGPU.td)明确区分raw/struct及idxen相关的bounds/swizzle行为。本文的海光可行性由本机CK接口及实测提供，不把LLVM支持视为性能保证。当前候选限制byte stride<0x4000，覆盖这四个H3代表shape，不是通用任意K实现。

- Y轮：独立A、B及AB启用结构化cache。结果全部正确，但A约157.43、B142.11、AB137.16TFLOPS，未胜过raw对照162.06。B使用CK inline asm异步load后提前全等待，不能把这组差额只归因于cache swizzle。
- Z轮：将B等待延至复制/消费前，并加编译调度屏障保护；另声明LLVM17结构化load，让编译器跟踪数据等待。B late-wait FC2约164.37TFLOPS，LLVM路径161.60，AB LLVM155.03；全部四投影检查通过，仍不胜过通用模板187.85。
- 不删除异步数据依赖所需的wait；把一个没有错误的快结果归因于“通常加载已完成”不是安全优化。所有失败/同哈希/回退均保留为证据。

## 7. AA轮方向及参考

下一轮回到通用CK大输出tile，测试BK16/WarpK16 + 扩展寄存器池和适合DS64的布局。意图是减小输入寄存器和LDS工作集，探索更多计算驻留；不能预先保证两block驻留或收益。构建/计时/正确性完成状态见RESULTS.md；初次名称解析失败见STAGE_AA_BUILD_RECOVERY.md。

KernelWiki参考`technique-double-buffering`（`wiki/techniques/double-buffering.md`）、`technique-pipeline-stages`及`technique-warp-specialization`，对NVIDIA实现的置信度为source-reported。可借鉴示意`stage = tile % NUM_STAGES`与缓冲读完再释放的生产/消费规则；不移植TMEM、TMA、mbarrier、32-lane warp或NVIDIA性能数字。原始参考：[CUTLASS efficient GEMM](https://docs.nvidia.com/cutlass/latest/media/docs/cpp/efficient_gemm.html)、[固定教程代码](https://github.com/gau-nernst/learn-cuda/tree/3b90ac9b/02e_matmul_sm100/)。实际候选均是gfx936上的独立实验。

03_gemm、共享CK include、SGLang正式环境未改，未commit/push。当前目标未完成；文件及实测证据用于继续优化，不是将算术探针替代真实GEMM的成功标准。

## 8. AA/AB/AC补充结果及关键角色澄清

- AA工作集确实变小：Kpack4/BK16候选LDS16KiB、VGPR166、无spill，但FC2约116.33TFLOPS；其他BK16布局也回退。不能仅从资源更小声称实际驻留或性能一定提高。
- AB保留全部GPU数据同步，只测试priority与编译sched barrier。取消二者的原型约170.36TFLOPS，其他设置约162–165，全部四投影正确但未胜过通用模板。
- 原始`backend_probe_20261012/rocblas.log`明确为`rocblas_gemm_ex,T,N,N_features,M_tokens,K,...`。例如FC2的BLAS m=5376、n=18944、k=7168；输出是列主序Feature×Token，等同PyTorch行主序Token×Feature。库中的**A是权重W，B是输入X**，而不是我们此前原型中的A=X/B=W。不要把库符号A/B直接当成模型接口的输入/权重名称。
- AC原型改为内部计算W@X.T并转置直接写回原C地址，不额外启动transpose kernel、不改变模型数学计算。Feature/Token CTA分组也分别对照。
- AC同轮FC2最好转置原型约182.07TFLOPS、QKV184.57、out_proj172.71、FC1 184.68；四投影正确，out_proj超过约165的通用模板，但不是整个目标已达成。单一变换同时影响数据角色、缓存/写回、地址指令和分组，不能把全部收益单独归因某一项。
- 当前容器额外单次BLAS归属核实见`probe_orientation.py`和`results/blas_orientation`；它只用于核对m/n及地址对应关系，不替换无日志正式计时。转置原型的M=18880尾token和输出guard验证见单独AC native阶段，完成状态与结果以JSON/RESULTS.md为准。
- 当前ck_yyc日志和inputs指针已严格逐一匹配：`rocblas_gemm_ex,T,N,5376,18944,7168`的第一个输入地址等于W、第二个输入地址等于X、输出等于Y，`analyze_orientation.py`通过。不是仅依据另一容器的旧记录猜测。
- AC原生M=18880转置候选四投影及输出guard通过：FC2约181.53、QKV184.13、out_proj172.45、FC1 184.36TFLOPS；out_proj相对同轮通用模板约164.97有收益，FC2仍不胜过通用模板约187.47。未改变330目标，未把不同M的ms混入同一最快榜。
- AD继续把相同数学方向变换迁回完整CK模板，采用ColumnMajor C view直接写回原行主序Y，不额外启动转置算子；按N分组公式有CPU覆盖测试，实际数值/guard仍须GPU确认。构建与测试状态见独立AD阶段。

## 9. AD/AE回退根因与AF向量加载修复（2026-10-13）

AD四投影正确但只有约50–54TFLOPS，LDS49088 bytes、VGPR211、scratch/spill=0，并非寄存器spill造成。AE用额外活跃变量将MMAC操作数起点分别移为奇数和偶数，吞吐仍约50TFLOPS；不能将回退归因于寄存器起点奇偶。

实际源码证据在共享只读`gemm_pipeline_problem.hpp`：B为ColumnMajor时，`VectorSizeB = kPadN ? 1 : GetAlignmentB()`。我们的B物理形状为连续[N,K]，尾部发生在N而非连续K方向，K仍整齐且基地址16字节对齐。该通用启发式将原本合法的vec8读降为scalar。反汇编静态计数显示原转置候选32条`buffer_load_ushort`、4条`buffer_load_dwordx4`；修复后分别0和8。计数含prologue与loop，不是动态执行次数。

AF仅在04私有派生Problem中恢复`VectorSizeB=8`，保留kPadN及输出边界；API新增输入16字节对齐和K%8检查，编译期保证BK%8=0。没有改共享CK模板，也没有禁用真实尾部边界。另建kPadN=0、只允许整齐M的控制候选，两者在M18944得到接近的结果，支持向量推导是此次回退的重要原因。

真实M18880结果（`results/stage_af_native_m18880/results.json`）：

| 投影 | N / K | 旧最好通用CK ms / TFLOPS | AF修复 ms / TFLOPS | 同轮F.linear TFLOPS |
|---|---|---|---|---:|
| FC2 | 5376 / 7168 | 7.7563 / 187.60 | 7.0587 / 206.14 | 327.99 |
| QKV | 10752 / 5376 | 11.8684 / 183.90 | 10.2657 / 212.61 | 327.70 |
| out_proj | 5376 / 3584 | 4.4120 / 164.90 | 3.5826 / 203.08 | 332.60 |
| FC1 | 14336 / 5376 | 15.8239 / 183.91 | 13.6956 / 212.49 | 328.36 |

上述四类全部通过精确二进制分数、普通随机误差、finite及输出尾部16行NaN guard。修复后VGPR202、LDS49088 bytes、scratch/spill=0。仍是热缓存单kernel成绩，不是H3 E2E；尚未达到约330TFLOPS目标。

## 10. AG重新检验cache swizzle与当前实际启用情况

原Y/Z轮是在A=激活/B=权重方向上测试；AG将其放到AC确认的BLAS数据方向：A=权重W、B=激活X，内部W@X.T直接按列主序写回，不额外transpose kernel。使用真实M18880、同轮raw原型、相同priority/sched设置；B采用LLVM结构化load，保留编译器等待跟踪。

| FC2路径 | ms | TFLOPS |
|---|---:|---:|
| 对应raw role64原型 | 8.0093 | 181.68 |
| A cache swizzle | 8.1055 | 179.52 |
| B cache swizzle | 8.2465 | 176.45 |
| AB cache swizzle | 8.3756 | 173.73 |
| AF通用CK向量修复控制项 | 7.0589 | 206.14 |

其他三投影结论一致：结构化A/B/AB均正确，但都比对应raw原型慢。cache swizzle未加入当前最快候选；更不能把AF的提升归因于它。

这里有三个容易混淆的概念：

- **Buffer cache swizzle**：`role64.cpp`的H3_ROLE_CACHE_A/B实验，结构化buffer descriptor/idxen load；已写代码并测过，当前最好候选不使用。
- **CTA tile分组/调度swizzle**：`grouped_n_partitioner.hpp`按token方向分组，当前AF候选已使用GROUP_N=8；并不是开启buffer cache swizzle位。
- **LDS布局**：role64的m0 wrap已经硬件逐元素验证；通用AF最快路径采用padding16的私有LDS policy，不是role64 wrap。三者都不是NVIDIA TMA swizzle的直接移植。

源码定位：`gemm.cpp`私有Problem、`build.py`候选与编译旗标、`role64.cpp`的resource_a/read_b/stage_a，以及原始stage_af*/stage_ag*数据。正式H3代码没有换用这些BF16实验GEMM。

## 11. AH：分组与宽token tile

真实M18880、当前AF向量修复作为同轮控制；GROUP_N为4/8/16/32。FC2以group4略好，6.9789ms / 208.50TFLOPS，对照group8为7.0550ms / 206.25TFLOPS。其他投影仍group8略好；差别不应当成所有shape的稳定通用收益。group16/32和128×512宽token tile没有更快。四投影全部数值和guard通过，CPU分组公式覆盖360种grid/group组合及不整组尾部。

## 12. AI：K32相位预取缩短寄存器生命周期

独立`rolephase.cpp`保留K64权重direct-to-LDS双缓冲和已验证wrap，改变激活B的寄存器预取：只持有当前及下一个K32相位，而不是当前和下一个完整K64 tile。每个block仍8计算wave＋4加载wave，输出256×256、FP32累加、BF16 RNE，不改变GEMM数学计算。A最后半块已读入寄存器后才ACK并允许加载wave覆写，raw-buffer LLVM intrinsic保留数据依赖等待；不靠取消同步提升。

比较1/2/4个feature方向计算wave分区及priority。统一输出量与128条静态MMAC的主函数清单；静态指令数不是动态执行次数。

| 计算wave分区 / FC2 | VGPR | LDS read b128静态数 | ms | TFLOPS |
|---|---:|---:|---:|---:|
| 旧K64 role64 / 1×8 | 248 | 32 | 8.0126 | 181.60 |
| K32 phase / 1×8 | 225 | 32 | 6.4423 | 225.87 |
| K32 phase / 2×4 | 215 | 16 | 8.7197 | 166.87 |
| K32 phase / 4×2 | 247 | 8 | 15.8898 | 91.57 |
| K32 phase / 4×2 + priority | 247 | 8 | 15.8785 | 91.64 |

全部scratch/spill为0、LDS64KiB，四shape全部正确且尾guard通过。减少LDS读取不自动意味着更快：2×4/4×2分区逻辑上重复加载的激活tile更多，分别由2/4个feature wave组请求；实际HBM流量还取决于缓存，尚不能把逻辑请求次数直接当作HBM计数或唯一回退原因。新1×8胜出没有减少LDS read静态数，明确不能将它的收益宣传为“LDS读取次数减少”。较短预取、资源和指令调度共同变化；225 vs 248 VGPR是证据，不单独证明驻留率提升。

AI真实M18880同轮结果：

| 投影 | N / K | 新phase ms | 新phase TFLOPS | AF控制TFLOPS | F.linear TFLOPS |
|---|---|---:|---:|---:|---:|
| FC2 | 5376 / 7168 | 6.4423 | 225.87 | 206.75 | 326.34 |
| QKV | 10752 / 5376 | 9.5414 | 228.75 | 212.69 | 327.94 |
| out_proj | 5376 / 3584 | 3.4236 | 212.51 | 202.92 | 332.11 |
| FC1 | 14336 / 5376 | 12.7138 | 228.90 | 212.31 | 327.21 |

约213–229TFLOPS是实矩阵GEMM成绩，并非此前只重复MMAC的算术探针；仍未达到约330目标。正式H3入口未替换。所有实验源/二进制/命令冻结于build/attempts；JSON在results/stage_ai_phase_register_prefetch，资源清单在results/phase_isa_analysis.json。

AJ进一步将结构化cache swizzle移入此新phase流水并保持同轮raw控制，实际结果以独立stage_aj_phase_cache_swizzle JSON和最新RESULTS.md为准，不因AG旧流水无收益就假定新流水一定无收益。

AJ已完成，四投影全部通过原有数值/finite/尾guard。真实M18880 FC2：raw 6.4438ms / 225.81TFLOPS；A cache 6.5974ms / 220.56TFLOPS；B cache 6.7902ms / 214.29TFLOPS；AB cache 6.9309ms / 209.94TFLOPS。其他三投影也都没有更快。因此最新phase最快方案仍不启用buffer cache swizzle，保留实验开关和证据，不删回退结果。AJ的raw复测QKV228.79、out_proj212.70、FC1 228.90TFLOPS，与AI大致一致，不用某次异常最快样本代替稳定表现。

当前迭代概括：旧通用候选约188TFLOPS → AF向量修复约203–214 → AI/AJ相位预取约213–229；不是全部优化简单相加，也不是H3 E2E加速百分比。距离同轮rocBLAS约327–332TFLOPS仍有差距，目标保持active。

## 13. AK：寄存器双bank与显式局部等待

新候选`rolephase_m1_bank_asmb_reg768`不把future复制到current，改为两个固定bank轮换。用CK raw-buffer asm发出每相位NT条16-byte load；当前1×8分区NT=2。先发出更年轻的future bank，再`vmcnt(2)`等待较老current bank完成；最后相位没有future时使用`vmcnt(0)`。compute wave不发producer的权重VMEM，计数依据是该wave的已核实load序列；不能把阈值2随意用于其他kernel/加载序列。MMAC前编译调度屏障保留，跨wave的LDS READY/ACK也保留。

源码在`rolephase.cpp`的H3_PHASE_BANK/ASM_B分支。本机ISA确认两处vmcnt(2)及随后MMAC，VGPR223、LDS65536、spill/scratch=0。静态MMAC128和LDS read32不变，waitcnt静态站点由28降至19；静态站点数不是动态停顿时间。更改priority反而慢，LLVM普通双bank也回退，因此收益不能只归因于“使用双bank”。

真实M18880，AK未插桩HIP-event中位数：

| 投影 | 新bank/asm ms | TFLOPS | 原phase TFLOPS | 同轮F.linear TFLOPS |
|---|---:|---:|---:|---:|
| FC2 | 6.0861 | 239.08 | 225.71 | 327.87 |
| QKV | 9.1013 | 239.81 | 228.84 | 327.65 |
| out_proj | 3.2694 | 222.53 | 212.73 | 332.57 |
| FC1 | 12.1351 | 239.82 | 228.98 | 326.54 |

AL同轮重复控制的对应TFLOPS为239.03/239.78/222.41/239.80，基本一致。用最新含K32实验的源重新编译后，最佳库SHA仍为`d89f4906cd47339c8ccb9d920f3d22c5d05c1bf8479e7d26bd8cffa59cb207fc`，原phase库仍为`3ef9c0533b24267a260d33fbf4e8c3db110d6e03a85e8e41b01ac464540fc64b`；不是因源码和库不同版得到比较。

独立`stress_phase.py`审计通过：12种尺寸×3种seed×二进制分数/普通随机两分布=72组，每组3次重复共216 launches。包含M=1/63/64/255/256/257/511/513及四个真实H3 shape，K覆盖64/128/256/512/768/1024及真实3584/5376/7168。精确数据逐元素等于FP32参考转BF16；普通随机沿用误差门槛；全部finite、输出16行NaN guard、输入不变、同输入重复输出bitwise一致。不能宣称任意浮点输入与参考bitwise一致。证据在`results/stress_ak_banked_wait_20261013`。

## 14. AL：32KiB权重缓冲不能只凭资源更小称更快

K32 wrap布局先用独立硬件probe验证全部8192元素，公式为`(r//64)*2048+(r%4)*512+(((r%64)//4)*32+c+(r%4)*16)%512`；原始结果在`results/lds_wrap_probe_k32`。随后在rolephase.cpp用H3_PHASE_WEIGHT_K=32，权重双缓冲减到32KiB，但每K64数学轮增加LDS READY/ACK。真实四shape均正确，最好仅约176–188TFLOPS；优先级也没救回，不采用。

实际最佳K32资源VGPR227、LDS32768、无spill，而原K64最佳VGPR223、LDS65536。静态barrier站点由4变6，不代表动态执行次数只增加50%。资源变小同时改变同步与流水粒度，不单独证明实际驻留更高。

`inspect_reference_resources.py`按精确dispatch全符号匹配元数据，核实rocBLAS为**LDS65536、VGPR255、threads768、scratch/spill=0**（`results/library_reference/exact_resources.json`）。因此不能将当前与rocBLAS的差距归结为“我们64KiB而库32KiB”。此前若从tile/符号猜出32KiB，应以该实际ELF证据为准。

## 15. 剩余差距：新的等待计数对照

`run_wait_banked.sh`在HCU4，真实FC2 M18880/N5376/K7168，分别采原phase、bank/asm、F.linear各3次，共9个过滤后的dispatch。本机catalog确认这些项目为per-SIMD wave等待聚合、单位4cycles、nondeterministic；不是独立的kernel ms，不可相加或转成E2E占比。

| 工具原始均值 | 原phase | bank/asm | rocBLAS |
|---|---:|---:|---:|
| VM counter等待 | 364,235,430 | 115,240,695 | 103,968,635 |
| LGKM counter等待 | 149,585,207 | 148,111,726 | 157,469,510 |
| barrier等待 | 717,456,821 | 637,834,360 | 472,496,928 |
| LDS发射等待 | 114,047,738 | 155,857,650 | 54,784,041 |

原phase VM样本范围197,560,758–697,040,913，变动大，不能把均值下降69%宣传成稳定收益；profiler也改变运行。bank/asm VM范围114,628,079–115,563,499，与库94,710,244–116,261,416同量级。LDS发射和barrier项目提示下一步应重点核查MMAC/LDS指令交错及生产消费节奏，而非继续只优化B的global等待；这仍是推断，需要下一轮实验验证。性能排序仍使用AK/AL的未插桩数据，不用profile duration。

采集脚本、全部CSV/log和CPU解析见`results/wait_phase_20261013`与`analyze_banked_wait.py`；精确源/库冻结记录保留。当前真实GEMM约223–240TFLOPS，目标仍是接近约330，不调整成功标准，不commit/push、不修改正式H3。

## 16. AM–AO：生产消费、LDS读取顺序和寄存器分配对照

AM把加载wave的下一块A加载推迟到READY后、ACK前，再等待完成；数值通过，但FC2约225.88TFLOPS，落后同轮239.07。仅上半组计算wave交换K32相位约239.37，差别很小，不作为确定收益。rocBLAS两个计算wave组具有不同调度，不能因此假定它们交换K相位。

AN强制ds_read_b128按消费顺序发射，并测试完整/逐渐降低LGKM等待阈值及MMAC后priority：全部正确，但FC2约207.83–225.97，未胜过约239控制项。AO额外保持1/2/3个VGPR活跃约238–239，并未改变第一条MMAC的操作数寄存器起点，因此不构成“所有寄存器对齐策略都无效”的证据。保留ISA和负结果，不采用这些候选。

## 17. AP–AT：交错预取与去掉热循环条件

AP将计算wave每相位的16次权重LDS读拆成8次 + 下一相位B预取 + 8次，其余数学计算和READY/ACK不变。FC2由约239升至246.59TFLOPS；只在上半组wave这样交错反而回退。结构化buffer cache A/B/AB仍未胜过raw，最新快路径没有启用cache swizzle。

AQ继续尝试每相位都发出下一次B预取，包括最后一次数学相位结束后的无用预取，从热循环去掉条件判断。终末预取由有界buffer保护；为避免尚未完成的VMEM在写回过程中覆盖复用VGPR，保持B bank活跃直到末尾，再vmcnt(0)及编译调度屏障后开始输出。不能只删掉分支而省掉终末等待。AQ的unroll2候选与always候选实际库SHA完全相同，不算独立优化；bench和汇总脚本现按实际SHA去重，原始记录保留。

AR用当前源重编译并独立复测mixed候选，SHA `70c43fb3186a613064ae6967a87860c3c893b749abf663521d57fb8dcd257a7f`，FC2约246.38、QKV246.40、out_proj228.56、FC1 246.51TFLOPS，72组/216次正确性审计通过。AP冻结旧源/库哈希仍保留，不把不同哈希的审计混用。

AS组合mixed与always，AT独立复测：FC2 248.16、QKV249.38、out_proj231.52、FC1 249.55TFLOPS。对应库SHA `b58492f49d35103ddba37ad4fe441de8aaf1368e4da14dfe1666d4548744eaec`，同样独立72组/216次审计通过。组合实测，不把两个方向的百分比简单相加。追加ds_progress则回退，未采用。

## 18. AU：有界buffer输出替代global地址与尾行分支

仅改变epilogue写回路径：每个token tile构造有界输出resource，所有token偏移都放入VGPR voffset，标量soffset保持0；每个feature的固定偏移为immediate。N在API处已要求256整除，M尾行由硬件buffer边界保护。LLVM17的[AMD buffer-store定义](https://raw.githubusercontent.com/llvm/llvm-project/llvmorg-17.0.6/llvm/include/llvm/IR/IntrinsicsAMDGPU.td)明确soffset不参与边界检查，不能把尾行token偏移放入soffset。

初次模板immediate参数构建失败已保留；修正编译后，发现早期版本将部分token偏移放入soffset，**该不安全版本未执行GPU测试**。改为全部voffset后重新构建和验收，不删除前次失败证据。BF16转换仍是CK原有RNE，未改变累加和数值语义。

当前最佳库`rolephase_m1_bank_asmb_always_mid_bufferepi_reg768`，SHA `a0e23fd8c4751c2f7843e782ab7e717dcd82c1a2c021973ad78e807745a2bc8b`；实际VGPR221、LDS65536、scratch/spill=0。

| 真实M=18880 | CK ms | CK TFLOPS | 同轮F.linear ms | TFLOPS |
|---|---:|---:|---:|---:|
| FC2 | 5.694236 | 255.537 | 4.438917 | 327.802 |
| QKV | 8.449162 | 258.325 | 6.663371 | 327.557 |
| out_proj | 2.982150 | 243.966 | 2.202022 | 330.398 |
| FC1 | 11.263696 | 258.368 | 8.905777 | 326.774 |

未插桩HIP event、原有六轮轮换口径。四投影精确数据/普通随机/finite/16行尾guard通过；独立`stress_au_buffer_output_20261013`72组/216次审计通过，哈希与计时相同。不宣称任意浮点输入与rocBLAS逐bit相同，也不把258称为330目标完成。原始JSON在`stage_au_bounded_buffer_output`。下一步使用已安装xprof深度采集，不因单个计数小就断言wall-clock瓶颈已消失。

## 19. xprof与AV：counter指导方向，实测决定采用

已完成用户指定完整sections、SQTT raw及原AK/AU/rocBLAS各3次文本counter。完整采集和限制详见`XPROF_ANALYSIS.md`。当前AU的LDS发射等待聚合计数约为库3.28倍；不是kernel时间比例。xprof指令/冲突计数部分标为emulated，FetchSize的gfx936表达式遗漏EA1，不能直接用它推导完整HBM带宽。原始1.3GiB记录保留，未安装额外分析软件或修改运行环境。

据此AV验证未来B预取插入第4/12次DS读或上下wave组分别4/12（对照点8）。全部四投影数值与guard通过，但FC2约241–247TFLOPS，低于同轮AU255.46；其它投影也回退，不采用。AU库SHA不变，独立重复QKV258.20、out_proj243.91、FC1 258.04。简单错开不够；保留最佳及负结果，不把counter改善假设当作吞吐提升。

## 20. AW：限制跨相位调度和post-priority的负结果

实际ISA中，当前编译器会把下一相位DS read提前到本相位MMAC尾部；rocBLAS一条数学路径先LGKM归零、第一条MMAC后setprio1再连续数学。AW验证末尾编译调度fence、LGKM归零+末尾fence（burst）、burst+首条MMAC后priority，**没有删掉任何必要数据同步**。

| 真实FC2 | TFLOPS |
|---|---:|
| AU同轮控制 | 255.469 |
| 仅math fence | 251.029 |
| burst | 240.090 |
| burst + post-priority | 233.562 |

四投影精确/随机/finite/尾guard均通过，全部VGPR221、LDS65536、scratch/spill0，但其余投影也回退，不采用。这里不能因为库有某条指令就认定加入相同指令会更快。新源以开关全关重编译compat，二进制仍与原AU SHA逐byte相同，bench按SHA去重，证明这些隔离实验没有悄悄改变控制项。

## 21. AX：完整K64激活双bank的资源代价

新`phase_k64_body.hpp`在compute-wave作用域实现独立备选路径：每次预取完整K64激活、两个静态bank交替、每个K64仍分两个K32数学相位。比较在phase0或phase1的第8次DS read后发出未来K64的四条B load，入口vmcnt(0)保证当前bank完成，终末预取所有目的VGPR保持活跃直至vmcnt(0)。A的READY/ACK保护协议保留；奇数tile尾有明确分支。

初次编译因static_assert直接引用未定义H3_PHASE_SWAP宏而失败，未进入GPU；增加默认0后补编译，失败冻结源/log仍在build/attempts，不通过更换环境绕过。

| 真实FC2 | TFLOPS | VGPR | scratch bytes | VGPR spill字段 |
|---|---:|---:|---:|---:|
| AU控制 | 255.430 | 221 | 0 | 0 |
| K64 B phase0预取 | 228.052 | 256 | 12 | 2 |
| K64 B phase1预取 | 219.944 | 242 | 0 | 0 |

四个真实投影数值/finite/尾guard通过，但全部比AU慢，不采用。phase1没有spill也回退，不能把两者的回退全部归因于spill。更粗的激活预取粒度同时改变生命周期、等待和循环展开，并不等同复刻库的完整流水；此备选未做72组广审计，不能外推所有K/M尾部已验证。

## 22. AY/AZ：branchless BF16转换的可复现小收益

`output_rne.hpp`用整数mask/select替代原CK RNE的条件路径。有限数依旧`u+0x7fff+((u>>16)&1)`；全1指数依旧保留Inf和NaN低16位payload，不做finite-only近似或NaN canonicalization。两个表达式最终生成**相同二进制SHA**，不是两种独立获胜方案；bench自动去重。

先完成独立位模式探针`rne_probe.cpp/probe_rne.py`：所有65536个高16位×7种低16位边界，共458752模式，再加1048576个随机32位模式，合计1507328。mask/select对CK oracle零bit差异、输入不变、尾guard不变，包含正负零、subnormal、ties、normal溢出、Inf和NaN payload。不是穷举所有2^32模式，更不是GEMM吞吐测量。

当前新库SHA `f046d922a9fc1ea3c003bcb674b6424c0d42595c8b86e95204973c49f2a4f38f`。实际VGPR221、SGPR25、LDS65536、scratch/spill0，资源与AU相同。ISA静态MMAC128、DS read b128 32、buffer_store_short128均不变；s_and_saveexec由129变1、s_andn2_saveexec由129变1、s_or_b64由128变0，cndmask由128变256。是输出分支控制变成选择，不是少做矩阵乘法，也不能把静态站点减少384直接当作周期收益。证据`results/rne_isa_comparison.json`。

AY首次与AZ独立复测都更快；AZ前完成独立72组/216次GEMM审计，覆盖8个小/尾shape和4个真实shape、3seed×精确/普通随机两分布，数值、finite、输入不变、16行输出guard、同输入重复determinism全部通过。**不宣称任意浮点输入与rocBLAS逐bit相同**。

| AZ真实M18880 | AU控制ms | 新RNE ms | 新TFLOPS | 耗时降低 | 同轮F.linear TFLOPS |
|---|---:|---:|---:|---:|---:|
| FC2 | 5.694316 | 5.642340 | 257.887 | 0.91% | 326.301 |
| QKV | 8.446034 | 8.386178 | 260.265 | 0.71% | 327.618 |
| out_proj | 2.982750 | 2.935622 | 247.833 | 1.58% | 332.643 |
| FC1 | 11.272184 | 11.163952 | 260.676 | 0.96% | 327.701 |

同轮未插桩六轮轮换HIP event口径，原始结果在stage_ay_branchless_bf16_output和stage_az_branchless_rne_independent；探针及审计在rne_probe_20261013/stress_ay_branchless_rne_20261013。新最佳约248–261TFLOPS，仍未达到约330目标，也尚未替换正式H3调用。

本轮参考KernelWiki `technique-pipeline-stages`（wiki/techniques/pipeline-stages.md）、`technique-register-budgeting`（wiki/techniques/register-budgeting.md），confidence均为source-reported。只迁移“缓冲生命周期、同步与资源成本共同验证”的思路；没有迁移NVIDIA TMA、TMEM、mbarrier、32-lane warp或收益数字。[NVIDIA官方tuning guide](https://docs.nvidia.com/cuda/blackwell-tuning-guide/index.html)中的资源上限只用于界定NVIDIA背景，不作为本机HCU事实。HCU可行性和收益均以本节真实ISA/基准为证据；AW/AX负结果也说明更深预取并非天然更快。

## 23. 原生LDS映射逐元素核对：现有wrap并没有缺失

`verify_native_lds_addresses.py`从已冻结的原生ISA地址修正推导完整16×64×2×8读取，共16384个不同逻辑元素；与当前phys(r,c)逐元素一致。原生v244对phase0的lane47/63减512；v249对phase1的lane45/61及row14/15减512，与现有wrap公式的模512相同。原生m0的wave<<17与我们的(2*wave)<<16也相同；原生A resource减16 bytes、voffset加8个BF16，净地址偏置为0。

这是一项CPU地址证明，结合此前实际HCU wrap探针；不是新的原生库GPU探针或指令latency采集，也不能因此认定所有指令调度相同。源ISA SHA `02d435f9b611d5789eb35ce662c5902a2b6230bebcacc34679df8fc2519542c3`，完整地址表在`results/native_lds_mapping_20261013`。因此不再假定“还有一个未移植的LDS swizzle”就是剩余性能差距。

## 24. BA/BB：让下一块权重预取跨越完整K64数学工作

`H3_PHASE_NATIVE_CADENCE`为隔离候选开关，默认关闭。旧协议初始装入A0/A1，在每tile开始READY，第二相位A读完后ACK，producer随后发出A(tile+2)。新协议初始只装A0、compute装B0，双方分别在成对startup barrier中完成各自VMEM；producer在当前tile开始即发出其他LDS slot的A(tile+1)。第一同步点在phase0数学结束、phase1读取前，第二同步点在phase1数学结束，此前producer必须vmcnt(0)保证下一A完成。

当前A slot只有在phase1数学消费完成后才允许producer覆写；下一A在其他slot上预取。每个正K64 tile两个匹配barrier，startup另两个；K64=1以及奇数tile尾也维持匹配。没有取消global完成等待，也不使用未确认的跨wave异步barrier接口。它是借鉴原生producer/consumer节奏的HIP/CK实现，不是宣称逐指令复刻整套rocBLAS算法。单独添加post-priority约慢0.5%，不采用。

四真实shape数值/finite/尾guard通过，随后独立72组/216次审计通过：8个小/尾shape、4个真实shape、3seed×两分布、各3次，涵盖K=64的startup/terminal边界。输入不变、重复determinism通过；普通随机仍用原误差门槛，不称任意输入对库bitwise无损。新源关闭cadence开关、以maskrne_compat重编译，二进制与旧RNE基线f046d922...逐byte相同。

BB未插桩独立复测：

| M18880 | 旧RNE ms | 新cadence ms | 新TFLOPS | 耗时下降 | 同轮F.linear TFLOPS |
|---|---:|---:|---:|---:|---:|
| FC2 | 5.642739 | 5.359892 | 271.477 | 5.01% | 328.238 |
| QKV | 8.394826 | 7.960746 | 274.174 | 5.17% | 327.793 |
| out_proj | 2.935342 | 2.811238 | 258.798 | 4.23% | 333.143 |
| FC1 | 11.162288 | 10.653848 | 273.157 | 4.55% | 326.978 |

六轮轮换HIP event口径，BA首次与BB基本一致。实际资源仍VGPR221、SGPR25、LDS65536、scratch/spill0，不能将收益说成新增驻留或消除spill。

最新有效库SHA `b44b845565a273e2986622fca58f599703511796779171aa0f0f7716ac16ac47`，候选`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_reg768`。记录在`stage_ba_producer_consumer_cadence`、`stage_bb_cadence_independent`、`stress_ba_native_cadence_20261013`。当前约259–274TFLOPS，**未达到约330目标**；尚未替换正式H3。

## 25. 新counter：不能把等待计数当作独立耗时

`run_xprof_cadence.sh`与`run_wait_cadence.sh`分别顺序在物理HCU4采旧RNE、新cadence、F.linear各3次。`analyze_cadence_counters.py`核对9条事件与调用归属，保存完整均值/范围。xprof与rocprof为不同采集运行，不能组合成同一时刻的联合时间线。

| rocprof raw wait均值 | 旧RNE | 新cadence | rocBLAS |
|---|---:|---:|---:|
| VM counter | 118,038,825 | 118,169,637 | 96,819,735 |
| LGKM counter | 131,345,800 | 50,664,244 | 157,716,467 |
| barrier | 600,111,501 | 460,046,755 | 473,951,823 |
| VMEM issue | 14,988,864 | 131,418,055 | 170,039,377 |
| LDS issue | 173,169,641 | 276,018,287 | 55,949,068 |

xprof的LDS issue均值分别172,792,306 / 276,156,230 / 54,183,806，与独立rocprof趋势一致。新方案虽更快，LDS/VMEM issue等待聚合量反而上升；barrier及LGKM下降。它们是per-SIMD wave等待累计、单位4cycles、nondeterministic，不是kernel毫秒、不可相加求GPU总时间。只能说更改同步节奏后的有效重叠有所改善，不能宣称“LDS等待下降所以快5%”，也不能把barrier计数减少比例直接当成加速百分比。

xprof指令/冲突统计仍有emulated标注；本轮不使用已知遗漏EA1的FetchSize表达式判定HBM瓶颈，不引用未知峰值求MFU。完整证据在`results/xprof_cadence_20261013`、`wait_cadence_20261013`和`cadence_counter_analysis.json`。

下一个有具体源码依据的方向：原生B descriptor用2的幂stride，再将逻辑行跨度余数放入voffset；现有CK make_wave_buffer_resource(const_stride)编码完整K字节跨度。过去cacheB实验用了完整stride，两者并不相同。若继续验证pow2拆分，必须先确认record-count单位与边界，尤其M=1及终末无用预取，禁止只改高地址标志位而不校验数据地址/OOB。另一个可分离方向是producer按tile更新标量resource base，减少每条VMEM前的向量地址更新；均尚未计为完成或收益。

## 26. BC：原生式pow2 B寻址正确，但没有净收益

`structured_b.hpp`采用stride=2^floor(log2(K_bytes))，逻辑index=row，voffset=row*(K_bytes-stride)+K_offset。descriptor record count必须为真实剩余token行数，不能用总逻辑bytes/stride；末尾kk>=K的无用预取强制使用越界record，避免探针仅靠更大backing allocation掩盖越界。比较无bias与resource base减16 bytes/voffset加16 bytes，实际访问位置相同。

初次编译因CK返回thread_buffer而函数声明int32x4，缺少显式bit_cast失败，未运行GPU；旧目录保留。r1顺序标记数据、r2随机int16独立探针各30组：rows=1/63/257、K=64/128/3584/5376/7168、bias=0/16，包含逻辑尾行和K+64的无效预取，全部数据、零填充、输出guard通过，r2另核对输入不变。随机探针避免顺序16-bit数据周期重复掩盖整周期错误。源/命令/库哈希冻结在structured_b_probe_20261013_r1/r2。

GEMM使用当前b44基线，另设raw+terminal vmcnt(0)控制，防止把终末等待代价全归因cache。四投影精确/随机/finite/16行guard通过：

| BC / TFLOPS | 原b44 | raw终末完整等待 | pow2 B | pow2 B+bias16 |
|---|---:|---:|---:|---:|
| FC2 | 271.449 | 269.716 | 263.845 | 263.881 |
| QKV | 274.124 | 273.387 | 264.685 | 264.816 |
| out_proj | 258.753 | 257.676 | 252.574 | 252.488 |
| FC1 | 273.183 | 272.369 | 264.186 | 264.200 |

pow2候选实际VGPR227、SGPR26、LDS65536、scratch/spill0；不采用。此结果证明的是该完整候选回退，不是断言原生库的寻址技术普遍无用。完整记录stage_bc_native_pow2_b；并未作72组GEMM广审计，不能外推所有尺寸。

## 27. BD：标量A base和显式wave-uniform索引

A base移动方案把tile*WK*2加入resource指针、等量缩小byte bound，保留物理末端不变，voffset只留下row*K+col。显式scalar wave方案用readfirstlane将wave index固定为每wave SGPR，并用于producer/consumer角色选择和索引。两项单独及组合均实际编译、四真实shape正确性通过。

| BD FC2 | TFLOPS | VGPR | scratch/spill |
|---|---:|---:|---|
| b44控制 | 271.355 | 221 | 0/0 |
| A base移动 | 271.585 | 221 | 0/0 |
| scalar wave | 250.469 | 245 | 0/0 |
| A base + scalar wave | 250.871 | 245 | 0/0 |

A base在QKV略慢、其它基本相同，FC2约0.08%不算稳定收益，没有独立复测或采用；scalar wave其它shape也回退。初次A base构建缺少H3_PHASE_CACHE_A默认宏，冻结错误后增加默认0，不通过更换环境绕过。当前源关闭新开关重编译compat与b44二进制逐byte一致。

## 28. BE：只移动了部分MMAC操作数，不能作全面对齐结论

ISA显示b44首条MMAC的C/A/B起点是132/156/124（mod4全0），scalar-wave为133/181/97（mod4全1）。live-pad1/2/3实际把A起点改为182/183/184，但C=133、B=97不变；VGPR分别246/247/248，仍无spill、LDS64KiB。

四投影正确。FC2由scalar-wave250.48恢复至251.16/251.26/251.92，但仍明显低于同轮b44的271.43。其它shape同样没有胜出，不采用。不是“所有操作数均已对齐而仍慢”的实验，不能据此全面排除寄存器排列影响；小幅变化也不能只归因于某一端口冲突，编译调度/资源同时变化。实际首条指令和资源在`scalar_wave_first_mmac_20261013.json`，性能在stage_be_scalar_wave_livepads。

## 29. BF：两组wave错位barrier并不自动带来同样性能

继续解码原生s_cbranch_scc1指令000001791C74（目标000001792744），发现另一计算组先读A、lgkmcnt(0)、barrier，再执行同一K相位的MMAC；原组则MMAC后barrier。两组首轮都使用phase0的A和B192，因此**不是反转K半段**。此前“不同组安排不证明K反转”的限制得到进一步具体核实。

H3_PHASE_STAGGER在native-cadence中分组实现这一区别：一组每相位MMAC后同步，另一组A片段读完并lgkmcnt(0)后先同步再MMAC。后者允许producer覆写shared slot时，A已经完整保存在private VGPR；两个组仍保持每tile两个匹配barrier。K顺序不变，必要global/LDS完成等待不省略。

四真实shape全部数值/finite/guard通过，但回退：FC2 243.29，增加post-priority后255.82，对照271.42；QKV249.64/263.35，对照274.13；out_proj240.92/256.85，对照258.71；FC1 249.14/264.06，对照273.15。两候选VGPR223、SGPR26、LDS65536、无scratch/spill。未采用，未称已复刻整套原生ISA或证明波组流水思路不可行。

当前有效候选仍b44，新报告明确区分跨轮筛选最小值与validated_current_candidate（独立BB+72组/216次审计）；不能因A base约0.1%筛选浮动就自动换库。目标仍约330TFLOPS，尚未完成。

剩余有源码依据的方向：原生数学路径在同一展开体中使用两个LDS slot的固定偏移（0/32768及64/32832）。当前每轮一个K64 tile，需要动态slot选择；旧pragma-unroll实验未生成不同库，不能代替显式双tile展开的验收。后续可保持K32 B bank生命周期，显式静态展开两K64 tile，再确认实际指令数量/资源与奇数尾K，而不是继续只按C++变量个数推测寄存器布局。此项尚未实现或计入收益。

## 30. BG/BH：显式双tile展开，保留K32 B bank生命周期

H3_PHASE_PAIR_TILES将单tile循环抽成always-inline的consume_pair_tile(tile,slot_tag)，每轮分别以编译期slot0/slot1消费两个K64 tile；奇数末尾只调用slot0。与此前AX不同，**没有扩大B预取为K64双bank**，仍每相位NT=2个K32 B寄存器向量预取、相同vmcnt/终末drain、相同producer/cadence。数学K顺序与总量不变。

实际设备ISA静态MMAC从128增至256、LDS读32增至64；两K64一轮所以动态数学和LDS访问量没有翻倍。原pragma-unroll同SHA的实验不能替代这次真实展开。资源VGPR226、SGPR27、LDS65536、scratch/spill0；较b44的221VGPR更高，不归因于新增驻留。叠加wave stagger反而约243–253TFLOPS，不采用。

BH独立未插桩复测：

| M18880 | b44 ms | 双tile ms | TFLOPS | 耗时降低 | 同轮F.linear TFLOPS |
|---|---:|---:|---:|---:|---:|
| FC2 | 5.358804 | 5.304716 | 274.301 | 1.01% | 327.086 |
| QKV | 7.962730 | 7.834146 | 278.605 | 1.61% | 328.799 |
| out_proj | 2.812918 | 2.767798 | 262.860 | 1.60% | 332.246 |
| FC1 | 10.656960 | 10.481720 | 277.643 | 1.64% | 327.149 |

BG首次与BH独立数据一致；四投影精确/正常随机、finite及16行guard全部通过。stress_phase.py新增可选--extra-odd-k，不改变旧默认72组口径：在原12shape上增加(65,512,192)、(129,256,320)、(258,512,448)、(513,256,704)、(17,512,832)，对应3/5/7/11/13个K64 tile。总17shape×3seed×2分布=102组、每组3次=306launch，全部数值、输入不变、尾guard、重复determinism通过。

当前新有效库SHA `19453374cf0ad04f191c2407e744b79125cecc05b44836e0a209413db642039f`，候选`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_reg768`。新源关闭额外aburst等开关重编译pairs_compat，仍逐byte相同。记录stage_bg_explicit_pair_tiles、stage_bh_pair_tiles_independent、stress_bg_pair_tiles_20261013。约263–279TFLOPS，未达到约330目标，不替换正式H3。

## 31. BI/BJ：减少m0设置不保证更快，必须完整声明汇编clobber

实际producer ISA每条A direct-to-LDS之前都有重新计算m0、readfirstlane及s_nop4。尝试将八次raw搬运放进一个汇编块，只读取一次初始m0，之后标量递增0x1000；另测结合A resource base移动。仍搬同样256×64权重、同一已验证wrap、保留VM/LDS完成及barrier。

初稿BI四真实shape数值正确但更慢。随后device LLVM IR审计发现：该块只有memory clobber，没有声明s_add写SCC以及m0修改，不能仅凭绿色矩阵测试认为汇编契约完整。已在源码显式加入memory/m0/scc并以新名字运行BJ，IR确实包含~{m0}/~{scc}。该版本的两库SHA与BI恰好相同，说明这次寄存器/分支安排没有产生机器码差异；不意味着以后可以省略clobber，也不声称本次已观察到SCC导致数值错误。

BJ回退：

| TFLOPS | 双tile控制 | A burst（已声明clobber） | A burst+A base |
|---|---:|---:|---:|
| FC2 | 274.309 | 270.222 | 267.527 |
| QKV | 278.630 | 272.994 | 270.276 |
| out_proj | 262.875 | 257.992 | 255.825 |
| FC1 | 277.627 | 272.028 | 269.398 |

两新方案仍VGPR226/SGPR27/LDS65536，无scratch/spill；四真实shape数值/finite/guard通过，但未做扩展102组审计、未采用。不能将减少readfirstlane/nop静态站点等同GEMM加速；整块发射还改变寻址生命周期和调度。冻结证据在stage_bi_scalar_m0_burst、stage_bj_aburst_declared_clobbers、aburst_asm_contract_20261013、aburst_declared_asm_contract_20261013。源已修复，旧初稿只保留作为实验记录。

## 32. 双tile的新xprof证据

run_xprof_pairs.sh在物理HCU4对b44、双tile、F.linear各采3次真实FC2，analyze_pairs.py核对9条事件归属并汇总静态ISA资源。指标定义版本与源/库SHA同目录冻结。

| 工具报告均值 | b44 | 双tile | rocBLAS |
|---|---:|---:|---:|
| SQ_WAVES | 18,648 | 18,648 | 18,648 |
| SQ_INSTS_VALU（emulated） | 238,420,896 | 230,029,296 | 191,041,872 |
| SQ_INSTS_SALU（emulated） | 17,815,056 | 13,712,496 | 27,738,480 |
| SQ_INSTS_LDS（emulated） | 44,556,288 | 44,556,288 | 44,556,288 |
| SQ_WAIT_INST_LDS | 276,248,714 | 286,941,333 | 53,295,788 |

当前工具报告指令量有所减少，动态LDS访问计数未翻倍，与静态展开/固定slot的方向一致；实际收益仍由BH的HIP event证明。LDS issue等待聚合反而增加，不能将其视为核函数毫秒或用多个等待相加。指令计数/emulated、每SIMD等待单位4cycles、profiling扰动等限制与前文相同，不从未知峰值推MFU。

完整证据在xprof_pairs_20261013及pair_tiles_analysis_20261013.json。仍有明显差距，需继续核查核心数学指令安排和资源/布局，不把263–279TFLOPS或减少指令数宣布为330目标完成。

## 33. BK：二维grid与token tile分组

双tile基线上分别比较token分组1/2/4/16，以及二维grid搭配分组8/1。二维grid先展平为flat，再使用同一CTA映射；改变的是dispatch grid形状和分组常量，数学不变。默认关闭新开关重编译与19453374...逐byte相同。

四投影初步数值、finite与尾guard全部通过，但未找到整体胜出的配置。二维grid+group1的FC2约277.12TFLOPS（对照274.00），FC1却约237.86（对照277.68）；单group1 FC1也降至239.40。QKV/out_proj没有稳定提升。因此不采用新分组，也不把FC2孤立收益当成全矩阵优化。原始数据stage_bk_grid_and_token_groups。

## 34. BL/BM：静态wave分组与MMAC后的priority取得新进展

旧STAGGER在每个相位中根据运行时wave组判断barrier位于数学前还是后。STATIC_STAGGER将完整消费循环封装为always-inline泛型lambda，以编译期stagger_tag生成两个循环体；wave>=4的选择只发生在整个K循环外。early-barrier组先完成全部当前A片段的LDS读取，然后允许producer复用shared slot，数学消费private VGPR；另一组数学后到达barrier。每个K64 tile仍有两个匹配barrier，K顺序、K32 B双bank及末尾drain保持原契约。

静态分组本身约252–262TFLOPS，低于原双tile。将s_setprio 1放在每相位第一条MMAC之后、数学结束复位0，与静态分组组合才得到提升；数学之前直接提升priority的候选仍约253–262，不采用。因此不能把收益全部归因于减少分支或单独priority。

BM独立未插桩复测：

| M18880投影 | 双tile控制ms | 新方案ms | 新TFLOPS | 耗时降低 | 同轮F.linear TFLOPS |
|---|---:|---:|---:|---:|---:|
| FC2 | 5.300460 | 4.902676 | 296.795 | 7.50% | 326.424 |
| QKV | 7.831818 | 7.250154 | 301.046 | 7.43% | 328.667 |
| out_proj | 2.766702 | 2.570974 | 282.984 | 7.07% | 331.882 |
| FC1 | 10.479184 | 9.707593 | 299.783 | 7.36% | 328.355 |

BL首次与BM独立结果一致。stress_bl_static_wave_20261013的17shape×3seed×2分布共102组/306launch通过：精确二进制分数、普通随机误差、finite、输入不变、尾部guard、重复determinism；包含K64 tile数1/3/5/7/11/13的奇数尾。

新有效候选`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768`，SHA `b13a73b09afbc30fb75563ffa896981b0e260ab5478d40735708047257ae2c7f`。实际VGPR230、SGPR27、LDS65536、scratch/spill0；原双tile226VGPR。设备ISA含两个静态消费体：静态MMAC512而原方案256；每个wave只走一体，实际矩阵数学工作不翻倍。保留原双tile库作对照。仍未达到约330TFLOPS，也尚未替换H3的GEMM后端。

## 35. 新counter：LDS issue等待下降，VM完成等待上升

run_profile_static_wave.sh使用物理HCU4、真实FC2，对原双tile/新方案/F.linear各3次采集。xprof文本与DTK rocprof等待counter是独立运行；analyze_static_wave.py核对9条事件归属和库SHA。ROCprof的gpu-id=12是其HSA agent编号，不能误认为物理第12张卡；xprof显示physical gpu-id4，launcher限定HIP_VISIBLE_DEVICES=4。

| rocprof等待聚合均值 | 原双tile | 静态分组+post-priority | rocBLAS |
|---|---:|---:|---:|
| SQ_WAIT_CNT_VM | 124,335,135 | 200,989,196 | 93,184,760 |
| SQ_WAIT_CNT_LGKM | 52,421,935 | 86,243,056 | 158,243,589 |
| SQ_WAIT_BARRIER | 467,905,903 | 333,811,307 | 470,837,439 |
| SQ_WAIT_INST_VMEM | 145,104,083 | 109,360,080 | 168,469,723 |
| SQ_WAIT_INST_LDS | 286,794,477 | 78,677,496 | 57,474,744 |

LDS发射等待聚合降约72.6%，barrier约28.7%；VM完成等待增加约61.7%。这些是wave等待counter，不能相加、除kernel时间得占比，不能把72.6%写为kernel加速。实际耗时降低由BM的7.07%–7.50%证明。新方案的LDS issue等待约为同轮rocBLAS的1.37倍，原方案约4.99倍；后续需同时处理数据预取，而不是只减少LDS wait。完整指标定义、文本、原始CSV与CPU分析在xprof_static_wave_20261013。

## 36. BN：LDS等待表达/读取顺序和预取点对照

比较当前新基线、builtin LGKM完成等待、显式DS asm、顺序progressive DS等待、未来B预取点4/12。builtin方案用0xc07f将LGKM设0且VM/EXP保持最大允许值，保留barrier；目的是让编译器识别普通C++ LDS读取完成，并未删同步。五新候选四真实shape初步正确性与guard均通过，但除点4对部分shape的小收益外都回退。

| BN / TFLOPS | 新基线点8 | builtin LGKM | DS asm | progressive DS | 点4 | 点12 |
|---|---:|---:|---:|---:|---:|---:|
| FC2 | 296.66 | 272.97 | 251.39 | 287.89 | 295.56 | 272.06 |
| QKV | 300.90 | 273.11 | 252.83 | 294.01 | 305.03 | 277.04 |
| out_proj | 282.80 | 263.11 | 240.26 | 274.75 | 283.33 | 268.91 |
| FC1 | 299.74 | 272.07 | 252.65 | 292.88 | 303.89 | 278.08 |

点4对FC2略慢、QKV/FC1约1.3%更快，out_proj约0.2%变化；尚无独立审计/复测，不替换默认有效候选。其余不采用。新开关关闭compat仍与b13a73b0...相同。完整记录stage_bn_static_wave_lds_schedule；减少编译器wait站点不能保证更快。

## 37. 当前版本全sections/SQTT留存

run_xprof_pairs_full.sh可传候选名与新目录，始终拒绝覆盖。按用户建议采集11个sections，加入--enable-sqtt --enable-raw；按名称子串h3_ck_rolephase,Cijk_分别捕获一次真实FC2。先后保存原双tile与新静态wave方案对照各自的F.linear。

- xprof_pairs_full_20261013：1个.perf、16个raw SQTT文件（双方各8SE），共1,370,222,224 bytes。
- xprof_static_wave_full_20261013：1个.perf、16个raw SQTT文件，共1,301,902,190 bytes。

各capture workload数值检查通过，manifest.json对每个原始文件记录bytes/SHA256。原始大文件留服务器；本地交付保留小型文本、清单和报告。工具限制仍见XPROF_ANALYSIS.md：full replay扰动UTCL2，xprof FetchSize定义不覆盖已核实全部EA计数，SQ指令计数/emulated等不能直接作roofline依据；.perf/SQTT尚需兼容XCompute逐指令分析，不能伪造精确源行latency。

后续围绕新方案增加的VM完成等待比较B resource标量rebase、原生式pow2 B寻址及预取点6/分组预取；以实际完整GEMM计时和正确性判断是否采用。330目标仍保持。

## 38. BO/BP：B resource标量rebase有独立小收益

raw B加载原来计算voffset=(row*K+kk)*2。新方案将每个完整compute wave共同的K32起点加入resource指针，并等量缩小resource byte bound；voffset保留row*K+(kk mod32)。末端物理地址不变，尾token仍在hardware bound内保护。readfirstlane只取每wave共同K起点，所有lane的行/K32内偏移保持原值。不是改矩阵布局、量化或忽略越界。

BO初筛的pow2 B寻址仍回退约287–291TFLOPS；预取点6约284–303，分组预取约282–302，均未胜过rebase。BP独立复测：

| M18880投影 | 静态wave基线ms | B rebase ms | 新TFLOPS | 同轮F.linear TFLOPS |
|---|---:|---:|---:|---:|
| FC2 | 4.905132 | 4.870980 | 298.726 | 326.265 |
| QKV | 7.256715 | 7.092379 | 307.743 | 327.189 |
| out_proj | 2.572934 | 2.548230 | 285.510 | 332.708 |
| FC1 | 9.711305 | 9.494265 | 306.519 | 327.495 |

耗时降低约0.70%/2.26%/0.96%/2.23%；独立102组/306launch审计通过，包括奇数K、单行/尾行guard、输入不变与determinism。实际VGPR225、SGPR31、LDS65536、scratch/spill0。注册为当前有效候选`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`，SHA `22ef3280c50201d0f66966e4fdfe6af75741f5d6dc4a2c46704bb4d7acd73025`。原始stage_bo/stage_bp、stress_bo_scalar_b_base_20261013保留。尚未达330目标。

## 39. BQ：FP分类BF16 RNE正确，收益太小暂不采用

核对原生库实际epilogue，它也使用buffer_store_short；没有证据支持“原生打包宽store而CK只有short”为主要差异。原生NaN转换用canonical payload，而CK保存payload/sign，不能直接复制它的转换作为无损方案。

h3_rne_fpclass用isnan选择CK的payload保存路径，普通RNE也会保持正负Infinity不变。独立probe核对全部65536 upper16×7类lower边界与1,048,576个随机32-bit，共1,507,328位模式，stock CK对照0 mismatch、输入和输出guard均通过；不是全部2^32穷举。

完整GEMM四投影初步正确。FP分类转换首次比B rebase控制只快约0.1%–0.2%，没有独立GEMM审计/性能确认，未采用；点4/6结合B rebase反而回退。因此当前有效库仍22ef3280...，不按跨轮筛选最小值替换。证据stage_bq_fpclass_rne_and_b_prefetch、rne_fpclass_probe_20261013。关闭新路径compat仍同SHA。

## 40. BR/BS：累加器大bank约束造成spill，不能据此排除对齐

BR通过空inline asm的v约束尝试把128个FP32累加器作为一个连续寄存器bank。DTK报couldn't allocate output register for constraint 'v'，3候选均未运行GPU；冻结source/build.log在各build/attempts目录。保留编译失败，不更换工具链。

BS改为16/32寄存器子组，分别在循环头尾、每相位、两者都放置约束。四真实shape数值正确，但头尾32方案VGPR256、spill111、scratch448，FC2仅165.42TFLOPS；头尾+每相位VGPR256、spill334、scratch956，FC2仅122.97，明确淘汰。仅每相位32/16方案VGPR225、无spill，但FC2约298.14/298.73，与298.76控制持平或略慢，未采用。

实际首条MMAC的C/A/B起点仍全mod4=1：各子组约束没有实现原本希望的全操作数对齐，不能将BS解读为对齐理论的完整反证。实际布局/resources保存在bs_first_mmac_20261013.json；工具修复为按候选集合生成独立默认输出名，避免新检查覆盖旧scalar-wave证据。

## 41. BT/BU：priority和producer对照无收益；全首条对齐也未胜出

BT在当前B rebase基线上重测A搬运burst（已完整声明m0/SCC clobber）、A base移动、结构化cache A，以及post-priority 2/3。四投影数值均通过；burst和A base整体略慢，cache A约266–286TFLOPS，priority 2/3与控制约0.1%浮动，没有稳定收益，均未采用。这些结论来自新静态wave流水，不能用旧BI/BJ直接替代。

BU在累加器活跃处通过空asm声明v0、v0/v1、v0/v1/v2保守clobber，让编译器把跨边界活跃值安排到其它寄存器。没有真的向这些寄存器写数值；检查它实际引入的布局变化及搬运成本。

| BU首条MMAC | C起点 | A起点 | B起点 | 三者mod4 | VGPR | FC2 TFLOPS |
|---|---:|---:|---:|---|---:|---:|
| 原B rebase | 125 | 161 | 133 | 1/1/1 | 225 | 298.57 |
| reserve1 | 126 | 162 | 134 | 2/2/2 | 226 | 298.28 |
| reserve2 | 127 | 163 | 135 | 3/3/3 | 227 | 298.26 |
| reserve3 | 128 | 164 | 136 | 0/0/0 | 228 | 298.26 |

三方案SGPR31、LDS65536、无spill/scratch，四投影数值通过；QKV/FC1也未优于控制。与BS/旧BE不同，reserve3确实令首条三个操作数均mod4=0；但完整方案不加速，不能把剩余约6%–14%差距仅归因于首条寄存器对齐，也不把一次首条检查外推为已验证所有指令/场景的bank冲突不存在。结果stage_bt_producer_priority_balance、stage_bu_low_register_constraints和bu_first_mmac_20261013.json。当前有效候选不变。

## 42. BV/BW：当前K64激活方案已完成，暂停新增实验

phase_k64_native.hpp在当前两barrier/静态wave节奏上预取完整K64 B bank，两bank轮换；分别在相位0/1发出未来四个向量。BV进入tile前vmcnt(0)，BW将等待移到DS读取后：相位0预取方案vmcnt(4)确保更早的当前bank完成并保留四个更新的未来加载；相位1方案在相位0完成vmcnt(0)。startup完成等待、匹配barrier和末尾全部bank活跃/VM drain均保留。

新终末future还含K+32，对单行尾token可能common>resource extent，必须夹紧bound到0，避免减法下溢。初次编译缺CACHE_B默认宏；补齐后发现CK generic r descriptor在夹紧后被分到VGPR，汇编无效。显式readfirstlane确认逻辑上CTA一致的resource字段并以s约束要求SGPR，编译通过；全部原失败源/日志已冻结，没有在错误版本上运行GPU。旧K32 bbase的compat仍与22ef3280...逐byte相同。

| 当前真实M / TFLOPS | BV p0 | BV p1 | BW p0 late | BW p1 late | BW同轮K32控制 |
|---|---:|---:|---:|---:|---:|
| FC2 | 223.39 | 259.14 | 221.65 | 278.55 | 298.84 |
| QKV | 227.56 | 263.43 | 224.91 | 282.94 | 307.83 |
| out_proj | 214.95 | 247.95 | 213.43 | 265.07 | 285.67 |
| FC1 | 226.67 | 263.08 | 224.65 | 281.22 | 306.54 |

四新候选各自102组/306launch审计全部通过（共408组/1224launch），包括K64 tile数1/3/5/7/11/13的奇数尾、真实四shape、输入不变、guard和重复结果。实际各VGPR242、SGPR28、LDS65536，scratch/spill0；不能把回退归为spill。相位1延后等待有所恢复但仍慢，四候选均未采用。独立有效最优仍BP的K32 B scalar rebase方案约286–308T。

2026-10-13用户要求“尝试完当前方案，停下来整理”。BV/BW已终止，停止新增GPU实验，整理76个完成阶段的优化主线至OPTIMIZATION_SUMMARY_20261013.md/JSON；约330T目标尚未达成，按用户请求暂停继续优化。未commit/push，保留有效库和全部负结果。

## 43. 用户恢复实验及审批额度后：BX–CB

用户随后授权继续，额度恢复后BZ已实际执行，并继续完成CA/CB。最终有效22ef库四shapecounter、BX固定N/K、BY CTA遍历和短K曲线先完成；BZ原生K64地址及四条MMAC窗口、CA wave组/分组预取、CB实际ISA指导的相位间B预取新增16候选，均通过四真实shape初筛，但没有新的稳定整体提升。81个完整计时阶段的索引已刷新，不把负收益候选或跨轮零点几百分点变化注册为有效版本。

当前有效库仍为BP独立验证的286–308T。四shapecounter显示VM完成等待差异仍较明显，尤其out_proj；这不是时间占比，也不足以单独确定根因。CB把原生的高层预取发射时机移入C++流水仍回退，说明仅模仿该顺序尚未复刻实际寄存器/DS/MMAC调度。详细新数据、资源、SHA和限制见RESUMED_RESULTS_20261013.md/JSON。所有新GPU任务已终止，03_gemm和共享include不变，未commit/push，目标未完成，代码目录整理仍按用户要求留待后面。

## 44. CC–CF：核对实际指令，再排除四种局部改法

先纠正第43节/初版恢复报告关于CB的过粗描述：完整原生ISA中，0x1791F08–0x1791F40为下一相位前8条DS读取，0x1791F48–0x1791F60才发出未来4个B向量，随后还有8条DS。不是“未来B先于下一相位全部DS”。CB点0/4依然是正确执行的时机对照，结果不变，但不能称为精确复刻原生顺序。生成脚本与最新报告已修正。

CC把phys(row+i*16,c)改为两个基地址加(i/2)*2048个BF16元素；CPU核对全部2048个lane片段起点，覆盖16384元素。C++读取、C++读取+LGKM全等待、显式单DS、显式成对DS四配置真实shape全部正确，但均无净收益。两显式DS配置同SHA f747cae0...，按二进制去重。实际ISA首相位DS地址VGPR3→2，C++候选VGPR225→221，显式DS为223，无spill/scratch；减少地址寄存器不能直接等同提速。初次将运行时unroll索引作为模板立即数编译失败，改为CK static_for后通过，原失败快照保留。

CD不删等待：K32提前到达barrier组在所有A读取完成后先ACK共享槽，再完成当前B等待再MMAC；K64候选在startup完成初始bank后于每个tile数学末尾等待未来bank，下一tile不重复入口等待。全部四真实shape初筛正确但未胜出，K32方案基本持平或小幅回退，K64仍约222–267T，不采用。

CE按4/8个i把独立C tile的低/高半数学指令分组，各C的K累加顺序仍为低16→高16→下一相位，四shape数值和guard通过。分组16控制生成与22ef有效库同SHA，分组4/8没有整体收益。CF移除producer终末未来A加载分支，分别以原bounded resource和终末zero-bound生成未消费的未来tile，必要VM完成等待/barrier全部保留，四shape初筛正确但约0.2%–0.5%回退，不采用。

本轮14个构建配置，12种不同于当前有效库的新二进制，当前85个完整阶段；没有给负收益候选追加独立102组广审计。完整矩阵、资源与限制见RESUMED_RESULTS_20261013.md/JSON和results/stage_cc/cd/ce/cf对应全名目录。85阶段中的筛选最小值不是有效库更新，当前独立BP数据及SHA不变，约330T目标尚未完成。

下一步仍需处理真实主循环指令流水，不能把这些局部失败解释为没有优化空间。可先核对producer动态slot/循环控制的实际ISA，若编译器尚未常量折叠，再验证producer固定slot的双tile展开；此项尚未实施，不把计划算作测试。原共享include、03_gemm和正式H3均未改，未commit/push，暂不重排实验目录。

## 45. CG–CK：producer寻址组合，仅两投影有小收益

已经执行第44节的producer双tile方向。原有效ISA中未来A的LDS槽仍动态s_and 0x8000，再经V运算/readfirstlane更新m0。CG将producer与consumer配对，未来A分别用固定slot1/0，奇数末tile不发出第二组握手。显式固定slot和编译器推断slot两候选生成同SHA 03025bda...；257种tile数的CPU序列核对及102组/306次GPU广审计通过，但CG单独不加速。

CH把权重搬运公共K byte位移放入MUBUF scalar offset。CK helper最后一项实际是immediate offset，不能直接用它传运行时K；新私有async_a_soffset.hpp显式指定s操作数并声明m0 clobber。scalar offset不参与voffset的硬件bounds检查，必须依赖已验证的执行域：host要求N%256、K%64，producer仅加载tile<count的完整feature/K tile；最末行/末tile的16byte向量恰好在分配末端。不将此接口用到M-tail的激活或输出、不与unused终末A预取等组合。

producer_soffset_probe.cpp独立验证8种K的首/中/末tile、3seed的任意int16位模式，63组/126次mapping、guard、输入不变通过；int16只是验证BF16大小搬运的原始位，不是量化。ppair1/asoff/ppair1_asoff三库各102组/306次GEMM广审计通过，包括奇数K和输入/输出guard。实际ISA的A load scalar操作数从0变为s寄存器；同VGPR225/SGPR31/LDS65536，无spill/scratch。static指令清单不是动态耗时或CPU/GPU比例。

CH初轮及CI独立复测均表明ppair1+asoff只对FC2/out_proj小幅更快，QKV/FC1会回退，不全局采用。CJ的shape_dispatch.cpp复用原/新两套相同数学源码，N5376/K7168或3584选择新producer，其余合法尺寸走原路径。增加真实N/K的小M尾部后，126组/378次全样本与原有效库输出逐位一致、finite/guard/输入不变/重复确定性及FP32参考误差均通过。

CK独立复测：FC2 300.174T（控制298.738T），QKV307.964T（307.794T），out_proj286.642T（285.552T），FC1306.604T（306.598T）。FC2时间下降0.478%、out_proj0.380%，其余基本持平；CJ初轮分别0.488%/0.397%，两轮各对应投影6轮中至少5轮更快。收益很小，不能称为接近330目标已完成，也不是H3 E2E收益。

保留可选库SHA d2e34b972e3961fb99b2ea05802fc458561788dbb9587dc74a52fb96b3a5361f；PROGRESS.json的validated_optional_shape_dispatch登记两轮原始证据和126组逐位审计，原validated_current_candidate/default22ef保持不变。详细实现、边界契约、资源、命令、结果见VALIDATED_PRODUCER_DISPATCH_20261013.md/JSON。当前90个完整计时阶段，原03_gemm/include和正式H3均未改，不commit/push，暂不做代码重排。

下一步待验证而尚未实现：重新检查完整K64 B bank的寻址。此前每个K32子向量分别rebase，终末K+32引入bound clamp及descriptor四字段readfirstlane；可尝试每个未来K64 bank仅rebase一次、将子相位32偏移留在voffset。对最少一行的resource有common<=K*2<=bound，可避免减法下溢，但是否能产生更好SGPR/VM指令和性能须实际编译、广审计、独立计时证明，不能把推导算成已优化。

## 46. CL–CP：完整K64寻址确有家族内改善，仍不替换K32

已经完成第45节的K64 bank single-rebase方向。raw_b_base64.hpp在future tile处将common=(tile+1)*128加到resource pointer、从bound等量扣除；两K32子相位留在voffset。每个remaining token行数>=1，future最多为K/64，所以common<=K*2<=bound，一行末尾也不下溢；不扩大物理访问范围。CL late方案相对同轮旧raw K64 parent耗时下降6.07%/6.50%/6.18%/6.60%，达到约277–296T。注意该raw父路径没有clamp，因此不能将6%全部归因于“删clamp”。实际旧逐K32 rebase静态compute区readfirstlane14，新K64 rebase为4；这些不是动态每tile计数或等待毫秒。

CM重测phase0、phase1点4/12及两基地址。更早phase0明显回退、点4/12也未胜出，两基地址使VGPR244→240、家族内部再改善约1%，仍未胜过K32。CN将初始B/startup提前到wave分组选择前，CO只共享不可变initial payload而将未来bank留在body内；两方法均回退。实际CN/CO达到256VGPR，spill1–3、scratch8–16bytes，但不能用spill数量解释全部性能差额。共享startup静态barrier12→10只是原两个互斥body中的初始化合到共同段；每个动态wave仍经历原来的两匹配startup barrier，未删同步。

CP保持future在phase1发出、顺序为next phase0的2load再phase1的2load。phase0 vmcnt2只完成最早的当前phase0两个向量；phase1在四个更新future之后vmcnt4完成更老的当前phase1数据。startup仍vmcnt0，最后所有bank仍活跃直到terminal vmcnt0 drain，A的LGKM/barrier保护不变。两基地址组合约0.5%进一步改善，最高FC2 295.263T、QKV301.506T、out_proj281.214T、FC1300.765T；同轮有效K32约298.460/307.906/285.583/306.451T，故不采用。

CL–CP共12候选，各102组/306次原版逐位审计（含odd K tile和M尾部）全部通过，合计1224组/3672次候选launch，不包含对照/FP32参考额外launch；全部输出与原22ef逐位一致，finite/guard/输入不变/三次确定性/FP32参考误差通过。重构后保留未启用startup方案时的原声明顺序，两K64父版本compat与981b3ca7...、45163df4...逐byte一致；默认22ef compat也不变。

当前95个完整计时阶段，目标仍约330T、尚未完成；默认22ef及d2e34...已验证可选库不替换，所有本轮GPU任务终止。完整同轮矩阵、资源、算法/边界契约、哈希和原始审计索引见B64_BANK_EXPERIMENTS_20261013.md/JSON；静态ISA库存为results/b64_bank_instruction_inventory_final_20261013。原03_gemm/include及正式H3未改，不commit/push，继续保留负结果而不重排目录。

## 47. CQ/CR：相邻feature打包输出与对齐分支外提

whole-wave lane XOR1交换将相邻feature的BF16打包为dword；aligned路径store128→64，2-byte C走两short回退，输出字节及CK RNE/NaN payload语义不变。DPP compiler builtin及DS 0x80b1两后端均通过审计。初始两次DS编码错误和raw inline DPP只有1拍源间隔的hazard失败冻结保留；compiler-aware DPP修复后通过，不能误判海光能力不支持。各候选aligned/offset2-byte各102组/306次原版逐位一致，四候选合计816组/2448次。

CQ静态branch141，CR将对齐选择外提至整个epilogue后降为15，但VGPR225→229。CR DPP static nop16→137，同时有两套对齐互斥body；静态store128short+64dword不能相加当动态192次。所有候选仍全部慢于同轮有效库，不采用。完整SHA、计时、资源、primary hazard资料与证据见PAIRED_OUTPUT_EXPERIMENTS_20261013.md/JSON。减少store条数不是收益保证。

## 48. CS：减小feature tile，资源更少但等待摊销变差

H3_PHASE_FEATURE_TILE默认256，实验128；MT16→8、NT仍2，8compute+4producer/768线程不变。每lane累加器128→64、LDS64→32KiB、VGPR225→129且无spill；producer load8→4，slot stride/CTA映射同步缩小。API原N%256/K%64域不变，matched barrier、data completion wait和RNE保留。CPU producer/LDS/C覆盖以及各35种grid证明通过。点4/6两候选各126组/378次含真实shape小M尾部原版逐位审计通过；默认compat22ef逐byte不变。

四shape仅约175–181TFLOPS，对比同轮286–308耗时回退63.4%–70.8%，不采用。full xprof及文本counter对照完成，point6相对BASE wave2倍、SALU聚合1.799倍、VMEM read聚合1.501倍；DTK VM等待3.061倍、barrier等待2.414倍。支持每wave数学量减半、控制/等待摊销变差，但不据此分配精确毫秒，SQ指令部分emulated，.perf未猜测解析，不宣称occupancy翻倍。若继续小tile，应同时调整compute/producer布局保持每wave数学量，该后续尚未实施。证据见FEATURE_TILE_EXPERIMENTS_20261013.md/JSON及results/profile_cs_feature_tile_compare_20261013/ANALYSIS.md。

当前98个完整计时阶段；330目标未完成，有效库与可选分流库不变，未commit/push，未重排目录或修改03/include/H3环境。

## 49. CT/CU：tile与compute/producer wave配套

CT小tile128×256配4compute+2producer，384线程，NT4/MT8恢复每lane128个FP32 accumulator；2producer两次覆盖原4逻辑row/swizzle组。startup/两barrier每K64/VM与LGKM完成保护均不变。比同轮CS的8+4小tile约2.5%–3%改善，但仅180–185T，feature CTA/token载荷重复仍增加，不采用。

CU回到原256×256，仅producer4→2（640线程），B加载数量和LDS64KiB不变。原producer循环及固定pair+A soffset两种都正确；多数shape耗时回退1.4%–5.6%。组合out_proj首次较原库约0.59%更快，但较同轮已验证选择库仅0.20%，尚未独立复测，不全局采用。四候选各126组/378次原版逐位广审计通过。CPU coverage/257种K握手、真实资源和计时在WAVE_LAYOUT_EXPERIMENTS_20261013.md/JSON。reg768是pool编译选项，不是这两个布局实际线程数。

## 50. CV：B K scalar offset及M尾部保护

async_b_soffset.hpp保留raw resource pointer，全部token row在checked voffset，公共K32 byte displacement进入soffset；不同于每K32 pointer rebase。soffset不参与bounds，因此终末unused common>=K*2时resource bound置0；有效phase必须K%64、lo<=24且16byte向量全在行内。不能把此前A soffset接口直接照搬M-tail，也未推广到任意stride/其它模型。

独立M1/3/63/192/256、K64/192/320/7168、首/中/末及terminal K/K+24、3seed/2repeat位模式probe360组/720次通过，invalid rows/terminal全零、input及输出prefix/tail guard不变。两个完整候选各126组/378次原版逐位通过；default compat22ef。初始MUBUF操作数次序编译错误冻结保留、未运行GPU。实际静态B loads20条全部使用soffset s寄存器（原版0），VGPR225/SGPR31/LDS64KiB不变；但耗时回退0.2%–0.7%，不采用。完整证据B_SOFFSET_EXPERIMENTS_20261013.md/JSON，静态条数不是动态每tile耗时。

## 51. CW：phase0末尾last-use重用av预取phase1 A

phase0每个/每四个fragment最后一次j1-hi MMAC完成后发射phase1 A读取，跳过phase1原始16次load。ACK0期间producer只写另一slot，当前slot由ACK1保护；early组LGKM0+ACK1及late组phase1显式LGKM0完成数据后才能数学使用或被producer tile+2覆盖。未删同步/terminal drain/VM等待，K累加顺序及RNE不变。CPU last-use/地址8192元素及257slot关系验证，两候选各126组/378次原版逐位广审计通过，default compat22ef。

实际ISA同一barrier区域内MMAC前后之间DS读取0→64，VGPR225→241，spill/scratch均0；高层prefetch确实进入kernel而不是被优化掉。计时却只有274–281T，比有效库慢，不采用。完整同轮xprof/DTK3次counter与静态片段在A_PHASE1_STREAM_EXPERIMENTS_20261013.md/JSON；counter为wave聚合，不解释为精确毫秒占比，静态交错不证明动态理想overlap。

当前102个完整计时阶段，330目标仍未达到；有效22ef/可选d2e不替换，不改H3/03/include，未commit/push、未重排目录，所有原始负结果保留。

## 52. CX/CY：部分A窗口及原生式B发射顺序

CX四个partial A phase1 last-use预取窗口各126组/378次正确，但没有净收益。CY换同row两个K32子相位优先发射，mode2使用共享Vaddr+immediate0/64；实际静态imm64 12条/共享pair12，VGPR240→237，无spill。分相位等待需按新issue顺序phase0 vmcnt1而不是2。raw探针840组/1680次、三候选各126组/378次原版逐位审计通过，但仍比原有效库慢约1.5%–2.4%，均不采用。CPU分析先后修复缺失reference文件和双kernel分派资源假定，不重复GPU运行。详见WINDOW_AND_B_ROW_EXPERIMENTS_20261013.md/JSON。

## 53. CZ–DC：bit62成对验证，out_proj定向采用

structured_b64.hpp使用pow2 stride+row remainder、每K64低48位pointer单次rebase，保留descriptor高位；同row两K32片通过immediate0/64，逻辑record count按剩余row，unused terminal range0。bit62 OFF/ON两套等待配对，独立搬运2640组/5280次及四候选各126组/378次逐位审计通过。开启bit62相对相同结构化OFF耗时下降约8.8%–13.1%，但不等于相对原raw的收益；只有out_proj真正胜过有效库，其他三类不全局采用。

DA独立复测及两候选各126组/378次C偏移2-byte审计确认out_proj优势。DB/DC组合分派仅out_proj用cache1/wait0；FC2保留producer pairs+A soffset；其余原路径。三路分派正常/offset2各126组/378次逐位审计通过；两轮out_proj 292.003/291.856T，较原库耗时下降2.142%/2.081%，较旧可选库下降1.852%/1.709%，各6/6轮胜过旧可选；其余相对旧可选在0.1%以内波动。登记为已验证可选库，不替换默认。

新增可选库SHA `de1cd4cc831fae0153f5cb0ef8dee163582501db7d83e776e192a33de05a5484`，三kernel资源均无spill/scratch；原/producer225VGPR31SGPR64KiB，cache240VGPR32SGPR64KiB。配对out_proj counter每实现3次：ON/OFF VM完成等待1.030倍、VM issue0.642倍、barrier0.632倍，不能将gain简单解释为完成等待下降，更不能把wave counter当毫秒比例。未插桩独立计时决定性能排名。

108个完整阶段；四shape约330T目标仍未完成，默认22ef和旧可选d2e保留，没有H3/INT8/E2E、commit/push或03/include/toolchain变化。全部产物与使用方式见VALIDATED_CACHE_DISPATCH_20261014.md/JSON，STOP_AUDIT_CX_DC_20261014.json确认本轮无自有GPU任务残留；不重排实验目录。

## 54. DD–DG：K32结构化CacheSwizzle达到约330T

沿用较快K32双bank/静态wave流水，不是继续K64方案。新增structured_b32.hpp：pow2 stride+row remainder，common K32偏移更新descriptor低48位pointer并保留stride/cache，逻辑row边界、terminal range0及最后current消费前VM0；默认宏0。GPU位模式1620组/3240次、descriptor540次回读、CPU地址138240项通过。新私有原型K<8192；统一入口仅四目标N/K使用它，其他尺寸保留原支持域fallback。

DD cache OFF/ON各自及各配producer组合，共四候选各126组/378次原版逐位审计。开启cache的新K32流水四shape约328–334T；组合对FC2/out_proj进一步改善。DE两开启候选各126组/378次C偏移2-byte审计、独立四shape计时确认收益。Cache OFF/ON实际SGPR44/38不同，不能把完整性能差全部描述成仅一个硬件bit的纯物理cache命中贡献。

struct32_dispatch.cpp统一h3_gemm入口：FC2/out_proj用cache+producer pair/A soffset，QKV/FC1用cache，其余原raw回退。DF统一入口正常/offset2各126组/378次与原22ef逐位一致；DF/DG两轮每个真实shape均达到约330T且接近最快rocBLAS（同轮耗时差不超过3%），不是用平均或最快单次替代。DD/DE/DF审计合计1008组/3024次candidate launch，额外72类特殊BF16/144次调用、三个fallback K8128/8192/16384、六类非法API参数均通过。

实际BF16 MMAC/结构化load已进入最终机器码，nm未见rocBLAS/hipBLAS/Tensile/LightOp计算依赖，冻结源码/库SHA一致，未改共享include/03；四shape最终counter同库每实现3次，旧DTK过滤遗漏新kernel的问题仅补采waits修复，原始6项保留，正式计时不重跑。所有资源无spill/scratch。

目标完成范围严格为四代表BF16独立GEMM在HCU4同口径接近rocBLAS约330T；没有量化、没有H3 E2E/并发/冷缓存结论。统一库SHA `cbd4638fde724142b41314e7d2578e79fbbdc0a6bcd43d6e154716e768f5f6b9`，源码包和完整审计见FINAL_RESULTS_330T_20261014.md/JSON。112个完整阶段，保留所有旧库/负结果，不commit/push，不重排目录。后续H3集成不属于本目标验收。
