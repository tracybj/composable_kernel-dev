# Activation B scalar offset 实验（CV，2026-10-13）

## 算法与边界

async_b_soffset.hpp新增私有K32 helper：保留raw resource pointer，voffset=(token_row*K+(kk&31))*2，公共common=(kk&~31)*2进入soffset。不同于旧每K32 resource pointer+=common/bound-=common，新的pointer不改变，正常bank保持原token bounds。H3_PHASE_B_SOFFSET默认0，只测试原K32 bank、8+4 waves，不叠加scalar B rebase、cache remap或K64 bank。第二候选另外配固定producer pair及A soffset。

scalar offset不参加原voffset bounds，因此不能把generic M-tail load直接照搬。这里保留整个token row在checked voffset：row>=剩余token数始终越界；有效K32 phase在K%64和lane kk%8、lo<=24条件下，完整16byte vector位于该行范围内。终末unused common>=K*2时将resource bound置0，保证最后一行的K+payload不会越过物理分配末端。startup、current/future VM完成等待、terminal pending vector live与VM drain全部保留。不是泛化任意kk、任意stride的接口。

未改变原API输入域、FP32累加顺序、BF16 RNE、output bounded短写。unused terminal payload由旧跨行可能读取变为全零，仅内部未消费bank不同；最终输出必须与旧库逐位一致。新 helper未用于任意图像/其它模型路径。

## 验证

独立raw int16搬运（不是量化）：M剩余1/3/63/192/256、K64/192/320/7168，首/中/末K32 lane片段及K/K+24终末、3seed、2repeat，共360组/720次，输入不变、输出prefix/tail guard、有效原位模式和invalid row/terminal全零全部通过。输入额外16行随机位模式guard（不是预填全零），降低越界读恰好读到零而漏报的风险。

两个完整GEMM候选各126组/378次，含odd K tile和真实N/K的小M尾部，原版逐位一致、输入/输出guard/finite/三次确定性/FP32参考全部通过。原版比较和FP32参考额外launch不计入此数。默认compat逐byte同22ef。

首次仅编译失败（MUBUF Vaddr/Srsrc操作数写反），未运行GPU；更正后构建/probe/广审计成功，冻结的build/attempts错误日志保留，不算完整阶段。

## 同轮四shape计时

HCU4 / ck_yyc / gfx936 / DTK25.10.0-0；M18880，BF16输入输出、FP32累加。5预热、6轮轮换×10 HIP events，取轮均值中位数、关闭profiler。Δtime>0表示较BASE慢。

| shape | candidate | ms | TFLOPS | Δtime vs BASE |
|---|---|---:|---:|---:|---:|
| fc2 | BASE | 4.869236 | 298.833 | +0.000% |
| fc2 | shape dispatch | 4.850420 | 299.992 | -0.386% |
| fc2 | B soffset | 4.899244 | 297.003 | +0.616% |
| fc2 | B soffset+producer pair/A soffset | 4.879036 | 298.233 | +0.201% |
| qkv | BASE | 7.090971 | 307.804 | +0.000% |
| qkv | shape dispatch | 7.085915 | 308.024 | -0.071% |
| qkv | B soffset | 7.129011 | 306.162 | +0.536% |
| qkv | B soffset+producer pair/A soffset | 7.142466 | 305.585 | +0.726% |
| out_proj | BASE | 2.548942 | 285.430 | +0.000% |
| out_proj | shape dispatch | 2.538534 | 286.600 | -0.408% |
| out_proj | B soffset | 2.562510 | 283.918 | +0.532% |
| out_proj | B soffset+producer pair/A soffset | 2.555614 | 284.685 | +0.262% |
| fc1 | BASE | 9.494665 | 306.506 | +0.000% |
| fc1 | shape dispatch | 9.495321 | 306.485 | +0.007% |
| fc1 | B soffset | 9.535105 | 305.206 | +0.426% |
| fc1 | B soffset+producer pair/A soffset | 9.550345 | 304.719 | +0.586% |

## 实际资源和ISA

| candidate | VGPR | SGPR | LDS bytes | scratch | spill | B load soffset-sreg static |
|---|---:|---:|---:|---:|---:|---:|
| BASE | 225 | 31 | 65536 | 0 | 0 | 0 |
| B soffset | 225 | 31 | 65536 | 0 | 0 | 20 |
| 组合 | 225 | 31 | 65536 | 0 | 0 | 20 |

静态inventory包含两个互斥wave-group body；不能当每tile动态指令数量/毫秒比例。SOFFSET_SREG证明真实进入s寄存器操作数，不仅是C++函数名字变动。

## 结论

没有四shape整体改善，不替换当前有效库。个别shape差异需独立复测，不能使用跨轮最低值宣称新有效版本。
330TFLOPS目标仍未完成，03_gemm/include/H3不变；无commit/push、无模型E2E结论。

