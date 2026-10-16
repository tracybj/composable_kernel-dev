# CK Tile BF16 GEMM：约70T到300T的优化总结（2026-10-13）

本轮GPU实验已结束，按用户要求暂停继续调优，整理已完成的实现与证据。最优有效方案仍约286–308 TFLOPS，约330 TFLOPS目标尚未达到。

## 1. 结果与测试口径

这次优化取得的主要进展，是把GEMM的访存、寄存器工作集与跨wave执行顺序逐步改为适合gfx936的流水。数学仍是BF16输入/权重、FP32累加、BF16 RNE输出，没有量化。实现位于04_gemm，经历了03派生通用模板的调优以及以CK MMAC primitive编写的rolephase.cpp流水；并非rocBLAS调用包装。

环境：ck_yyc、物理HCU4、gfx936、DTK25.10.0-0、Torch2.9.0/HIP6.3.26113。四类H3本地投影shape沿用TP2+SP2代表数据；本次是单卡独立GEMM，不包含TP/SP通信、H3网络或端到端推理。

初始base实际约65–86T，称“70T”是近似概括。当前有效方案独立BP复测如下：

| 投影 | 当前M/N/K | 初始TFLOPS | 当前ms | 当前TFLOPS | 同轮rocBLAS TFLOPS | 归一化吞吐比 | 达到参考吞吐比例 |
|---|---|---:|---:|---:|---:|---:|---:|
| fc2 | 18880/5376/7168 | 67.88 | 4.870980 | 298.73 | 326.26 | 4.40× | 91.6% |
| qkv | 18880/10752/5376 | 64.63 | 7.092379 | 307.74 | 327.19 | 4.76× | 94.1% |
| out_proj | 18880/5376/3584 | 86.13 | 2.548230 | 285.51 | 332.71 | 3.31× | 85.8% |
| fc1 | 18880/14336/5376 | 64.69 | 9.494265 | 306.52 | 327.49 | 4.74× | 93.6% |

早期筛选M=18944，后续真实M=18880；N/K一致。两M相差约0.34%，因此上表倍数是按实际逻辑FLOPs归一化的跨轮吞吐比，不能称为严格同轮、同shape的加速实验。比如FC2由67.88到298.73约4.40倍，QKV/FC1约4.7倍，out_proj约3.3倍；不能写所有投影都是4倍。性能稳定性以每个新里程碑自己的同轮控制和独立复测为依据。

计时采用5次预热、6轮轮换、每轮10次HIP event均值的中位数，属于热缓存重复单算子基准。TFLOPS=2×M×N×K/(ms×10^9)。没有使用未知硬件峰值计算MFU，也不以profiler replay时间排名。

## 2. 有效里程碑

下表按技术演进排列；不是把各轮最快样本拼成一个新kernel。标注“筛选”的早期方案没有与后期独立广审计相同的验收强度。早期通用模板到后期分工流水属于实现路线切换，某项技术的收益不能脱离组合直接相加。

| 实现变化 | M | FC2 T | QKV T | out_proj T | FC1 T | 验证/证据目录 |
|---|---:|---:|---:|---:|---:|---|
| 起点：03派生base | 18944 | 67.88 | 64.63 | 86.13 | 64.69 | 初始控制：stage_a_layout |
| 扩大输出tile | 18944 | 105.02 | 106.53 | 103.86 | 106.50 | 筛选：stage_b_tiles |
| 避免提前等待未来VMEM | 18944 | 117.27 | 124.02 | 120.16 | 123.31 | 筛选：stage_c_raw_barrier |
| LDS padding16 | 18944 | 138.21 | 135.70 | 129.30 | 135.38 | 筛选：stage_e_lds_padding |
| CTA分组、wave分区 | 18944 | 149.56 | 150.19 | 139.47 | 149.59 | 筛选：stage_l_balanced_operands |
| 大tile＋允许足够VGPR | 18944 | 187.84 | 184.48 | 165.46 | 184.33 | 筛选：stage_o_register_pool |
| 同一大tile的真实M控制 | 18880 | 187.40 | 184.00 | 164.84 | 183.81 | 真实尾行验证：stage_u_native_m18880 |
| 恢复连续K向量读取 | 18880 | 206.14 | 212.61 | 203.08 | 212.49 | 真实shape验证：stage_af_native_m18880 |
| A经LDS、B K32寄存器预取 | 18880 | 225.87 | 228.75 | 212.51 | 228.90 | 真实shape验证：stage_ai_phase_register_prefetch |
| B双bank＋显式完成等待 | 18880 | 239.08 | 239.81 | 222.53 | 239.82 | 真实shape验证：stage_ak_banked_wait_schedule |
| DS读取与未来B预取交错 | 18880 | 246.38 | 246.40 | 228.56 | 246.51 | 独立复测：stage_ar_mixed_prefetch_rebuild_verify |
| 交错预取＋移除终末分支 | 18880 | 248.17 | 249.38 | 231.52 | 249.55 | 独立复测/广审计：stage_at_combined_prefetch_independent |
| 有界buffer输出 | 18880 | 255.54 | 258.33 | 243.97 | 258.37 | 独立审计：stage_au_bounded_buffer_output |
| 保持CK语义的branchless RNE | 18880 | 257.89 | 260.27 | 247.83 | 260.68 | 独立复测/广审计：stage_az_branchless_rne_independent |
| producer/consumer同步节奏 | 18880 | 271.48 | 274.17 | 258.80 | 273.16 | 独立复测/广审计：stage_bb_cadence_independent |
| 显式双K64 tile展开 | 18880 | 274.30 | 278.60 | 262.86 | 277.64 | 独立复测/102组审计：stage_bh_pair_tiles_independent |
| 静态wave分组＋post-priority | 18880 | 296.79 | 301.05 | 282.98 | 299.78 | 独立复测/102组审计：stage_bm_static_wave_independent |
| B标量resource rebase | 18880 | 298.73 | 307.74 | 285.51 | 306.52 | 独立复测/102组审计：stage_bp_scalar_b_base_independent |

## 3. 哪些改动解决了什么问题

### 3.1 约70T到188T：提高数据复用，解除寄存器分配限制

初始128×128 tile的数据复用和数学工作密度较低。扩大输出tile、调整wave分区、LDS padding和CTA分组后，FC2从约68T到约150T。局部同步调整避免当前LDS同步顺便等待未来global load，改善搬运与计算重叠；必要的数据完成等待仍保留。

扩大到256×256 tile时，累加器和操作数需要更多寄存器。768-thread原型曾因编译器分配限制只有84 VGPR、出现大量spill，性能约4.6T。核实并使用DTK的--enable-num-vgprs-768=true后，允许足够VGPR，消除这类严重spill；结合大tile/padding，通用方案FC2约188T。此开关只作用于候选编译，不是改驱动/硬件资源或换运行库；也不能把初始base全部低性能归因于spill。

### 3.2 约188T到240T：恢复向量加载，采用分工和寄存器B预取

源码和ISA发现，padding配置把连续K方向B读取降为scalar ushort。私有problem配置恢复vec8读取，同时保留真实M尾行保护，FC2约206T。优化padding时若无意失去向量化，会抵消布局收益。

从实际rocBLAS ISA确认8个计算wave、4个加载wave、A direct-to-LDS、B直接进入计算wave寄存器，随后用CK primitive构造同类分工。此处内部A是权重、B是激活，来自W@X.T的列主序解释，外部结果仍为X@W.T。

K32相位预取控制B寄存器存活区间；采用1×8计算wave分区让同一B fragment被多组MMAC复用。B双bank加显式vmcnt等待，确保消费当前bank时允许更晚发出的未来bank仍在飞行；终末bank保持活跃并drain。FC2约226T再到239T。关键是减少冗余LDS搬运和正确安排数据生命周期。

### 3.3 约240T到274T：精调预取、输出和producer/consumer节奏

在16次A的LDS读取中间插入未来B预取，减少整段DS/VMEM串行发射；去掉热循环终末预取条件，但末尾保持bank活跃并完整等待。组合独立FC2约248T。

输出改用有界buffer_store_short，并把所有token偏移放入可参与bounds check的voffset，保护尾行；branchless RNE保持CK的Inf/NaN payload/sign语义。独立FC2约256T、258T。输出路径的改动同样影响指令数量和存活区间，收益不能只归因于store指令种类。

producer的下一A tile预取跨越当前两数学相位；consumer与producer使用匹配的双barrier节奏，避免提前等待未来A。独立FC2约271T、QKV约274T，比同轮旧RNE耗时下降约4%–5%。

### 3.4 约274T到308T：静态展开、两组wave错位、B寻址

显式双K64 tile展开让LDS slot0/1成为编译期固定偏移，保留K32 B bank生命周期及奇数尾K。FC2约274T，其他投影约263–279T；静态MMAC数翻倍对应循环体展开，动态数学工作没有增加。

仅把两组wave的barrier前后错位，曾明显回退。后续将两组完整消费循环在编译期分别生成，把wave组选择移到K循环外，并在第一条MMAC后设置priority、数学结束复位。这个组合才取得独立7.07%–7.50%的耗时降低，约283–301T。单独静态分组或在数学前提升priority仍慢，因此不能把组合收益归给其中一个因素。

最后把共同K32偏移放入B resource base，并等量缩小resource bound，保持物理末端与尾行保护不变。独立收益约0.7%–2.3%，得到当前约286–308T。源码采用的是raw B读取；结构化cache swizzle候选没有胜出。

## 4. 最后K64方案：正确，但不采用

沿用当前静态wave与双barrier节奏，B寄存器bank从K32扩成K64；分别在相位0/1预取，并进一步把当前bank等待延后到DS读取之后。未来完整K64 bank会涉及未使用的K+32加载，因此必须防止resource bound减法下溢；修正为夹紧到0，并明确SGPR descriptor约束。首次未定义CACHE_B宏、以及generic r把夹紧后descriptor分配到VGPR的编译失败均保留冻结证据。

四候选分别通过102组/306launch审计，但性能均低于当前有效方案：

| BV/BW候选 | FC2 T | QKV T | out_proj T | FC1 T |
|---|---:|---:|---:|---:|
| K64 p0 | 223.39 | 227.56 | 214.95 | 226.67 |
| K64 p1 | 259.14 | 263.43 | 247.95 | 263.08 |
| K64 p0_late | 221.65 | 224.91 | 213.43 | 224.65 |
| K64 p1_late | 278.55 | 282.94 | 265.07 | 281.22 |

这些候选VGPR242、SGPR28、LDS64KiB、无scratch/spill。相位1延后等待能恢复一部分性能，但仍不如K32方案；不能把回退全部归因于spill，也不能凭“更深预取”预测更快。扩大的寄存器存活区间、读发射与等待安排一起变化，尚无逐指令证据将成本完全拆开。

## 5. 无效尝试带来的结论

| 尝试 | 实测结论 | 对后续分析的启示 |
|---|---|---|
| 单纯增加BK/双LDS缓冲/计算wave数量 | 多次回退或spill | 增加流水深度必须同时核对资源、依赖和实际访存 |
| 结构化cache swizzle、pow2 B寻址 | 多轮正确但慢 | 当前收益不是来自cache swizzle；不能因原生库使用就直接照搬 |
| 显式DS发射顺序、progressive等待、builtin等待 | 多数回退 | 更少wait站点或更“整齐”的源代码不等于更好的执行 |
| 更少m0/readfirstlane、A搬运burst | 在新旧流水均未胜出 | 减少静态指令同时改变发射与存活区间，必须实测 |
| 2D grid/更多tile分组 | 一些FC2略快，FC1明显慢 | 单shape优化不能未经dispatch设计就作为全局方案 |
| 整体128寄存器bank约束 | 编译不支持 | 当前工具链无法直接表达该约束 |
| 16/32子bank约束 | 头尾约束导致spill，phase-only无收益 | 约束改变必须看实际ISA，不能仅按C++变量排列推断 |
| reserve3令首条C/A/B均mod4=0 | 正确但仍不加速 | 首条寄存器对齐不足以解释剩余差距，不代表所有bank效应已排除 |
| FP分类版CK语义RNE | 150万位模式通过，首次仅0.1%–0.2%变化 | 小收益需独立确认；未替换当前有效库 |
| priority由1升到2/3 | 基本持平 | 当前组合的收益不是priority数值越大越好 |

## 6. profiler证据能说明到哪一步

xprof完整11个sections、SQTT及DTK rocprof等待counter均已使用。阶段BM（静态分组+post-priority）相对原双tile：LDS issue等待聚合约286.79M→78.68M，barrier约467.91M→333.81M；VM完成等待却约124.34M→200.99M。数据支持“执行节奏改变后LDS发射竞争减少，新的数据等待值得检查”，实际性能提升仍由独立HIP event测量证明。

这一counter对照对应BM，不能冒充最终B rebase版本的counter。等待是跨wave/per-SIMD累计量，不能相加作kernel时间占比；SQ指令和bank conflict部分为emulated，0 bank conflict不证明无冲突。完整replay扰动UTCL2，当前FetchSize定义也未核实为全部HBM流量；不能据此宣称HBM打满或算MFU。

完整raw数据和逐文件SHA256留在服务器results/xprof_pairs_full_20261013及xprof_static_wave_full_20261013；CPU可读对照在results/xprof_static_wave_20261013/analysis.json。.perf/SQTT仍需兼容XCompute作逐指令分析，当前没有把二进制猜解成精确源行latency。

## 7. 已确认的成果与剩余差距

最大的成果不是一项孤立技巧，而是把通用模板的低效访存/同步路线改为适配HCU的数据流，再用真实ISA与正确性约束逐层调整。Tile复用、寄存器容量、连续向量读取和A/B分工奠定了从70T到200多T的主体收益；后面的bank等待、同步节奏、输出和静态wave组合把它提高到300T左右。

当前约为同轮rocBLAS有效吞吐的86%–94%。达到参考耗时还需缩短当前kernel耗时约6%–14%，out_proj差距最大；目标仍是代表shape接近约330T。此次没有测完整H3替换后的E2E或跨卡通算融合，单算子约4倍收益不等于H3端到端约4倍收益。

暂停后如恢复调优，优先先用现存SQTT定位最终候选的暴露等待和逐指令调度差异，尤其分析out_proj的较短K；再考虑按shape调度的tile/pipeline或少量核心指令安排。尚未验证的方案不能列为已完成，也无需重复已经明确回退的同一配置。

## 8. 当前有效实现与交付

有效候选：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`。

库SHA256：`22ef3280c50201d0f66966e4fdfe6af75741f5d6dc4a2c46704bb4d7acd73025`。

源码入口：rolephase.cpp；构建配置：build.py；独立复测：run_stage_bp.sh；102组审计：results/stress_bo_scalar_b_base_20261013；当前候选注册：PROGRESS.json的validated_current_candidate。

继续重现时显式选择该候选，使用相同DTK/gfx936编译和全新的bench tag，保留默认模型环境。所有新实现都在04_gemm，03_gemm与共享CK include保持原内容；本轮未commit/push。当前按用户要求暂停GPU优化，保留有效方案、已完成实验及负结果。

详细逐轮记录：LDS_WRAP_AND_PIPELINE.md、RESULTS.md、WIKI_AND_ROCBLAS_ISA.md、XPROF_ANALYSIS.md。本文结构化数据在OPTIMIZATION_SUMMARY_20261013.json，可用于后续图表或文档。
