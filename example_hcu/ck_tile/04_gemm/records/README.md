# 04_gemm：H3 BF16 CK Tile 优化

## 当前状态（2026-10-14，约330T目标验收完成）

四个H3 TP2+SP2代表本地BF16 GEMM已在两个独立轮次接近rocBLAS约330TFLOPS。仅HCU4独立算子、M18880、BF16输入输出/FP32累加、hot-cache HIP event；不是H3或INT8 E2E结果。

| shape | 最终DG ms | 最终DG TFLOPS | 同轮rocBLAS linear TFLOPS |
|---|---:|---:|---:|
| fc2 | 4.420853 | 329.142 | 328.756 |
| qkv | 6.550875 | 333.182 | 328.280 |
| out_proj | 2.206406 | 329.742 | 332.359 |
| fc1 | 8.775322 | 331.632 | 327.607 |

统一入口[struct32_dispatch.cpp](struct32_dispatch.cpp)，私有加载[structured_b32.hpp](structured_b32.hpp)，共同流水[rolephase.cpp](rolephase.cpp)。构建目标`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32dispatch_reg768`，SHA `cbd4638fde724142b41314e7d2578e79fbbdc0a6bcd43d6e154716e768f5f6b9`。主要新增为K32结构化激活B寻址+descriptor bit62 CacheSwizzle，FC2/out_proj配producer fixed pair+A soffset；其余非目标N/K回退原库。

完整结果、两轮每shape对比、逐条验收、边界、命令和源码包见[FINAL_RESULTS_330T_20261014.md](FINAL_RESULTS_330T_20261014.md)/同名JSON；counter见[最终四shape分析](results/profile_struct32_four_20261014/ANALYSIS.md)。112个完整计时阶段，原22ef/d2e/de1c库保留；不改03/include、H3、驱动或工具链；无commit/push、无目录重排。

## 此前状态（2026-10-14，K64缓存阶段）

原默认有效库22ef和旧可选分派d2e保留。新增已验证可选库`..._bbase_cachedispatch_reg768`，SHA `de1cd4cc831fae0153f5cb0ef8dee163582501db7d83e776e192a33de05a5484`：仅out_proj N5376/K3584启用descriptor第62位CacheSwizzle与K64结构化流水；FC2保持已有producer优化，其余走原K32 raw。不要把下面历史阶段的“未启用swizzle”理解成所有最新可选路径均关闭。

组合分派两轮out_proj约291.86–292.00TFLOPS，较原默认耗时下降2.08%–2.14%，较旧分派下降1.71%–1.85%；FC2约300.3，QKV/FC1基本保持。正常及2-byte输出对齐各126组/378次原版逐位审计通过。当前108个完整计时阶段，四shape约330T目标尚未达成，不是H3 E2E收益。

主入口[cache_dispatch.cpp](cache_dispatch.cpp)，私有加载实现[structured_b64.hpp](structured_b64.hpp)，完整复现与两轮证据见[VALIDATED_CACHE_DISPATCH_20261014.md](VALIDATED_CACHE_DISPATCH_20261014.md)。bit62成对开关及raw对照见[STRUCT64_CACHE_SWIZZLE_20261014.md](STRUCT64_CACHE_SWIZZLE_20261014.md)，counter见[ANALYSIS](results/profile_struct64_out_proj_20261014/ANALYSIS.md)。只在04_gemm测试，未接H3，未改03/include，无commit/push；原始失败/负结果保留，目录未重排。

由03_gemm的MMAC实现及前期修正BF16 traits/RNE适配派生，保留03和共享include。
目标：四卡TP2+SP2代表本地GEMM shape下，接近rocBLAS约330TFLOPS。

在ck_yyc编译/单卡测试，仅物理HCU4；不启动H3。
候选首先比较LDS布局及tile/BK/wave组合，之后迭代流水。
build.py保存每个编译配置/命令/日志/哈希。bench.py通过合成精确数据和普通随机数据检查后才计时。
性能使用独立HIP event；未达到近似rocBLAS前不宣布目标完成。

2026-10-13：AF修复转置候选kPadN触发的B读取scalar化，真实M18880四投影约203–213TFLOPS；后续AI的K32激活寄存器相位预取约213–229TFLOPS，数值/finite/输出尾guard通过；未达330目标。
cache swizzle已在role64实验中测试，但未胜过raw，未用于最快候选。CTA按token分组和LDS padding是不同机制。详见[LDS_WRAP_AND_PIPELINE.md](LDS_WRAP_AND_PIPELINE.md)第9–10节及[RESULTS.md](RESULTS.md)。AH分组/tile筛选以最新结果JSON为准。

AI代码在rolephase.cpp，不在正式H3代码中；详见报告第12节。run_stage_af/ag/ah/ai/aj.sh可重放各方向（默认生成新tag，拒绝覆盖已有结果）。AJ确认新phase流水下A/B/AB cache swizzle也没有净收益，最快路径仍关闭这些选项。

最新AK显式等待双bank约223–240TFLOPS，AL独立重复一致，72组/216次多尺寸正确性审计通过。32KiB权重缓冲回退，保留64KiB。run_stage_ak/al.sh可重放，等待计数与下一步方向见报告第13–15节。尚未达330目标，未替换正式H3 GEMM。

最新AU：mixed LDS/B预取 + 去除终末预取分支 + bounded buffer输出，真实M18880四投影约244–258TFLOPS（FC2 255.54、QKV 258.33、out_proj 243.97、FC1 258.37）。同轮rocBLAS仍约327–330，未达到目标；AU独立72组/216次正确性审计通过，不是H3 E2E成绩。详情与所有负结果见报告第16–18节。

`run_xprof_au.sh`使用已安装xprof采集完整11个sections及SQTT原始数据；`run_xprof_metrics_au.sh`保存文本counter对照。均限定物理HCU4，拒绝覆盖旧目录，不安装或切换工具链。采集时间不用于性能排名；UTCL2需注意replay扰动。原始大文件留在results/xprof_au_full_20261013，文档见XPROF_ANALYSIS.md。

AV按counter方向改变预取点4/12或wave分组错开，四投影全部正确但更慢，未采用。原AU点8独立复测仍244–258TFLOPS，最佳库SHA不变，330目标继续。`run_stage_av.sh`与stage_av_xprof_guided_prefetch_points保留可复现证据。

AW限制跨相位调度/MMAC burst及post-priority、AX完整K64激活双bank都回退，未替换快路径。AY/AZ的branchless BF16输出转换有小幅收益：独立复测FC2 257.89、QKV260.27、out_proj247.83、FC1 260.68TFLOPS；保持CK RNE及NaN payload语义。约150万个转换位模式、72组/216次真实GEMM审计均通过。mask与select源码最终生成相同SHA，按二进制去重。

最新有效候选`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_reg768`，SHA `f046d922a9fc1ea3c003bcb674b6424c0d42595c8b86e95204973c49f2a4f38f`。详见报告第20–22节、`results/rne_isa_comparison.json`和stage_ay/az原始结果。原AU仍保留；默认配置及正式H3未替换，不把261称为330目标完成。

BA/BB新同步节奏进一步提升：独立复测FC2 271.48、QKV274.17、out_proj258.80、FC1 273.16TFLOPS，较同轮旧RNE耗时下降4.23%–5.17%。72组/216次审计通过，关闭新开关重编译与旧基线逐byte相同。最新有效候选为`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_reg768`，SHA `b44b845565a273e2986622fca58f599703511796779171aa0f0f7716ac16ac47`；仍未达约330目标。

16384元素CPU地址核对确认当前K64 LDS wrap与原生库数学读取映射相同，不是缺一个新swizzle；xprof/rocprof新counter见`cadence_counter_analysis.json`，barrier与LGKM等待聚合减少，但LDS issue等待增加，不能将counter变化直接换算为ms。详情见报告第23–25节。

BC–BF继续验证pow2 B寻址、标量地址计算、寄存器live-pad及wave组错位同步，全部真实shape数值通过，但没有取得优于b44的稳定收益，未采用。寻址探针强化为随机int16及逻辑OOB保护，共两轮各30组；初始类型转换/未定义宏编译失败原样保留。当前有效候选仍b44、约259–274TFLOPS；PROGRESS.json新增validated_current_candidate，避免把筛选约0.1%浮动当作更新。详见报告第26–29节。

BG/BH显式双K64 tile展开有独立小收益，约263–279TFLOPS：FC2 274.30、QKV278.60、out_proj262.86、FC1 277.64。扩展102组/306次审计覆盖K64 tile数1/3/5/7/11/13等奇数尾，全部通过。最新已验证库为`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_reg768`，SHA `19453374cf0ad04f191c2407e744b79125cecc05b44836e0a209413db642039f`。原b44保留，未替换正式H3，330目标尚未完成。

BI/BJ producer m0搬运批处理回退，未采用。初稿内联汇编未声明m0/SCC clobber，已修正并核对device LLVM IR；当前机器码虽相同，源码契约不能省略。新的xprof和完整记录见报告第30–32节、`pair_tiles_analysis_20261013.json`。

BL/BM取得新进展：静态特化wave组消费循环，结合第一条MMAC后priority，独立复测FC2 296.79、QKV301.05、out_proj282.98、FC1 299.78TFLOPS，较同轮双tile耗时下降7.07%–7.50%。102组/306次广审计通过。最新有效候选`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768`，SHA `b13a73b09afbc30fb75563ffa896981b0e260ab5478d40735708047257ae2c7f`。原库保留，330目标尚未达到。

BK的二维grid/分组没有整体收益；BN的显式DS顺序和builtin等待回退，点4仅改善部分shape，未采用。新xprof/rocprof显示LDS issue等待明显减少但VM完成等待增加。完整11sections和SQTT在`results/xprof_static_wave_full_20261013`，原始文件逐项SHA见manifest；CPU可读对比在`results/xprof_static_wave_20261013/analysis.json`。详情见LDS_WRAP_AND_PIPELINE.md第33–37节。

BO/BP进一步有小收益：B的共同K偏移移入resource base，等量缩小边界；独立FC2 298.73、QKV307.74、out_proj285.51、FC1 306.52TFLOPS，102组/306launch审计通过。当前有效候选为`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`，SHA `22ef3280c50201d0f66966e4fdfe6af75741f5d6dc4a2c46704bb4d7acd73025`，约286–308，330目标尚未达到。

BQ的isnan版CK语义BF16 RNE通过1,507,328位模式检查，但完整GEMM首次仅约0.1%–0.2%变化，未采用。BR大寄存器bank编译不支持，BS子bank头尾约束导致spill，phase-only没有收益；BU实际把首条C/A/B全部mod4对齐仍没赢过控制。BT新流水的producer/cache/更高priority也未胜出。证据和限制见报告第38–41节，所有已验证旧库保留。

2026-10-13按用户要求完成当前K64激活预取方案（BV/BW）后暂停GPU迭代。四个K64候选分别通过102组/306launch审计，但约213–283T均未胜出，当前有效最优仍BP的约286–308T。历史主线、每项收益及无效尝试分析见[OPTIMIZATION_SUMMARY_20261013.md](OPTIMIZATION_SUMMARY_20261013.md)，结构化数据在同名JSON；详细最后一轮见LDS_WRAP_AND_PIPELINE.md第42节。约330T目标尚未达成，未commit/push。

随后用户授权恢复实验，完成最终B rebase库的四shape counter、固定N/K特化、CTA遍历分组和短K曲线。审批额度中断后用户再次恢复额度，BZ已实际执行；继续完成CA wave分组/分组预取及CB原生式相位间B预取，共新增16候选，四真实shape初筛全部通过，但没有稳定整体收益，均未替换有效库。当前共81个完整计时阶段，仍约286–308T，未达到330目标。最新可核对表格和设备资源见[RESUMED_RESULTS_20261013.md](RESUMED_RESULTS_20261013.md)及同名JSON；[EXPERIMENTS_RESUMED_20261013.md](EXPERIMENTS_RESUMED_20261013.md)保留恢复过程说明。报告和原始BX/BY/BZ/CA/CB、短K曲线已同步本地。未commit/push，未重排目录，未改03_gemm或共享include。

最新续接：CC两组LDS基地址、CD等待/barrier解耦、CE独立C tile数学顺序、CF producer终末分支共14个构建配置（12种不同于当前有效库的新二进制）完成，四真实shape初筛全部通过，但未取得稳定整体收益；当前85个完整阶段，有效库仍22ef3280...。CC确认首相位DS地址VGPR3→2、C++候选总VGPR225→221但并未加速；CE分组16与当前库同SHA作为控制。报告已补充各阶段矩阵、资源和负结果。注意ISA顺序纠正：原生未来B加载在下一相位前8条DS之后，而非DS之前；CB点0/4是调度对照，不是精确复刻原生。详细依据见RESUMED_RESULTS_20261013.md。

最新CG–CK：producer固定槽双tile单独回退；与A scalar offset组合后，仅FC2/out_proj有可复测的小收益。`shape_dispatch.cpp`仅对N5376/K7168或3584选择组合核，其余走原核。126组/378次含小M尾部的审计与原版逐位一致，两轮完整计时FC2约0.48%、out_proj约0.38%–0.40%时间降低，QKV/FC1基本持平。保留为可选库`..._bbase_shapedispatch_reg768`，SHA `d2e34b972e3961fb99b2ea05802fc458561788dbb9587dc74a52fb96b3a5361f`，原有效22ef/default不替换；90个完整阶段，约330T仍未达标。实现域/硬件bounds注意事项、probe及独立证据见[VALIDATED_PRODUCER_DISPATCH_20261013.md](VALIDATED_PRODUCER_DISPATCH_20261013.md)。这些是BF16单算子，不是INT8/E2E收益；不commit/push。

最新CL–CP：每K64 B bank单次rebase使raw K64父路径耗时下降6.07%–6.60%，两基地址及分相位等待进一步小幅改善；但最好的新K64仍约281–302T，未胜过有效K32/选择库，不采用。共享全部/初始B startup达到256VGPR并出现spill，已记录回退；关闭该新路径的两父版本compat逐byte同SHA。12候选各102组/306次原版逐位广审计全部通过，合计1224组/3672次候选launch（原版比较/FP32参考额外launch不计入）；95个完整阶段。详细算法、边界、同轮数据和实际资源见[B64_BANK_EXPERIMENTS_20261013.md](B64_BANK_EXPERIMENTS_20261013.md)及同名JSON；目标尚未完成，原库/已验证可选库/03_gemm/include均不变，未commit/push。

最新CQ/CR：相邻feature成对输出，将aligned路径每wave store128→64，但交换/打包及对齐处理成本抵消收益，四新候选均回退、不采用。每候选aligned和C偏移2-byte各102组/306次原版逐位审计通过，合计816组/2448次；quad编码错误、raw DPP hazard失败已记录及修复，失败不计入完成阶段。CR外提对齐选择后静态branch141→15，但VGPR225→229，仍未加速。97个完整计时阶段。具体数据与证据见[PAIRED_OUTPUT_EXPERIMENTS_20261013.md](PAIRED_OUTPUT_EXPERIMENTS_20261013.md)。原库22ef及可选分流库d2e不变，330目标未完成。

最新CS：feature tile256→128、token tile仍256，LDS64→32KiB、VGPR225→129、每lane accumulator128→64，但compute/producer保持8+4，四shape吞吐回退至约175–181T。两个预取点各126组/378次原版逐位审计通过，默认compat仍22ef；不采用。CPU映射证明、真实资源、同轮计时及xprof/DTK对照见[FEATURE_TILE_EXPERIMENTS_20261013.md](FEATURE_TILE_EXPERIMENTS_20261013.md)及[Counter分析](results/profile_cs_feature_tile_compare_20261013/ANALYSIS.md)。总wave翻倍、VM等待聚合3.061倍、barrier2.414倍，但counter不能当耗时占比；.perf保留未猜解析。98个完整阶段，目标未完成，无H3改动/commit/push。

最新CT–CW：4阶段/8候选各126组/378次原版逐位审计通过（共1008组/3024次）。小tile配4+2 waves改善CS约2.5%–3%但仅180–185T；原tile配8+2 waves多数回退；B K soffset独立M-tail/terminal probe360组/720次通过但完整核回退0.2%–0.7%；A phase1 last-use预取真实进入ISA，mixed DS0→64、VGPR225→241无spill，但仅274–281T。全部不替换当前有效库；CU组合out_proj首次小收益尚未独立确认。当前102个完整阶段，约330T未完成。分别见[Wave布局](WAVE_LAYOUT_EXPERIMENTS_20261013.md)、[B soffset](B_SOFFSET_EXPERIMENTS_20261013.md)、[A跨相位预取](A_PHASE1_STREAM_EXPERIMENTS_20261013.md)及同名JSON，所有原始失败/审计/计时/counter保留。无H3/03/include改动，无commit/push，未整理重排目录。
