# 恢复迭代后的实验记录（2026-10-13）

目标仍为真实H3 BF16 GEMM接近rocBLAS约330TFLOPS。用户已授权恢复实验，暂不整理代码目录。当前经独立广审计的有效库仍为B rebase/K32方案，SHA `22ef3280c50201d0f66966e4fdfe6af75741f5d6dc4a2c46704bb4d7acd73025`；约286–308T，目标未完成。

## 已完成：最终库四shape counter

原始目录：`results/profile_final_bbase_four_20261013`。每种实现每shape各3次xprof文本指标及DTK rocprof等待counter，物理HCU4。严格核对6事件/shape、MNK、当前有效库哈希和参考kernel归属，CPU分析已完成。目录及analysis.json/ANALYSIS.md已下载本地。

| shape | VM完成等待CK/rocBLAS | LDS发射等待CK/rocBLAS |
|---|---:|---:|
| FC2 | 1.998 | 1.461 |
| QKV | 2.191 | 1.295 |
| out_proj | 5.916 | 1.249 |
| FC1 | 2.376 | 1.235 |

指标是wave聚合，不能当作毫秒、相加或作为时间占比。out_proj的VM等待差异明显，提示进一步检查数据供给/主循环，但不能仅凭该比率认定全部差额源于访存。完整样本范围、参考符号及限制见该目录分析。

## 已完成：BX固定N/K特化

三候选分别固定N+K、仅K、仅N；M仍运行时，其他尺寸保留通用实例。固定路径禁止整个长K循环自动展开。每候选含多个kernel实例；kernel_metadata.py已修正资源解析，避免只用首个实例代表整库。

四类真实shape的二进制分数、普通随机、finite、输出guard初筛全部通过；原有效配置compat重编译仍同SHA。以下为HIP-event轮均值中位数，完整原始JSON在`results/stage_bx_fixed_nk_specialization`，现已下载本地。

| TFLOPS | 原有效库 | 固定N+K | 仅固定K | 仅固定N |
|---|---:|---:|---:|---:|
| FC2 | 298.717 | 299.199 | 299.330 | 298.812 |
| QKV | 307.962 | 306.871 | 306.823 | 307.876 |
| out_proj | 285.472 | 286.149 | 284.657 | 285.499 |
| FC1 | 306.593 | 304.916 | 305.322 | 306.642 |

没有稳定整体收益，未采用，也未运行额外126组特化广审计。预留的--extra-fixed-shape-tails仅用于未来有收益候选的验收，不冒充已经完成。

## 已完成：BY最终库CTA遍历分组

对最终B rebase库重新比较分组1/2/4；不同于旧BK阶段较早的双tile库。四shape初筛均正确，原库compat同SHA。完整数据在`results/stage_by_final_bbase_cta_traversal`，现已下载本地。

| TFLOPS | 分组8控制 | 分组1 | 分组2 | 分组4 |
|---|---:|---:|---:|---:|
| FC2 | 298.627 | 282.462 | 289.555 | 293.206 |
| QKV | 307.889 | 270.650 | 272.701 | 301.138 |
| out_proj | 285.786 | 287.631 | 287.273 | 285.020 |
| FC1 | 306.426 | 244.787 | 273.057 | 300.468 |

out_proj分组1首次约0.64%时间降低，尚未独立复测/广审计；其余投影回退明显，不能全局开启。若后续采用必须验证专用dispatch本身的数值及计时。当前不采用。

## 已完成：固定M/N的短K曲线

M=18880、N=5376，当前有效库与F.linear同轮轮换。每点先做精确二进制分数和正常随机、finite、guard检查，均通过。原始结果在`results/short_k_curve_bbase_20261013`，现已下载本地。

| K | CK ms | rocBLAS ms | 差额ms |
|---|---:|---:|---:|
| 64 | 0.275416 | 0.239288 | 0.036128 |
| 128 | 0.328464 | 0.318832 | 0.009632 |
| 256 | 0.421720 | 0.382904 | 0.038816 |
| 512 | 0.604135 | 0.521000 | 0.083136 |
| 1024 | 0.938055 | 0.780231 | 0.157824 |
| 2048 | 1.565327 | 1.309015 | 0.256312 |
| 3584 | 2.546030 | 2.185158 | 0.360872 |
| 5376 | 3.657589 | 3.273293 | 0.384296 |
| 7168 | 4.870940 | 4.458220 | 0.412720 |

差额并非一个固定常数，不能只解释为启动/收尾成本。不同K可能选择不同原生kernel，短K event span也可能暴露launch gap；没有把两点或回归截距当作实际epilogue耗时。结果支持继续检查主循环和访存延迟隐藏。

## 已完成：BZ核心指令窗口/原生B bank地址

用户恢复额度后已经成功上传、编译并在HCU4完成四shape初筛及计时：

- native K64 B bank去掉scalar B rebase，分别在相位0/1预取并延后等待；
- native K64 B bank使用已验证的pow2 structured B寻址，分别在相位0/1预取；
- 当前有效K32路径的四条MMAC指令窗口，保留原第一条MMAC后priority位置；
- 四条MMAC窗口结合显式DS读取作为调度对照。

四MMAC helper使用read/write early-clobber输出，避免块内较早输出覆盖后续仍需要的输入。没有省略原来的VM完成等待、匹配barrier或终末drain。四真实shape的初筛数值全部通过，但六个新候选均更慢，不采用，也未为负收益候选追加102组广审计。

已上传文件：rolephase.cpp、phase_k64_native.hpp、mmac_group4.hpp、build.py、run_stage_bz.sh。脚本拒绝覆盖已有结果，最多两路CPU编译，GPU限定HCU4；原库compat同SHA。原始结果为`results/stage_bz_native_b64_addressing_and_mmac_window`，已同步本地。

历史中断：首次上传因自动审批额度不足而未执行，不是安全判定；在用户明确恢复额度后才重新通过审批上传和运行，没有绕过审批。该阻断现已解除，不再作为当前状态。

继续完成CA：静态wave组从4/4改为2/6、6/2，以及分组预取点4/8/12，共6候选；CB根据实际rocBLAS ISA把K64未来B预取移到前一段数学完成、下一段DS读取开始（点0）或点4，共4候选。两阶段四shape初筛全部正确，但没有稳定整体提升，保留原有效库。

恢复后新增BZ/CA/CB共16候选，当前81个完整计时阶段。最新全表、每个候选资源和同轮参考在[RESUMED_RESULTS_20261013.md](RESUMED_RESULTS_20261013.md)及同名JSON。全部终止，不保留本轮后台GPU实验；未commit/push，约330T目标仍未完成。

再续接完成CC–CF四阶段，现为85个完整计时阶段；14个构建配置（12个不同于有效库的新二进制）均无稳定整体收益。最新报告已包含四阶段的计时、资源与初筛验证；有效库、原始03_gemm和共享include不变。上文关于CB“根据实际ISA移到DS开始”的描述需要限定：点0/4是对照时机，完整原生顺序实际为下一相位先8条DS、未来4条B加载、再8条DS；点0/4并非精确原生调度。该纠正及指令地址已写入最新报告和LDS_WRAP_AND_PIPELINE.md第44节。

最新CG–CK已完成，现90个完整计时阶段。producer双tile单独不胜出，与权重K scalar offset组合只改善FC2/out_proj；shape_dispatch.cpp仅这两投影使用新核，另外两投影原路径不变。三producer各102组/306次，选择库126组/378次含小M尾部且与原版逐位一致；独立两轮FC2约0.48%、out_proj约0.38%–0.40%时间降低，另外两类基本持平。保留为已验证可选库，不全局启用、不替换默认22ef、不宣称E2E或330目标完成。详细证据见VALIDATED_PRODUCER_DISPATCH_20261013.md/JSON。所有本轮GPU任务已终止，目标保持未完成；未commit/push，暂不重排目录。

最新CL–CP现95个完整阶段，完成每K64 bank一次rebase、预取点/两基地址、两种共享startup、分相位等待五阶段共12候选，全部各102组/306次原版逐位广审计通过。家族内改善使最好K64达到约281–302T，但仍不如有效K32/已验证选择库，不采用。共享startup实际有spill，父版本compat逐byte相同，未删动态barrier/data wait。详细证据见B64_BANK_EXPERIMENTS_20261013.md/JSON及LDS_WRAP_AND_PIPELINE.md第46节。全部终止，目标约330T仍未完成，不commit/push。

再续接完成CQ/CR/CS，98个完整阶段。成对输出四候选总816组/2448次及feature128两候选总252组/756次均与原版逐位一致，但没有新收益，全部不采用。DS初始编码与raw DPP hazard失败均有冻结记录，修正后完整审计通过，未把失败计为性能阶段。CS资源减半但吞吐175–181T；full xprof原始.perf保留，同轮文本counter及DTK VM/barrier等待聚合对照已完成，不把聚合counter当毫秒比例。详见PAIRED_OUTPUT_EXPERIMENTS_20261013.md、FEATURE_TILE_EXPERIMENTS_20261013.md及各JSON。原有效库22ef和可选d2e仍保持，约330T未完成。

继续完成CT/CU两种tile/wave配套，CV guarded B scalar offset，CW A phase1 last-use预取四阶段，共8新候选各126组/378次原版逐位审计。4+2 wave仍慢、原tile2producer多数慢；B soffset在保留token voffset并将terminal bound置0后360组/720次位模式搬运验证通过，但完整核仍慢。A fragment stream1/4实际DS与MMAC交错0→64、VGPR225→241，无spill却回退，已采集同轮xprof/DTK指标。102个完整阶段，原有效库及可选选择库不变，目标未完成。分项报告WAVE_LAYOUT_EXPERIMENTS_20261013、B_SOFFSET_EXPERIMENTS_20261013、A_PHASE1_STREAM_EXPERIMENTS_20261013及其JSON；CV首次仅操作数顺序编译失败冻结保留，不计完整阶段。
