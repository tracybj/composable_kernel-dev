# GEMM 112阶段逐项记录（2026-10-12—14）

覆盖2026-10-12 16:22至10-14 11:27的已登记正式阶段；按environment.date排序，不按目录字典序。文件日期是结果登记/测试时间，独立counter、probe和失败构建另列。

所有表格顺序为FC2/QKV/out_proj/FC1，每格“毫秒 / TFLOPS”；同轮rocBLAS linear和mm_out均保留。T按2×M×N×K计算。`未计时`表示没有有效性能样本，绝非0；同SHA别名不重复计时。

“本轮数值初筛通过”只表示results.json里的已测分布检查；不自动等同广审计、任意输入bitwise一致或已采用。每阶段的文字结论及专门审计记录限定采用范围。

当前源码链接已核对本地副本与服务器当前文件哈希一致，但不表示当前源码等于早期实测源码；每个V编号链接到匹配实测库哈希的构建/冻结快照索引，便于区分同一rolephase.cpp的多次历史修改。

[总览与指标解释](GEMM_实验迭代总览_20261012-14.md) · [293个库的源码索引](GEMM_源码与构建索引.md)

## 阶段导航

- [001 / A：起点与旧 LDS swizzle](#stage-001) — 2026-10-12T16:22:38 / `stage_a_layout`
- [002 / B：输出 tile / K tile / wave 组合](#stage-002) — 2026-10-12T16:31:23 / `stage_b_tiles`
- [003 / C：LDS barrier 与未来 VMEM 等待解耦](#stage-003) — 2026-10-12T16:39:32 / `stage_c_raw_barrier`
- [004 / D：展开的双 LDS buffer](#stage-004) — 2026-10-12T16:47:00 / `stage_d_double_buffer`
- [005 / E：LDS 行跨度 padding](#stage-005) — 2026-10-12T16:50:46 / `stage_e_lds_padding`
- [006 / G：更小 warp K 片段](#stage-006) — 2026-10-12T16:59:37 / `stage_g_warp_k16`
- [007 / F：动态双 buffer 主循环](#stage-007) — 2026-10-12T17:05:55 / `stage_f_dynamic_double`
- [008 / H：CTA 按 token 分组遍历](#stage-008) — 2026-10-12T17:12:08 / `stage_h_tile_order`
- [009 / I：K-pack XOR 布局](#stage-009) — 2026-10-12T17:19:42 / `stage_i_pack_xor`
- [010 / J：launch bounds 约束](#stage-010) — 2026-10-12T17:28:40 / `stage_j_launch_bounds`
- [011 / K：N 方向 lane/interleave](#stage-011) — 2026-10-12T17:37:11 / `stage_k_n_interleave`
- [012 / L：平衡 wave 分区与操作数复用](#stage-012) — 2026-10-12T17:41:18 / `stage_l_balanced_operands`
- [013 / M：LDS K-pack 主序](#stage-013) — 2026-10-12T17:50:55 / `stage_m_kpack_major`
- [014 / N：8 compute + 4 producer wave 原型](#stage-014) — 2026-10-12T18:24:40 / `stage_n_wave_specialized`
- [015 / O：解除候选的寄存器池编译限制](#stage-015) — 2026-10-12T18:32:18 / `stage_o_register_pool`
- [016 / P：大 tile 配套布局/流水](#stage-016) — 2026-10-12T18:37:51 / `stage_p_large_pipeline`
- [017 / Q：分工原型的 padding / Kpack / direct-to-LDS](#stage-017) — 2026-10-12T18:39:10 / `stage_q_specialized_layout`
- [018 / R：B buffer load 与 wave priority](#stage-018) — 2026-10-12T18:43:36 / `stage_r_buffer_priority`
- [019 / S：tile 长宽比、BK64 与不支持尺寸](#stage-019) — 2026-10-12T18:49:39 / `stage_s_aspect_k64`
- [020 / U：12 数学 wave 与真实 M 尾行](#stage-020) — 2026-10-12T19:03:58 / `stage_u_12_math_waves`
- [021 / U：12 数学 wave 与真实 M 尾行](#stage-021) — 2026-10-12T19:04:28 / `stage_u_native_m18880`
- [022 / V：m0 wrap 与 K64 分工流水](#stage-022) — 2026-10-12T19:16:43 / `stage_v_k64_wrapped_pipeline`
- [023 / W：MMAC 发射与累加器初始化](#stage-023) — 2026-10-12T19:25:27 / `stage_w_mmac_order`
- [024 / X：8 数学 wave 协作搬运](#stage-024) — 2026-10-12T19:32:25 / `stage_x_cooperative_prefetch`
- [025 / Y：早期结构化 buffer / Cache Swizzle](#stage-025) — 2026-10-12T19:38:30 / `stage_y_structured_cache_swizzle`
- [026 / Z：结构化加载的等待位置](#stage-026) — 2026-10-12T19:42:46 / `stage_z_structured_wait_tracking`
- [027 / AA：BK16 / WarpK16 缩小工作集](#stage-027) — 2026-10-12T19:55:02 / `stage_aa_k16_residency`
- [028 / AB：priority 与编译调度屏障对照](#stage-028) — 2026-10-12T20:01:01 / `stage_ab_scheduling_controls`
- [029 / AC：对齐 rocBLAS 的内部操作数方向](#stage-029) — 2026-10-12T20:07:21 / `stage_ac_blas_operand_orientation`
- [030 / AC：对齐 rocBLAS 的内部操作数方向](#stage-030) — 2026-10-12T20:12:20 / `stage_ac_native_m18880`
- [031 / AD：通用 CK 的列主序输出](#stage-031) — 2026-10-12T20:22:20 / `stage_ad_generic_column_output`
- [032 / AE：VGPR 起点奇偶对照](#stage-032) — 2026-10-12T20:37:20 / `stage_ae_vgpr_allocation_shift`
- [033 / AF：恢复连续 K 的 vec8 读取](#stage-033) — 2026-10-13T10:22:57 / `stage_af_contiguous_vector_restore`
- [034 / AF：恢复连续 K 的 vec8 读取](#stage-034) — 2026-10-13T10:25:06 / `stage_af_native_m18880`
- [035 / AG：正确 A/B 方向下重测 cache](#stage-035) — 2026-10-13T10:26:28 / `stage_ag_transposed_cache_swizzle`
- [036 / AH：向量化恢复后的 tile 遍历](#stage-036) — 2026-10-13T10:31:53 / `stage_ah_vector_group_tile`
- [037 / AI：K32 相位预取与 1×8 wave 分区](#stage-037) — 2026-10-13T10:36:14 / `stage_ai_phase_register_prefetch`
- [038 / AJ：K32 相位流水的 cache 对照](#stage-038) — 2026-10-13T10:41:51 / `stage_aj_phase_cache_swizzle`
- [039 / AK：B 寄存器双 bank + 显式局部等待](#stage-039) — 2026-10-13T10:50:46 / `stage_ak_banked_wait_schedule`
- [040 / AL：权重 K32 / 32KiB LDS](#stage-040) — 2026-10-13T11:00:09 / `stage_al_weight_k32_buffer`
- [041 / AM：producer 发射推迟与相位对照](#stage-041) — 2026-10-13T11:21:48 / `stage_am_ready_wait_and_phase_stagger`
- [042 / AN：显式 DS 顺序 / 渐进 LGKM wait](#stage-042) — 2026-10-13T11:29:38 / `stage_an_ordered_lds_reads`
- [043 / AO：额外活跃 VGPR 对齐试探](#stage-043) — 2026-10-13T11:34:01 / `stage_ao_phase_vgpr_alignment`
- [044 / AP：DS 中间插入未来 B 预取](#stage-044) — 2026-10-13T11:40:33 / `stage_ap_mixed_prefetch_and_structured_buffer`
- [045 / AQ：移除热循环 terminal 预取分支](#stage-045) — 2026-10-13T11:43:47 / `stage_aq_branchfree_prefetch_and_unroll`
- [046 / AR：交错预取重建与独立复测](#stage-046) — 2026-10-13T11:46:48 / `stage_ar_mixed_prefetch_rebuild_verify`
- [047 / AS：mixed + always 组合](#stage-047) — 2026-10-13T11:51:42 / `stage_as_combined_mixed_branchfree`
- [048 / AT：组合流水独立复测](#stage-048) — 2026-10-13T11:54:12 / `stage_at_combined_prefetch_independent`
- [049 / AU：有界 buffer 输出](#stage-049) — 2026-10-13T12:03:34 / `stage_au_bounded_buffer_output`
- [050 / AV：xprof 指导的预取点4/12](#stage-050) — 2026-10-13T12:26:47 / `stage_av_xprof_guided_prefetch_points`
- [051 / AW：限制跨相位调度与 math burst](#stage-051) — 2026-10-13T12:36:36 / `stage_aw_mmac_burst_boundaries`
- [052 / AX：完整 K64 激活双 bank](#stage-052) — 2026-10-13T12:41:58 / `stage_ax_k64_activation_banks`
- [053 / AY：branchless BF16 RNE](#stage-053) — 2026-10-13T12:45:55 / `stage_ay_branchless_bf16_output`
- [054 / AZ：RNE 独立验证](#stage-054) — 2026-10-13T12:48:31 / `stage_az_branchless_rne_independent`
- [055 / BA：producer/consumer cadence](#stage-055) — 2026-10-13T12:59:23 / `stage_ba_producer_consumer_cadence`
- [056 / BB：cadence 独立复测](#stage-056) — 2026-10-13T13:02:29 / `stage_bb_cadence_independent`
- [057 / BC：pow2 stride + row remainder](#stage-057) — 2026-10-13T13:27:26 / `stage_bc_native_pow2_b`
- [058 / BD：A 标量 base / 显式 uniform wave](#stage-058) — 2026-10-13T13:32:38 / `stage_bd_scalar_addressing`
- [059 / BE：scalar-wave 上的 live-pad](#stage-059) — 2026-10-13T13:40:23 / `stage_be_scalar_wave_livepads`
- [060 / BF：wave 组 barrier 错位](#stage-060) — 2026-10-13T13:52:08 / `stage_bf_wave_group_barrier_stagger`
- [061 / BG：显式双 K64 tile 展开](#stage-061) — 2026-10-13T14:07:08 / `stage_bg_explicit_pair_tiles`
- [062 / BH：双 tile 独立复测](#stage-062) — 2026-10-13T14:14:06 / `stage_bh_pair_tiles_independent`
- [063 / BI：A 搬运 m0 burst 初稿](#stage-063) — 2026-10-13T14:18:55 / `stage_bi_scalar_m0_burst`
- [064 / BJ：补齐 m0/SCC clobber](#stage-064) — 2026-10-13T14:26:41 / `stage_bj_aburst_declared_clobbers`
- [065 / BK：二维 grid 与 token group](#stage-065) — 2026-10-13T14:57:56 / `stage_bk_grid_and_token_groups`
- [066 / BL：静态 wave 分组 + post-priority](#stage-066) — 2026-10-13T15:09:52 / `stage_bl_static_wave_groups`
- [067 / BM：静态分组独立复测](#stage-067) — 2026-10-13T15:13:27 / `stage_bm_static_wave_independent`
- [068 / BN：新流水的 LGKM / DS / 预取对照](#stage-068) — 2026-10-13T15:18:14 / `stage_bn_static_wave_lds_schedule`
- [069 / BO：K32 B resource 标量 rebase](#stage-069) — 2026-10-13T15:26:46 / `stage_bo_static_wave_b_prefetch`
- [070 / BP：B rebase 独立复测](#stage-070) — 2026-10-13T15:31:54 / `stage_bp_scalar_b_base_independent`
- [071 / BQ：FP class RNE / 新预取点](#stage-071) — 2026-10-13T15:34:37 / `stage_bq_fpclass_rne_and_b_prefetch`
- [072 / BS：累加器子 bank 约束](#stage-072) — 2026-10-13T15:41:35 / `stage_bs_accumulator_subbanks`
- [073 / BT：新基线 producer / priority 重测](#stage-073) — 2026-10-13T15:48:10 / `stage_bt_producer_priority_balance`
- [074 / BU：保留低号寄存器改变起点](#stage-074) — 2026-10-13T15:52:53 / `stage_bu_low_register_constraints`
- [075 / BV：native cadence 上的 K64 B bank](#stage-075) — 2026-10-13T16:07:55 / `stage_bv_native_k64_activation_banks`
- [076 / BW：K64 B 完成等待延后](#stage-076) — 2026-10-13T16:12:43 / `stage_bw_native_k64_late_wait`
- [077 / BX：固定 N/K 特化](#stage-077) — 2026-10-13T17:23:26 / `stage_bx_fixed_nk_specialization`
- [078 / BY：最终 raw 基线的 CTA 遍历](#stage-078) — 2026-10-13T17:27:02 / `stage_by_final_bbase_cta_traversal`
- [079 / BZ：K64 地址 + 四 MMAC 指令窗口](#stage-079) — 2026-10-13T18:30:12 / `stage_bz_native_b64_addressing_and_mmac_window`
- [080 / CA：静态 wave 组比例与预取点](#stage-080) — 2026-10-13T18:33:46 / `stage_ca_wave_group_prefetch_balance`
- [081 / CB：相位之间更早预取 K64 B](#stage-081) — 2026-10-13T18:37:46 / `stage_cb_native_prefetch_between_math_phases`
- [082 / CC：A 的两组 LDS 基地址](#stage-082) — 2026-10-13T18:49:33 / `stage_cc_two_lds_bases_and_immediate_reads`
- [083 / CD：B 完成等待与 barrier 解耦](#stage-083) — 2026-10-13T18:53:57 / `stage_cd_b_wait_and_barrier_decoupling`
- [084 / CE：独立 C tile 的 MMAC 顺序](#stage-084) — 2026-10-13T18:58:58 / `stage_ce_independent_mmac_tile_order`
- [085 / CF：producer 终末分支](#stage-085) — 2026-10-13T19:02:02 / `stage_cf_producer_terminal_branch`
- [086 / CG：producer 固定槽双 tile](#stage-086) — 2026-10-13T19:12:06 / `stage_cg_producer_fixed_slot_pairs`
- [087 / CH：权重 K 使用 scalar offset](#stage-087) — 2026-10-13T19:17:57 / `stage_ch_producer_scalar_offset`
- [088 / CI：producer 组合独立复测](#stage-088) — 2026-10-13T19:22:29 / `stage_ci_producer_scalar_offset_independent`
- [089 / CJ：两投影 producer 分派](#stage-089) — 2026-10-13T19:28:33 / `stage_cj_two_projection_producer_dispatch`
- [090 / CK：producer 分派独立复测](#stage-090) — 2026-10-13T19:31:12 / `stage_ck_shape_dispatch_independent`
- [091 / CL：每个 K64 bank 单次 rebase](#stage-091) — 2026-10-13T19:51:19 / `stage_cl_native_b64_single_rebase`
- [092 / CM：K64 rebase 配套调度](#stage-092) — 2026-10-13T19:58:04 / `stage_cm_native_b64_rebase_schedule`
- [093 / CN：K64 B 共享 startup](#stage-093) — 2026-10-13T20:02:31 / `stage_cn_native_b64_shared_startup`
- [094 / CO：只共享不可变 B 初始值](#stage-094) — 2026-10-13T20:12:09 / `stage_co_native_b64_immutable_initial`
- [095 / CP：K64 按相位局部等待](#stage-095) — 2026-10-13T20:24:04 / `stage_cp_native_b64_phase_waits`
- [096 / CQ：相邻 feature 的 BF16 成对输出](#stage-096) — 2026-10-13T21:18:36 / `stage_cq_paired_feature_output_builtin`
- [097 / CR：对齐分支提到整个 epilogue 外](#stage-097) — 2026-10-13T21:27:44 / `stage_cr_paired_output_alignment_hoist`
- [098 / CS：feature tile256→128](#stage-098) — 2026-10-13T21:43:17 / `stage_cs_half_feature_tile`
- [099 / CT：小 tile 改配4+2 wave](#stage-099) — 2026-10-13T22:02:32 / `stage_ct_half_tile_six_waves`
- [100 / CU：原 tile 仅减至2 producer](#stage-100) — 2026-10-13T22:08:25 / `stage_cu_original_tile_two_producers`
- [101 / CV：激活 B 的 K scalar offset](#stage-101) — 2026-10-13T22:18:40 / `stage_cv_activation_scalar_offset`
- [102 / CW：last-use 后提前 phase1 A 读取](#stage-102) — 2026-10-13T22:30:40 / `stage_cw_phase1_a_streaming`
- [103 / CX：缩小跨相位 A 预取窗口](#stage-103) — 2026-10-13T22:48:07 / `stage_cx_partial_a_phase1_windows`
- [104 / CY：K64 同 row 发射顺序 / immediate](#stage-104) — 2026-10-13T23:01:40 / `stage_cy_k64_row_order_immediate`
- [105 / CZ：K64 结构化 bit62 OFF/ON 配对](#stage-105) — 2026-10-14T10:31:07 / `stage_cz_struct64_bit62_20261014`
- [106 / DA：K64 Cache Swizzle 独立复测](#stage-106) — 2026-10-14T10:34:58 / `stage_da_struct64_independent_20261014`
- [107 / DB：K64 cache 仅用于 out_proj](#stage-107) — 2026-10-14T10:39:40 / `stage_db_cache_shape_dispatch_20261014`
- [108 / DC：K64 cache 分派独立复测](#stage-108) — 2026-10-14T10:42:54 / `stage_dc_cache_dispatch_independent_20261014`
- [109 / DD：把结构化 cache 搭配快的 K32 流水](#stage-109) — 2026-10-14T11:17:52 / `stage_dd_struct32_cache_pair_20261014`
- [110 / DE：K32 Cache Swizzle 独立复测](#stage-110) — 2026-10-14T11:21:25 / `stage_de_struct32_independent_20261014`
- [111 / DF：最终 K32 统一入口](#stage-111) — 2026-10-14T11:24:47 / `stage_df_struct32_dispatch_20261014`
- [112 / DG：最终统一入口独立复测](#stage-112) — 2026-10-14T11:27:21 / `stage_dg_struct32_dispatch_independent_20261014`

<a id="stage-001"></a>
## 001 / A：起点与旧 LDS swizzle

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_a_layout`；时间：2026-10-12T16:22:38.808981+08:00。

**改了什么及结论：** 从03派生128×128×32的BF16 GEMM，比较普通policy与旧LDS swizzle；保持矩阵数学和输入布局。旧swizzle正确但回退，不能与后期descriptor bit62 Cache Swizzle混为一谈。

**源码位置：** [gemm.cpp](<../gemm.cpp>)；[policy.hpp](<../policy.hpp>)。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4523 / 327.93 | 6.6537 / 329.14 | 2.1880 / 333.64 | 8.8892 / 328.49 | 库对照 |
| rocblas_mm_out | 4.4618 / 327.23 | 6.6747 / 328.11 | 2.1862 / 333.92 | 8.8913 / 328.41 | 库对照 |
| [V001](GEMM_源码与构建索引.md#v001) | 21.5086 / 67.88 | 33.8878 / 64.63 | 8.4760 / 86.13 | 45.1404 / 64.69 | 通过 |
| [V002](GEMM_源码与构建索引.md#v002) | 25.2198 / 57.89 | 37.3572 / 58.62 | 10.1274 / 72.08 | 50.0972 / 58.29 | 通过 |

**xprof / DTK 表现：** base作为早期DTK对照：MMOP178.23M、LDS77.97M、bank-conflict445.56M（部分emulated），不能视为实际stall周期。没有本阶段旧swizzle的独立counter对照。 证据P00：[RESULTS.md](<../RESULTS.md>)。

**候选对应关系与配置：**

- [V001](GEMM_源码与构建索引.md#v001) `base`：本索引首次出现该哈希。
- [V002](GEMM_源码与构建索引.md#v002) `swizzle`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_a_layout/results.json>)；[environment.json](<../results/stage_a_layout/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-002"></a>
## 002 / B：输出 tile / K tile / wave 组合

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_b_tiles`；时间：2026-10-12T16:31:23.325288+08:00。

**改了什么及结论：** 比较128×256、256×128、256×256及BK64等组合，提高操作数复用与每CTA算术量。宽tile提升明显；早期大tile受寄存器spill影响，不能仅按tile越大越好选择。

**源码位置：** [gemm.cpp](<../gemm.cpp>)；[policy.hpp](<../policy.hpp>)；[build.py](<../build.py>)。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4716 / 326.51 | 6.6660 / 328.54 | 2.2036 / 331.28 | 8.9078 / 327.81 | 库对照 |
| rocblas_mm_out | 4.4658 / 326.93 | 6.6450 / 329.57 | 2.1838 / 334.29 | 8.9026 / 328.00 | 库对照 |
| [V001](GEMM_源码与构建索引.md#v001) | 21.6345 / 67.49 | 33.9254 / 64.55 | 8.5090 / 85.79 | 45.0923 / 64.76 | 通过 |
| [V003](GEMM_源码与构建索引.md#v003) | 20.9000 / 69.86 | 33.6784 / 65.03 | 8.4791 / 86.10 | 45.3167 / 64.44 | 通过 |
| [V004](GEMM_源码与构建索引.md#v004) | 13.9020 / 105.02 | 20.5585 / 106.53 | 7.0289 / 103.86 | 27.4186 / 106.50 | 通过 |
| [V005](GEMM_源码与构建索引.md#v005) | 15.7756 / 92.55 | 23.5189 / 93.12 | 7.1817 / 101.65 | 31.4308 / 92.90 | 通过 |
| [V006](GEMM_源码与构建索引.md#v006) | 128.6938 / 11.34 | 193.7710 / 11.30 | 63.7619 / 11.45 | 259.2458 / 11.26 | 通过 |
| [V007](GEMM_源码与构建索引.md#v007) | 21.2114 / 68.83 | 32.0665 / 68.30 | 10.8641 / 67.19 | 42.7719 / 68.27 | 通过 |
| [V008](GEMM_源码与构建索引.md#v008) | 24.8321 / 58.80 | 37.5270 / 58.36 | 12.5270 / 58.27 | 49.7658 / 58.68 | 通过 |
| [V009](GEMM_源码与构建索引.md#v009) | 122.2801 / 11.94 | 183.5174 / 11.93 | 61.1498 / 11.94 | 245.0248 / 11.92 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V001](GEMM_源码与构建索引.md#v001) `base`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V003](GEMM_源码与构建索引.md#v003) `balanced`：本索引首次出现该哈希。
- [V004](GEMM_源码与构建索引.md#v004) `wide`：本索引首次出现该哈希。
- [V005](GEMM_源码与构建索引.md#v005) `tall`：本索引首次出现该哈希。
- [V006](GEMM_源码与构建索引.md#v006) `large`：本索引首次出现该哈希。
- [V007](GEMM_源码与构建索引.md#v007) `k64`：本索引首次出现该哈希。
- [V008](GEMM_源码与构建索引.md#v008) `wide_k64`：本索引首次出现该哈希。
- [V009](GEMM_源码与构建索引.md#v009) `large_k64`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_b_tiles/results.json>)；[environment.json](<../results/stage_b_tiles/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-003"></a>
## 003 / C：LDS barrier 与未来 VMEM 等待解耦

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_c_raw_barrier`；时间：2026-10-12T16:39:32.698312+08:00。

**改了什么及结论：** 在私有同步路径中使用局部LDS barrier，避免当前块计算前提前vmcnt(0)等待下一块global load。仍保留跨wave LDS同步和消费依赖；宽tile得到提升。

**源码位置：** [gemm.cpp](<../gemm.cpp>)；[policy.hpp](<../policy.hpp>)。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4696 / 326.66 | 6.6523 / 329.22 | 2.1885 / 333.57 | 8.9088 / 327.77 | 库对照 |
| rocblas_mm_out | 4.4543 / 327.78 | 6.6594 / 328.86 | 2.1870 / 333.79 | 8.8795 / 328.85 | 库对照 |
| [V001](GEMM_源码与构建索引.md#v001) | 21.6547 / 67.42 | 33.8734 / 64.65 | 8.5116 / 85.77 | 45.1749 / 64.64 | 通过 |
| [V004](GEMM_源码与构建索引.md#v004) | 13.8794 / 105.19 | 20.5612 / 106.51 | 7.0250 / 103.92 | 27.4214 / 106.49 | 通过 |
| [V010](GEMM_源码与构建索引.md#v010) | 22.8828 / 63.80 | 31.2526 / 70.08 | 8.3428 / 87.50 | 39.7867 / 73.39 | 通过 |
| [V011](GEMM_源码与构建索引.md#v011) | 12.4500 / 117.27 | 17.6592 / 124.02 | 6.0755 / 120.16 | 23.6796 / 123.31 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V001](GEMM_源码与构建索引.md#v001) `base`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V004](GEMM_源码与构建索引.md#v004) `wide`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V010](GEMM_源码与构建索引.md#v010) `base_raw_barrier`：本索引首次出现该哈希。
- [V011](GEMM_源码与构建索引.md#v011) `wide_raw_barrier`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_c_raw_barrier/results.json>)；[environment.json](<../results/stage_c_raw_barrier/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-004"></a>
## 004 / D：展开的双 LDS buffer

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_d_double_buffer`；时间：2026-10-12T16:47:00.828088+08:00。

**改了什么及结论：** 为相邻K tile设置两个LDS槽并尝试展开生产/消费，意图覆盖global load延迟。增加缓冲和寄存器生命周期带来的成本抵消预期收益；未成为最终路线。

**源码位置：** [double_buffer.hpp](<../double_buffer.hpp>)；[gemm.cpp](<../gemm.cpp>)。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4483 / 328.22 | 6.6834 / 327.68 | 2.1910 / 333.18 | 8.8917 / 328.40 | 库对照 |
| rocblas_mm_out | 4.4206 / 330.27 | 6.6631 / 328.68 | 2.1852 / 334.08 | 8.8938 / 328.32 | 库对照 |
| [V001](GEMM_源码与构建索引.md#v001) | 21.6036 / 67.58 | 33.5966 / 65.19 | 8.5085 / 85.80 | 44.5817 / 65.50 | 通过 |
| [V011](GEMM_源码与构建索引.md#v011) | 12.4610 / 117.17 | 17.6678 / 123.96 | 6.0745 / 120.18 | 23.6850 / 123.29 | 通过 |
| [V012](GEMM_源码与构建索引.md#v012) | 12.0848 / 120.81 | 16.8544 / 129.94 | 5.7703 / 126.51 | 22.5804 / 129.32 | 通过 |
| [V013](GEMM_源码与构建索引.md#v013) | 16.9549 / 86.11 | 23.9562 / 91.42 | 7.6125 / 95.90 | 32.1021 / 90.96 | 通过 |
| [V014](GEMM_源码与构建索引.md#v014) | 17.2653 / 84.56 | 24.8416 / 88.16 | 7.9449 / 91.88 | 33.1979 / 87.96 | 通过 |
| [V015](GEMM_源码与构建索引.md#v015) | 82.3379 / 17.73 | 121.7481 / 17.99 | 39.7030 / 18.39 | 162.7558 / 17.94 | 通过 |
| [V016](GEMM_源码与构建索引.md#v016) | 35.0002 / 41.71 | 52.8179 / 41.46 | 17.6128 / 41.45 | 70.6645 / 41.32 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V001](GEMM_源码与构建索引.md#v001) `base`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V011](GEMM_源码与构建索引.md#v011) `wide_raw_barrier`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V012](GEMM_源码与构建索引.md#v012) `wide4_raw_barrier`：本索引首次出现该哈希。
- [V013](GEMM_源码与构建索引.md#v013) `tall4_raw_barrier`：本索引首次出现该哈希。
- [V014](GEMM_源码与构建索引.md#v014) `base_double`：本索引首次出现该哈希。
- [V015](GEMM_源码与构建索引.md#v015) `wide_double`：本索引首次出现该哈希。
- [V016](GEMM_源码与构建索引.md#v016) `wide4_double`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_d_double_buffer/results.json>)；[environment.json](<../results/stage_d_double_buffer/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-005"></a>
## 005 / E：LDS 行跨度 padding

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_e_lds_padding`；时间：2026-10-12T16:50:46.541422+08:00。

**改了什么及结论：** 改变LDS物理行跨度，比较padding8/16，逻辑矩阵和输出不变。padding16改善本阶段宽tile；它改变布局及读指令，不是硬件cache swizzle。

**源码位置：** [padded_policy.hpp](<../padded_policy.hpp>)；[gemm.cpp](<../gemm.cpp>)。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4633 / 327.12 | 6.6754 / 328.07 | 2.1920 / 333.03 | 8.9224 / 327.27 | 库对照 |
| rocblas_mm_out | 4.4520 / 327.95 | 6.6631 / 328.68 | 2.1863 / 333.90 | 8.8882 / 328.53 | 库对照 |
| [V001](GEMM_源码与构建索引.md#v001) | 21.5315 / 67.81 | 33.8490 / 64.70 | 8.4659 / 86.23 | 45.1976 / 64.61 | 通过 |
| [V011](GEMM_源码与构建索引.md#v011) | 12.4627 / 117.15 | 17.6831 / 123.85 | 6.0748 / 120.17 | 23.6823 / 123.30 | 通过 |
| [V017](GEMM_源码与构建索引.md#v017) | 21.2231 / 68.79 | 26.1466 / 83.76 | 8.1200 / 89.90 | 34.6682 / 84.23 | 通过 |
| [V018](GEMM_源码与构建索引.md#v018) | 12.5784 / 116.07 | 17.6267 / 124.25 | 6.1463 / 118.77 | 23.5870 / 123.80 | 通过 |
| [V019](GEMM_源码与构建索引.md#v019) | 10.5635 / 138.21 | 16.1383 / 135.70 | 5.6460 / 129.30 | 21.5697 / 135.38 | 通过 |
| [V020](GEMM_源码与构建索引.md#v020) | 13.3041 / 109.74 | 18.5933 / 117.79 | 6.1494 / 118.71 | 24.7452 / 118.00 | 通过 |

**xprof / DTK 表现：** 另采wide_pad16与base/rocBLAS：LDS77.97M→72.40M、bank-conflict445.56M→133.67M。DTK报告量部分emulated，不表示等待时间降低同样比例；不是E所有候选均采了counter。 证据P00：[RESULTS.md](<../RESULTS.md>)。

**候选对应关系与配置：**

- [V001](GEMM_源码与构建索引.md#v001) `base`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V011](GEMM_源码与构建索引.md#v011) `wide_raw_barrier`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V017](GEMM_源码与构建索引.md#v017) `base_pad8`：本索引首次出现该哈希。
- [V018](GEMM_源码与构建索引.md#v018) `wide_pad8`：本索引首次出现该哈希。
- [V019](GEMM_源码与构建索引.md#v019) `wide_pad16`：本索引首次出现该哈希。
- [V020](GEMM_源码与构建索引.md#v020) `wide4_pad8`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_e_lds_padding/results.json>)；[environment.json](<../results/stage_e_lds_padding/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-006"></a>
## 006 / G：更小 warp K 片段

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_g_warp_k16`；时间：2026-10-12T16:59:37.767595+08:00。

**改了什么及结论：** 比较BK16/WarpK16与原K片段，降低瞬时操作数工作集。同步及循环摊销同时增加，本阶段没有优于已有配置。

**源码位置：** [warp_variants.hpp](<../warp_variants.hpp>)；[gemm.cpp](<../gemm.cpp>)；[policy.hpp](<../policy.hpp>)。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4649 / 327.00 | 6.6655 / 328.56 | 2.1960 / 332.43 | 8.8811 / 328.79 | 库对照 |
| rocblas_mm_out | 4.4568 / 327.59 | 6.6380 / 329.92 | 2.1875 / 333.72 | 8.8984 / 328.15 | 库对照 |
| [V001](GEMM_源码与构建索引.md#v001) | 21.6272 / 67.51 | 33.5673 / 65.24 | 8.5101 / 85.78 | 44.6338 / 65.42 | 通过 |
| [V019](GEMM_源码与构建索引.md#v019) | 10.5659 / 138.18 | 16.1512 / 135.60 | 5.6499 / 129.21 | 21.6048 / 135.16 | 通过 |
| [V021](GEMM_源码与构建索引.md#v021) | 27.0243 / 54.03 | 34.9618 / 62.64 | 9.5652 / 76.32 | 45.3286 / 64.42 | 通过 |
| [V022](GEMM_源码与构建索引.md#v022) | 31.4634 / 46.40 | 47.1116 / 46.49 | 15.9079 / 45.89 | 62.7765 / 46.51 | 通过 |
| [V023](GEMM_源码与构建索引.md#v023) | 21.2208 / 68.80 | 31.8620 / 68.73 | 10.9482 / 66.68 | 42.4607 / 68.77 | 通过 |
| [V024](GEMM_源码与构建索引.md#v024) | 20.7341 / 70.42 | 31.3074 / 69.95 | 10.7818 / 67.71 | 41.7310 / 69.97 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V001](GEMM_源码与构建索引.md#v001) `base`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V019](GEMM_源码与构建索引.md#v019) `wide_pad16`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V021](GEMM_源码与构建索引.md#v021) `wide4_k16`：本索引首次出现该哈希。
- [V022](GEMM_源码与构建索引.md#v022) `wide4_wk16`：本索引首次出现该哈希。
- [V023](GEMM_源码与构建索引.md#v023) `wide4_wk16_pad16`：本索引首次出现该哈希。
- [V024](GEMM_源码与构建索引.md#v024) `wide_wk16_pad16`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_g_warp_k16/results.json>)；[environment.json](<../results/stage_g_warp_k16/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-007"></a>
## 007 / F：动态双 buffer 主循环

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_f_dynamic_double`；时间：2026-10-12T17:05:55.451713+08:00。

**改了什么及结论：** 把双LDS缓冲改成一个运行时slot切换循环，对比显式展开版本，尝试控制代码量与寄存器存活。数值通过，未获得整体净收益。

**源码位置：** [double_buffer_dynamic.hpp](<../double_buffer_dynamic.hpp>)；[gemm.cpp](<../gemm.cpp>)。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4471 / 328.31 | 6.6677 / 328.45 | 2.1882 / 333.62 | 8.8823 / 328.75 | 库对照 |
| rocblas_mm_out | 4.4280 / 329.73 | 6.6609 / 328.79 | 2.1833 / 334.35 | 8.9012 / 328.05 | 库对照 |
| [V001](GEMM_源码与构建索引.md#v001) | 21.6049 / 67.58 | 33.6070 / 65.17 | 8.4792 / 86.09 | 44.6183 / 65.44 | 通过 |
| [V012](GEMM_源码与构建索引.md#v012) | 12.0776 / 120.89 | 16.8737 / 129.79 | 5.7688 / 126.55 | 22.5602 / 129.43 | 通过 |
| [V025](GEMM_源码与构建索引.md#v025) | 17.1488 / 85.14 | 24.7882 / 88.35 | 7.8998 / 92.41 | 33.1359 / 88.12 | 通过 |
| [V026](GEMM_源码与构建索引.md#v026) | 17.9120 / 81.51 | 27.8884 / 78.53 | 9.2559 / 78.87 | 37.1287 / 78.65 | 通过 |
| [V027](GEMM_源码与构建索引.md#v027) | 15.9692 / 91.43 | 23.7818 / 92.09 | 8.5203 / 85.68 | 30.9878 / 94.23 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V001](GEMM_源码与构建索引.md#v001) `base`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V012](GEMM_源码与构建索引.md#v012) `wide4_raw_barrier`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V025](GEMM_源码与构建索引.md#v025) `base_dynamic_double`：本索引首次出现该哈希。
- [V026](GEMM_源码与构建索引.md#v026) `wide_dynamic_double`：本索引首次出现该哈希。
- [V027](GEMM_源码与构建索引.md#v027) `wide4_dynamic_double`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_f_dynamic_double/results.json>)；[environment.json](<../results/stage_f_dynamic_double/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-008"></a>
## 008 / H：CTA 按 token 分组遍历

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_h_tile_order`；时间：2026-10-12T17:12:08.628798+08:00。

**改了什么及结论：** 改变输出tile的遍历次序，让一组CTA沿同一维复用cache中的操作数；只改block到矩阵tile的映射。不同shape收益不同，不能全局用某一个单项最快分组。

**源码位置：** [grouped_partitioner.hpp](<../grouped_partitioner.hpp>)；[gemm.cpp](<../gemm.cpp>)。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4348 / 329.22 | 6.6581 / 328.93 | 2.1901 / 333.33 | 8.9145 / 327.56 | 库对照 |
| rocblas_mm_out | 4.4243 / 330.00 | 6.6559 / 329.04 | 2.1845 / 334.18 | 8.8793 / 328.86 | 库对照 |
| [V001](GEMM_源码与构建索引.md#v001) | 21.5868 / 67.63 | 33.5843 / 65.21 | 8.5060 / 85.82 | 44.6098 / 65.46 | 通过 |
| [V019](GEMM_源码与构建索引.md#v019) | 10.5417 / 138.50 | 16.1434 / 135.66 | 5.6422 / 129.38 | 21.5867 / 135.27 | 通过 |
| [V028](GEMM_源码与构建索引.md#v028) | 13.7144 / 106.46 | 17.1716 / 127.54 | 5.5369 / 131.85 | 24.1829 / 120.75 | 通过 |
| [V029](GEMM_源码与构建索引.md#v029) | 10.7370 / 135.98 | 16.0615 / 136.35 | 5.3541 / 136.35 | 21.5209 / 135.68 | 通过 |
| [V030](GEMM_源码与构建索引.md#v030) | 10.2838 / 141.97 | 15.1671 / 144.39 | 5.3861 / 135.54 | 20.1633 / 144.82 | 通过 |
| [V031](GEMM_源码与构建索引.md#v031) | 10.1265 / 144.18 | 15.2099 / 143.99 | 5.3674 / 136.01 | 20.2054 / 144.52 | 通过 |
| [V032](GEMM_源码与构建索引.md#v032) | 10.3095 / 141.62 | 15.3097 / 143.05 | 5.3424 / 136.65 | 20.4305 / 142.93 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V001](GEMM_源码与构建索引.md#v001) `base`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V019](GEMM_源码与构建索引.md#v019) `wide_pad16`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V028](GEMM_源码与构建索引.md#v028) `wide_pad16_g1`：本索引首次出现该哈希。
- [V029](GEMM_源码与构建索引.md#v029) `wide_pad16_g2`：本索引首次出现该哈希。
- [V030](GEMM_源码与构建索引.md#v030) `wide_pad16_g4`：本索引首次出现该哈希。
- [V031](GEMM_源码与构建索引.md#v031) `wide_pad16_g8`：本索引首次出现该哈希。
- [V032](GEMM_源码与构建索引.md#v032) `wide_pad16_g16`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_h_tile_order/results.json>)；[environment.json](<../results/stage_h_tile_order/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-009"></a>
## 009 / I：K-pack XOR 布局

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_i_pack_xor`；时间：2026-10-12T17:19:42.386825+08:00。

**改了什么及结论：** 对LDS内K-pack地址使用轻量XOR映射，尝试分散访问，同时保持逻辑数据位置可逆。正确性通过，未成为总体最佳，个别out_proj有局部改善。

**源码位置：** [xor_policy.hpp](<../xor_policy.hpp>)；[gemm.cpp](<../gemm.cpp>)。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4599 / 327.36 | 6.6568 / 328.99 | 2.1923 / 332.99 | 8.9161 / 327.50 | 库对照 |
| rocblas_mm_out | 4.4533 / 327.85 | 6.6569 / 328.98 | 2.1866 / 333.85 | 8.8818 / 328.77 | 库对照 |
| [V001](GEMM_源码与构建索引.md#v001) | 21.5234 / 67.83 | 33.8553 / 64.69 | 8.4600 / 86.29 | 45.1524 / 64.67 | 通过 |
| [V031](GEMM_源码与构建索引.md#v031) | 10.1170 / 144.31 | 15.1685 / 144.38 | 5.3677 / 136.00 | 20.2919 / 143.90 | 通过 |
| [V033](GEMM_源码与构建索引.md#v033) | 22.4329 / 65.08 | 30.1031 / 72.75 | 8.3805 / 87.11 | 38.5230 / 75.80 | 通过 |
| [V034](GEMM_源码与构建索引.md#v034) | 11.2538 / 129.74 | 15.8550 / 138.13 | 5.3941 / 135.33 | 21.1374 / 138.15 | 通过 |
| [V035](GEMM_源码与构建索引.md#v035) | 11.6115 / 125.74 | 16.0649 / 136.32 | 5.4111 / 134.91 | 21.4211 / 136.32 | 通过 |
| [V036](GEMM_源码与构建索引.md#v036) | 12.5023 / 116.78 | 15.3306 / 142.85 | 5.0143 / 145.59 | 21.5086 / 135.76 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V001](GEMM_源码与构建索引.md#v001) `base`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V031](GEMM_源码与构建索引.md#v031) `wide_pad16_g8`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V033](GEMM_源码与构建索引.md#v033) `base_xor`：本索引首次出现该哈希。
- [V034](GEMM_源码与构建索引.md#v034) `wide_xor`：本索引首次出现该哈希。
- [V035](GEMM_源码与构建索引.md#v035) `wide4_xor`：本索引首次出现该哈希。
- [V036](GEMM_源码与构建索引.md#v036) `wide_xor_g8`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_i_pack_xor/results.json>)；[environment.json](<../results/stage_i_pack_xor/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-010"></a>
## 010 / J：launch bounds 约束

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_j_launch_bounds`；时间：2026-10-12T17:28:40.812495+08:00。

**改了什么及结论：** 比较默认与放宽的launch bounds，观察编译器寄存器分配和调度的变化；未改硬件资源或驱动。必须结合实际ELF和同轮数据判断，不能从声明推断真实occupancy。

**源码位置：** [gemm.cpp](<../gemm.cpp>)；[build.py](<../build.py>)。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4796 / 325.92 | 6.6640 / 328.64 | 2.1995 / 331.89 | 8.9072 / 327.83 | 库对照 |
| rocblas_mm_out | 4.4505 / 328.06 | 6.6438 / 329.64 | 2.1872 / 333.76 | 8.9033 / 327.97 | 库对照 |
| [V001](GEMM_源码与构建索引.md#v001) | 21.6502 / 67.44 | 33.6237 / 65.13 | 8.4826 / 86.06 | 45.1960 / 64.61 | 通过 |
| [V031](GEMM_源码与构建索引.md#v031) | 10.1136 / 144.36 | 15.1702 / 144.36 | 5.3630 / 136.12 | 20.2889 / 143.92 | 通过 |
| [V037](GEMM_源码与构建索引.md#v037) | 243.5615 / 5.99 | 366.0385 / 5.98 | 121.4571 / 6.01 | 488.7101 / 5.97 | 通过 |
| [V038](GEMM_源码与构建索引.md#v038) | 248.7767 / 5.87 | 373.1675 / 5.87 | 123.9596 / 5.89 | 497.7689 / 5.87 | 通过 |
| [V039](GEMM_源码与构建索引.md#v039) | 237.8809 / 6.14 | 356.6224 / 6.14 | 118.3156 / 6.17 | 476.0944 / 6.13 | 通过 |
| [V019](GEMM_源码与构建索引.md#v019) | 10.5812 / 137.98 | 16.1426 / 135.67 | 5.6496 / 129.21 | 21.5671 / 135.39 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V001](GEMM_源码与构建索引.md#v001) `base`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V031](GEMM_源码与构建索引.md#v031) `wide_pad16_g8`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V037](GEMM_源码与构建索引.md#v037) `large_relaxed`：本索引首次出现该哈希。
- [V038](GEMM_源码与构建索引.md#v038) `large_pad16_relaxed`：本索引首次出现该哈希。
- [V039](GEMM_源码与构建索引.md#v039) `large_pad16_g8_relaxed`：本索引首次出现该哈希。
- [V019](GEMM_源码与构建索引.md#v019) `wide_pad16_relaxed`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

原始证据：[原始 results.json](<../results/stage_j_launch_bounds/results.json>)；[environment.json](<../results/stage_j_launch_bounds/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-011"></a>
## 011 / K：N 方向 lane/interleave

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_k_n_interleave`；时间：2026-10-12T17:37:11.930443+08:00。

**改了什么及结论：** 改变每wave在N方向的片段交错和BF16 lane映射，测试同一数学tile下的访问与寄存器布局。用NI4等私有配置选择，不是修改共享CK的默认策略。

**源码位置：** [warp_variants.hpp](<../warp_variants.hpp>)；[policy.hpp](<../policy.hpp>)；[gemm.cpp](<../gemm.cpp>)。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4651 / 326.99 | 6.6703 / 328.33 | 2.1931 / 332.87 | 8.9073 / 327.83 | 库对照 |
| rocblas_mm_out | 4.4552 / 327.71 | 6.6428 / 329.68 | 2.1852 / 334.08 | 8.8801 / 328.83 | 库对照 |
| [V001](GEMM_源码与构建索引.md#v001) | 21.5203 / 67.84 | 33.7538 / 64.88 | 8.4822 / 86.06 | 45.3463 / 64.39 | 通过 |
| [V031](GEMM_源码与构建索引.md#v031) | 10.1319 / 144.10 | 15.1732 / 144.34 | 5.3661 / 136.04 | 20.3217 / 143.69 | 通过 |
| [V040](GEMM_源码与构建索引.md#v040) | 22.2373 / 65.66 | 31.2750 / 70.02 | 8.5234 / 85.65 | 41.0153 / 71.19 | 通过 |
| [V041](GEMM_源码与构建索引.md#v041) | 16.4407 / 88.81 | 24.4346 / 89.63 | 8.1417 / 89.66 | 32.5530 / 89.70 | 通过 |
| [V042](GEMM_源码与构建索引.md#v042) | 18.0391 / 80.94 | 26.8893 / 81.45 | 9.0775 / 80.42 | 35.8250 / 81.51 | 通过 |
| [V043](GEMM_源码与构建索引.md#v043) | 16.3296 / 89.41 | 24.3654 / 89.88 | 8.1824 / 89.22 | 32.4511 / 89.98 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V001](GEMM_源码与构建索引.md#v001) `base`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V031](GEMM_源码与构建索引.md#v031) `wide_pad16_g8`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V040](GEMM_源码与构建索引.md#v040) `base_ni4`：本索引首次出现该哈希。
- [V041](GEMM_源码与构建索引.md#v041) `wide_ni4`：本索引首次出现该哈希。
- [V042](GEMM_源码与构建索引.md#v042) `wide_pad16_ni4_g8`：本索引首次出现该哈希。
- [V043](GEMM_源码与构建索引.md#v043) `wide4_ni4`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_k_n_interleave/results.json>)；[environment.json](<../results/stage_k_n_interleave/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-012"></a>
## 012 / L：平衡 wave 分区与操作数复用

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_l_balanced_operands`；时间：2026-10-12T17:41:18.724036+08:00。

**改了什么及结论：** 测试2×4等wave划分及每wave M方向工作量，配合padding与CTA分组，改善A/B复用的平衡。取得约140–150T阶段性结果；不同于后期rolephase的1×8分工。

**源码位置：** [warp_variants.hpp](<../warp_variants.hpp>)；[policy.hpp](<../policy.hpp>)；[gemm.cpp](<../gemm.cpp>)。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4628 / 327.15 | 6.6633 / 328.67 | 2.2051 / 331.06 | 8.9127 / 327.63 | 库对照 |
| rocblas_mm_out | 4.4479 / 328.25 | 6.6698 / 328.35 | 2.1859 / 333.96 | 8.9037 / 327.96 | 库对照 |
| [V001](GEMM_源码与构建索引.md#v001) | 21.5679 / 67.69 | 33.9307 / 64.54 | 8.4511 / 86.38 | 45.1644 / 64.65 | 通过 |
| [V031](GEMM_源码与构建索引.md#v031) | 10.1213 / 144.25 | 15.2589 / 143.52 | 5.3648 / 136.08 | 20.2575 / 144.15 | 通过 |
| [V044](GEMM_源码与构建索引.md#v044) | 11.4553 / 127.45 | 16.0769 / 136.22 | 5.4197 / 134.70 | 21.4334 / 136.24 | 通过 |
| [V045](GEMM_源码与构建索引.md#v045) | 9.7622 / 149.56 | 14.5817 / 150.19 | 5.2341 / 139.47 | 19.5208 / 149.59 | 通过 |
| [V046](GEMM_源码与构建索引.md#v046) | 17.8190 / 81.94 | 27.0238 / 81.04 | 9.3408 / 78.15 | 36.0013 / 81.11 | 通过 |
| [V047](GEMM_源码与构建索引.md#v047) | 11.9054 / 122.64 | 16.4437 / 133.18 | 5.3452 / 136.57 | 22.3301 / 130.77 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V001](GEMM_源码与构建索引.md#v001) `base`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V031](GEMM_源码与构建索引.md#v031) `wide_pad16_g8`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V044](GEMM_源码与构建索引.md#v044) `wide_mr4`：本索引首次出现该哈希。
- [V045](GEMM_源码与构建索引.md#v045) `wide_mr4_pad16_g8`：本索引首次出现该哈希。
- [V046](GEMM_源码与构建索引.md#v046) `wide_mr4_wk16_pad16_g8`：本索引首次出现该哈希。
- [V047](GEMM_源码与构建索引.md#v047) `wide_mr4_pad8_g8`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_l_balanced_operands/results.json>)；[environment.json](<../results/stage_l_balanced_operands/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-013"></a>
## 013 / M：LDS K-pack 主序

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_m_kpack_major`；时间：2026-10-12T17:50:55.870939+08:00。

**改了什么及结论：** 比较Kpack4/8的LDS描述符和访问顺序，检查连续搬运与MMAC片段之间的匹配。保留原始结果，不把更宽Kpack预设为更高吞吐。

**源码位置：** [kpack_policy.hpp](<../kpack_policy.hpp>)；[gemm.cpp](<../gemm.cpp>)。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4662 / 326.91 | 6.6651 / 328.58 | 2.1912 / 333.16 | 8.8763 / 328.97 | 库对照 |
| rocblas_mm_out | 4.4631 / 327.13 | 6.6447 / 329.59 | 2.1850 / 334.10 | 8.8864 / 328.60 | 库对照 |
| [V001](GEMM_源码与构建索引.md#v001) | 21.5529 / 67.74 | 33.8817 / 64.64 | 8.4673 / 86.22 | 45.1489 / 64.68 | 通过 |
| [V031](GEMM_源码与构建索引.md#v031) | 10.1336 / 144.08 | 15.2484 / 143.62 | 5.3657 / 136.05 | 20.3233 / 143.68 | 通过 |
| [V048](GEMM_源码与构建索引.md#v048) | 12.1201 / 120.46 | 15.0753 / 145.27 | 5.2355 / 139.43 | 19.9088 / 146.67 | 通过 |
| [V049](GEMM_源码与构建索引.md#v049) | 12.7178 / 114.80 | 14.9436 / 146.55 | 5.0741 / 143.87 | 19.2677 / 151.55 | 通过 |
| [V050](GEMM_源码与构建索引.md#v050) | 11.8144 / 123.58 | 16.5090 / 132.66 | 5.4280 / 134.49 | 22.1398 / 131.89 | 通过 |
| [V051](GEMM_源码与构建索引.md#v051) | 12.3206 / 118.50 | 16.1871 / 135.30 | 5.1689 / 141.23 | 22.0123 / 132.65 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V001](GEMM_源码与构建索引.md#v001) `base`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V031](GEMM_源码与构建索引.md#v031) `wide_pad16_g8`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V048](GEMM_源码与构建索引.md#v048) `wide_kpack4_g8`：本索引首次出现该哈希。
- [V049](GEMM_源码与构建索引.md#v049) `wide_kpack8_g8`：本索引首次出现该哈希。
- [V050](GEMM_源码与构建索引.md#v050) `wide4_kpack4_g8`：本索引首次出现该哈希。
- [V051](GEMM_源码与构建索引.md#v051) `wide4_kpack8_g8`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_m_kpack_major/results.json>)；[environment.json](<../results/stage_m_kpack_major/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-014"></a>
## 014 / N：8 compute + 4 producer wave 原型

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_n_wave_specialized`；时间：2026-10-12T18:24:40.777438+08:00。

**改了什么及结论：** 独立写CK MMAC分工原型，8个计算wave、4个加载wave。默认768-thread编译限制导致仅84VGPR及大量spill，FC2约4.6T；正确但严重回退。

**源码位置：** [specialized.cpp](<../specialized.cpp>)。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4506 / 328.05 | 6.6575 / 328.96 | 2.2087 / 330.52 | 8.9118 / 327.66 | 库对照 |
| rocblas_mm_out | 4.4343 / 329.26 | 6.6458 / 329.54 | 2.1862 / 333.92 | 8.8803 / 328.82 | 库对照 |
| [V001](GEMM_源码与构建索引.md#v001) | 21.6077 / 67.57 | 33.8616 / 64.68 | 8.5017 / 85.87 | 44.5880 / 65.49 | 通过 |
| [V045](GEMM_源码与构建索引.md#v045) | 9.7627 / 149.55 | 14.5471 / 150.55 | 5.2414 / 139.28 | 19.4250 / 150.32 | 通过 |
| [V052](GEMM_源码与构建索引.md#v052) | 315.2783 / 4.63 | 475.4893 / 4.61 | 159.1985 / 4.59 | 634.6400 / 4.60 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V001](GEMM_源码与构建索引.md#v001) `base`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V045](GEMM_源码与构建索引.md#v045) `wide_mr4_pad16_g8`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V052](GEMM_源码与构建索引.md#v052) `specialized`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_n_wave_specialized/results.json>)；[environment.json](<../results/stage_n_wave_specialized/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-015"></a>
## 015 / O：解除候选的寄存器池编译限制

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_o_register_pool`；时间：2026-10-12T18:32:18.149000+08:00。

**改了什么及结论：** 使用本机DTK支持的--enable-num-vgprs-768=true重新编译候选，允许大tile所需VGPR。large_pad16_g8约202VGPR、无spill，FC2约188T；分工原型消除spill后约132T仍落后。

**源码位置：** [compiler_options.py](<../compiler_options.py>)；[build.py](<../build.py>)；[gemm.cpp](<../gemm.cpp>)；[specialized.cpp](<../specialized.cpp>)。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4781 / 326.04 | 6.6532 / 329.17 | 2.1890 / 333.49 | 8.9197 / 327.37 | 库对照 |
| rocblas_mm_out | 4.4644 / 327.04 | 6.6608 / 328.80 | 2.1857 / 333.99 | 8.8734 / 329.08 | 库对照 |
| [V045](GEMM_源码与构建索引.md#v045) | 9.7354 / 149.97 | 14.5186 / 150.84 | 5.2396 / 139.33 | 19.3719 / 150.74 | 通过 |
| [V053](GEMM_源码与构建索引.md#v053) | 11.0105 / 132.60 | 16.6020 / 131.91 | 5.9966 / 121.74 | 22.1455 / 131.86 | 通过 |
| [V054](GEMM_源码与构建索引.md#v054) | 7.7728 / 187.84 | 11.8714 / 184.48 | 4.4119 / 165.46 | 15.8417 / 184.33 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V045](GEMM_源码与构建索引.md#v045) `wide_mr4_pad16_g8`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V053](GEMM_源码与构建索引.md#v053) `specialized_reg768`：本索引首次出现该哈希。
- [V045](GEMM_源码与构建索引.md#v045) `wide_mr4_pad16_g8_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V054](GEMM_源码与构建索引.md#v054) `large_pad16_g8_reg768`：本索引首次出现该哈希。

本轮同SHA去重：`wide_mr4_pad16_g8_reg768` → `wide_mr4_pad16_g8`。

原始证据：[原始 results.json](<../results/stage_o_register_pool/results.json>)；[environment.json](<../results/stage_o_register_pool/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-016"></a>
## 016 / P：大 tile 配套布局/流水

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_p_large_pipeline`；时间：2026-10-12T18:37:51.764701+08:00。

**改了什么及结论：** 在寄存器池已确认可用后，重新测试大tile的padding、双buffer等组合；把资源限制与算法流水问题分开。结果以逐候选四shape表为准，未把编译通过等同为收益。

**源码位置：** [gemm.cpp](<../gemm.cpp>)；[padded_policy.hpp](<../padded_policy.hpp>)；[double_buffer.hpp](<../double_buffer.hpp>)。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4691 / 326.69 | 6.6727 / 328.21 | 2.1937 / 332.77 | 8.9163 / 327.50 | 库对照 |
| rocblas_mm_out | 4.4527 / 327.90 | 6.6575 / 328.96 | 2.1825 / 334.48 | 8.9001 / 328.09 | 库对照 |
| [V054](GEMM_源码与构建索引.md#v054) | 7.7732 / 187.83 | 11.8729 / 184.46 | 4.4130 / 165.42 | 15.8460 / 184.28 | 通过 |
| [V055](GEMM_源码与构建索引.md#v055) | 7.8150 / 186.82 | 11.9013 / 184.02 | 4.4333 / 164.66 | 15.8897 / 183.77 | 通过 |
| [V056](GEMM_源码与构建索引.md#v056) | 8.7954 / 166.00 | 13.2813 / 164.90 | 4.8996 / 148.99 | 17.7008 / 164.97 | 通过 |
| [V057](GEMM_源码与构建索引.md#v057) | 8.2503 / 176.96 | 12.5862 / 174.00 | 4.6590 / 156.69 | 16.7858 / 173.96 | 通过 |
| [V058](GEMM_源码与构建索引.md#v058) | 9.8118 / 148.80 | 14.7785 / 148.19 | 5.3937 / 135.34 | 19.7082 / 148.16 | 通过 |
| [V059](GEMM_源码与构建索引.md#v059) | 24.4281 / 59.77 | 36.9501 / 59.27 | 12.5350 / 58.24 | 49.5115 / 58.98 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V054](GEMM_源码与构建索引.md#v054) `large_pad16_g8_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V055](GEMM_源码与构建索引.md#v055) `large_mr4_pad16_g8_reg768`：本索引首次出现该哈希。
- [V056](GEMM_源码与构建索引.md#v056) `large_mr4_xor_g8_reg768`：本索引首次出现该哈希。
- [V057](GEMM_源码与构建索引.md#v057) `large_kpack8_g8_reg768`：本索引首次出现该哈希。
- [V058](GEMM_源码与构建索引.md#v058) `large_raw_barrier_g8_reg768`：本索引首次出现该哈希。
- [V059](GEMM_源码与构建索引.md#v059) `large_double_g8_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_p_large_pipeline/results.json>)；[environment.json](<../results/stage_p_large_pipeline/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-017"></a>
## 017 / Q：分工原型的 padding / Kpack / direct-to-LDS

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_q_specialized_layout`；时间：2026-10-12T18:39:10.932450+08:00。

**改了什么及结论：** 给specialized原型配置padding16、Kpack布局及HCU异步direct-to-LDS搬运。尝试减少寄存器中转；这些配置没有全面超过同期通用大tile。

**源码位置：** [specialized.cpp](<../specialized.cpp>)。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4451 / 328.46 | 6.6499 / 329.33 | 2.1886 / 333.54 | 8.8853 / 328.64 | 库对照 |
| rocblas_mm_out | 4.4332 / 329.34 | 6.6510 / 329.28 | 2.1855 / 334.02 | 8.8927 / 328.36 | 库对照 |
| [V054](GEMM_源码与构建索引.md#v054) | 7.7661 / 188.00 | 11.8727 / 184.46 | 4.4158 / 165.32 | 15.8390 / 184.36 | 通过 |
| [V053](GEMM_源码与构建索引.md#v053) | 11.0046 / 132.67 | 16.6196 / 131.77 | 5.9978 / 121.71 | 22.2007 / 131.53 | 通过 |
| [V060](GEMM_源码与构建索引.md#v060) | 10.4498 / 139.72 | 16.0746 / 136.24 | 5.7060 / 127.94 | 21.4401 / 136.20 | 通过 |
| [V061](GEMM_源码与构建索引.md#v061) | 10.4577 / 139.61 | 15.9409 / 137.38 | 5.7022 / 128.02 | 21.3031 / 137.07 | 通过 |
| [V062](GEMM_源码与构建索引.md#v062) | 12.1430 / 120.24 | 18.0411 / 121.39 | 6.2229 / 117.31 | 23.7815 / 122.79 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V054](GEMM_源码与构建索引.md#v054) `large_pad16_g8_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V053](GEMM_源码与构建索引.md#v053) `specialized_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V060](GEMM_源码与构建索引.md#v060) `specialized_pad16_reg768`：本索引首次出现该哈希。
- [V061](GEMM_源码与构建索引.md#v061) `specialized_kpack8_reg768`：本索引首次出现该哈希。
- [V062](GEMM_源码与构建索引.md#v062) `specialized_async_kpack8_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_q_specialized_layout/results.json>)；[environment.json](<../results/stage_q_specialized_layout/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-018"></a>
## 018 / R：B buffer load 与 wave priority

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_r_buffer_priority`；时间：2026-10-12T18:43:36.647311+08:00。

**改了什么及结论：** 在原型中比较B的buffer-load、计算wave s_setprio以及组合版本；检查搬运和数学竞争。单个指令替换不等于复刻rocBLAS的完整流水。

**源码位置：** [specialized.cpp](<../specialized.cpp>)。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4559 / 327.66 | 6.6699 / 328.34 | 2.1880 / 333.64 | 8.8778 / 328.91 | 库对照 |
| rocblas_mm_out | 4.4536 / 327.83 | 6.6466 / 329.50 | 2.1844 / 334.20 | 8.8745 / 329.04 | 库对照 |
| [V054](GEMM_源码与构建索引.md#v054) | 7.7721 / 187.86 | 11.8689 / 184.52 | 4.4120 / 165.46 | 15.8446 / 184.29 | 通过 |
| [V061](GEMM_源码与构建索引.md#v061) | 10.4491 / 139.73 | 15.9358 / 137.43 | 5.7001 / 128.07 | 21.2455 / 137.44 | 通过 |
| [V063](GEMM_源码与构建索引.md#v063) | 10.3466 / 141.11 | 15.2616 / 143.50 | 5.5608 / 131.28 | 20.3718 / 143.34 | 通过 |
| [V064](GEMM_源码与构建索引.md#v064) | 10.3911 / 140.51 | 15.1037 / 145.00 | 5.4634 / 133.62 | 20.1091 / 145.21 | 通过 |
| [V065](GEMM_源码与构建索引.md#v065) | 10.3957 / 140.44 | 15.1223 / 144.82 | 5.4886 / 133.01 | 20.1148 / 145.17 | 通过 |
| [V066](GEMM_源码与构建索引.md#v066) | 12.0247 / 121.42 | 17.9211 / 122.20 | 6.0824 / 120.02 | 23.7995 / 122.69 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V054](GEMM_源码与构建索引.md#v054) `large_pad16_g8_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V061](GEMM_源码与构建索引.md#v061) `specialized_kpack8_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V063](GEMM_源码与构建索引.md#v063) `specialized_kpack8_prio_reg768`：本索引首次出现该哈希。
- [V064](GEMM_源码与构建索引.md#v064) `specialized_kpack8_buffer_reg768`：本索引首次出现该哈希。
- [V065](GEMM_源码与构建索引.md#v065) `specialized_kpack8_buffer_prio_reg768`：本索引首次出现该哈希。
- [V066](GEMM_源码与构建索引.md#v066) `specialized_async_kpack8_buffer_prio_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_r_buffer_priority/results.json>)；[environment.json](<../results/stage_r_buffer_priority/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-019"></a>
## 019 / S：tile 长宽比、BK64 与不支持尺寸

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_s_aspect_k64`；时间：2026-10-12T18:49:39.336543+08:00。

**改了什么及结论：** 测试更宽/更高tile、BK64及padding8。最初BN512不整除N5376导致InvalidValue，后续bench显式记录unsupported_alignment并跳过，不把不支持的shape记为零耗时。

**源码位置：** [gemm.cpp](<../gemm.cpp>)；[policy.hpp](<../policy.hpp>)；[bench.py](<../bench.py>)；[STAGE_S_INITIAL_FAILURE.md](<../STAGE_S_INITIAL_FAILURE.md>)。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4820 / 325.75 | 6.6270 / 330.47 | 2.1942 / 332.70 | 8.9093 / 327.75 | 库对照 |
| rocblas_mm_out | 4.4613 / 327.26 | 6.6225 / 330.69 | 2.1864 / 333.89 | 8.9182 / 327.43 | 库对照 |
| [V054](GEMM_源码与构建索引.md#v054) | 7.7730 / 187.83 | 11.8682 / 184.53 | 4.4149 / 165.35 | 15.8365 / 184.39 | 通过 |
| [V067](GEMM_源码与构建索引.md#v067) | 9.8629 / 148.03 | 14.8834 / 147.15 | 5.4391 / 134.22 | 19.8422 / 147.16 | 通过 |
| [V068](GEMM_源码与构建索引.md#v068) | 11.2405 / 129.89 | 16.9495 / 129.21 | 6.1475 / 118.75 | 22.5995 / 129.21 | 通过 |
| [V069](GEMM_源码与构建索引.md#v069) | 9.6357 / 151.52 | 14.6635 / 149.35 | 5.2732 / 138.44 | 19.6204 / 148.83 | 通过 |
| [V070](GEMM_源码与构建索引.md#v070) | 未计时 | 14.0930 / 155.40 | 未计时 | 18.7823 / 155.47 | 不支持尺寸/通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V054](GEMM_源码与构建索引.md#v054) `large_pad16_g8_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V067](GEMM_源码与构建索引.md#v067) `large_pad8_g8_reg768`：本索引首次出现该哈希。
- [V068](GEMM_源码与构建索引.md#v068) `large_mr4_k64_kpack8_g8_reg768`：本索引首次出现该哈希。
- [V069](GEMM_源码与构建索引.md#v069) `tall512_mr4_pad16_g8_reg768`：本索引首次出现该哈希。
- [V070](GEMM_源码与构建索引.md#v070) `wide512_mr4_pad16_g8_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_s_aspect_k64/results.json>)；[environment.json](<../results/stage_s_aspect_k64/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-020"></a>
## 020 / U：12 数学 wave 与真实 M 尾行

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_u_12_math_waves`；时间：2026-10-12T19:03:58.595564+08:00。

**改了什么及结论：** 尝试384×256的12数学wave候选，发现global B分区256/(12×16)截断而漏覆盖，候选未计时，并增加完整覆盖static_assert。另一有效256×256配置开启原生padding，单独验证M18880及输出guard。

**源码位置：** [policy.hpp](<../policy.hpp>)；[gemm.cpp](<../gemm.cpp>)；[STAGE_U_CORRECTNESS.md](<../STAGE_U_CORRECTNESS.md>)。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4465 / 328.36 | 6.6434 / 329.66 | 2.1848 / 334.14 | 8.8801 / 328.83 | 库对照 |
| rocblas_mm_out | 4.4399 / 328.84 | 6.6515 / 329.26 | 2.1816 / 334.62 | 8.8741 / 329.05 | 库对照 |
| [V054](GEMM_源码与构建索引.md#v054) | 7.7729 / 187.84 | 11.8688 / 184.52 | 4.4148 / 165.35 | 15.8474 / 184.26 | 通过 |
| [V071](GEMM_源码与构建索引.md#v071) | 7.7740 / 187.81 | 11.8672 / 184.54 | 4.4122 / 165.45 | 15.8414 / 184.33 | 通过 |
| [V072](GEMM_源码与构建索引.md#v072) | 未计时 | 未计时 | 未计时 | 未计时 | 失败 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V054](GEMM_源码与构建索引.md#v054) `large_pad16_g8_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V071](GEMM_源码与构建索引.md#v071) `large_pad16_g8_pad_m_reg768`：本索引首次出现该哈希。
- [V072](GEMM_源码与构建索引.md#v072) `taller384_pad16_g8_pad_m_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_u_12_math_waves/results.json>)；[environment.json](<../results/stage_u_12_math_waves/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-021"></a>
## 021 / U：12 数学 wave 与真实 M 尾行

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_u_native_m18880`；时间：2026-10-12T19:04:28.652565+08:00。

**改了什么及结论：** 尝试384×256的12数学wave候选，发现global B分区256/(12×16)截断而漏覆盖，候选未计时，并增加完整覆盖static_assert。另一有效256×256配置开启原生padding，单独验证M18880及输出guard。

**源码位置：** [policy.hpp](<../policy.hpp>)；[gemm.cpp](<../gemm.cpp>)；[STAGE_U_CORRECTNESS.md](<../STAGE_U_CORRECTNESS.md>)。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4386 / 327.82 | 6.6558 / 327.93 | 2.1888 / 332.40 | 8.8870 / 327.47 | 库对照 |
| rocblas_mm_out | 4.4550 / 326.62 | 6.6410 / 328.66 | 2.1881 / 332.50 | 8.8890 / 327.39 | 库对照 |
| [V071](GEMM_源码与构建索引.md#v071) | 7.7648 / 187.40 | 11.8624 / 184.00 | 4.4137 / 164.84 | 15.8325 / 183.81 | 通过 |
| [V072](GEMM_源码与构建索引.md#v072) | 未计时 | 未计时 | 未计时 | 未计时 | 失败 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V071](GEMM_源码与构建索引.md#v071) `large_pad16_g8_pad_m_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V072](GEMM_源码与构建索引.md#v072) `taller384_pad16_g8_pad_m_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

原始证据：[原始 results.json](<../results/stage_u_native_m18880/results.json>)；[environment.json](<../results/stage_u_native_m18880/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-022"></a>
## 022 / V：m0 wrap 与 K64 分工流水

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_v_k64_wrapped_pipeline`；时间：2026-10-12T19:16:43.883057+08:00。

**改了什么及结论：** 通过16384元素硬件探针确认m0 wrap的真实LDS映射，再实现256×256、8计算+4加载wave、K64分两K32相位。wrap单/双缓冲约150/162T，优于不wrap但仍落后通用188T。

**源码位置：** [role64.cpp](<../role64.cpp>)；[lds_wrap_probe.cpp](<../lds_wrap_probe.cpp>)；[verify_wrap_formula.py](<../verify_wrap_formula.py>)。

历史算法/同步/正确性细节：[原记录§1](<../LDS_WRAP_AND_PIPELINE.md:7>)；[原记录§2](<../LDS_WRAP_AND_PIPELINE.md:23>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4715 / 326.52 | 6.6691 / 328.39 | 2.1909 / 333.20 | 8.9326 / 326.90 | 库对照 |
| rocblas_mm_out | 4.4671 / 326.84 | 6.6691 / 328.38 | 2.1844 / 334.19 | 8.9039 / 327.95 | 库对照 |
| [V071](GEMM_源码与构建索引.md#v071) | 7.7707 / 187.89 | 11.8744 / 184.43 | 4.4158 / 165.32 | 15.8281 / 184.48 | 通过 |
| [V073](GEMM_源码与构建索引.md#v073) | 17.6338 / 82.80 | 26.2142 / 83.54 | 9.2753 / 78.70 | 34.9965 / 83.44 | 通过 |
| [V074](GEMM_源码与构建索引.md#v074) | 9.7587 / 149.61 | 14.7339 / 148.64 | 5.3620 / 136.14 | 19.6780 / 148.39 | 通过 |
| [V075](GEMM_源码与构建索引.md#v075) | 9.0078 / 162.08 | 13.5773 / 161.30 | 4.9541 / 147.36 | 18.1003 / 161.33 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V071](GEMM_源码与构建索引.md#v071) `large_pad16_g8_pad_m_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V073](GEMM_源码与构建索引.md#v073) `role64_single_plain_reg768`：本索引首次出现该哈希。
- [V074](GEMM_源码与构建索引.md#v074) `role64_single_wrap_reg768`：本索引首次出现该哈希。
- [V075](GEMM_源码与构建索引.md#v075) `role64_double_wrap_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_v_k64_wrapped_pipeline/results.json>)；[environment.json](<../results/stage_v_k64_wrapped_pipeline/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-023"></a>
## 023 / W：MMAC 发射与累加器初始化

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_w_mmac_order`；时间：2026-10-12T19:25:27.085037+08:00。

**改了什么及结论：** inline asm MMAC与原版同SHA，不计新收益；literal-zero操作数不被汇编器支持。改首步初始化的有效候选约135T，寄存器安排变化后回退。

**源码位置：** [role64.cpp](<../role64.cpp>)。

历史算法/同步/正确性细节：[原记录§3](<../LDS_WRAP_AND_PIPELINE.md:37>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4219 / 330.18 | 6.6410 / 329.77 | 2.1875 / 333.73 | 8.8914 / 328.41 | 库对照 |
| rocblas_mm_out | 4.4228 / 330.12 | 6.6262 / 330.51 | 2.1858 / 333.98 | 8.8975 / 328.19 | 库对照 |
| [V071](GEMM_源码与构建索引.md#v071) | 7.7743 / 187.80 | 11.8656 / 184.57 | 4.4125 / 165.44 | 15.8315 / 184.44 | 通过 |
| [V075](GEMM_源码与构建索引.md#v075) | 9.0077 / 162.09 | 13.5762 / 161.31 | 4.9496 / 147.49 | 18.0962 / 161.36 | 通过 |
| [V076](GEMM_源码与构建索引.md#v076) | 10.8029 / 135.15 | 16.2140 / 135.07 | 5.8689 / 124.39 | 21.6023 / 135.17 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V071](GEMM_源码与构建索引.md#v071) `large_pad16_g8_pad_m_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V075](GEMM_源码与构建索引.md#v075) `role64_double_wrap_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V076](GEMM_源码与构建索引.md#v076) `role64_double_wrap_zero_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_w_mmac_order/results.json>)；[environment.json](<../results/stage_w_mmac_order/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-024"></a>
## 024 / X：8 数学 wave 协作搬运

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_x_cooperative_prefetch`；时间：2026-10-12T19:32:25.863928+08:00。

**改了什么及结论：** 取消4个专门producer，由8个compute wave协作direct-to-LDS；保留真实搬运和同步，比较wrap及单/双buffer。约153–155T，没有超过分工原型。

**源码位置：** [role64.cpp](<../role64.cpp>)。

历史算法/同步/正确性细节：[原记录§5](<../LDS_WRAP_AND_PIPELINE.md:56>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4374 / 329.03 | 6.6589 / 328.89 | 2.1904 / 333.27 | 8.9180 / 327.43 | 库对照 |
| rocblas_mm_out | 4.4332 / 329.34 | 6.6410 / 329.78 | 2.1840 / 334.26 | 8.8795 / 328.85 | 库对照 |
| [V071](GEMM_源码与构建索引.md#v071) | 7.7756 / 187.77 | 11.8726 / 184.46 | 4.4149 / 165.35 | 15.8415 / 184.33 | 通过 |
| [V075](GEMM_源码与构建索引.md#v075) | 9.0071 / 162.10 | 13.5770 / 161.30 | 4.9503 / 147.47 | 18.0931 / 161.39 | 通过 |
| [V077](GEMM_源码与构建索引.md#v077) | 9.5536 / 152.82 | 14.5232 / 150.80 | 5.2390 / 139.34 | 19.3925 / 150.58 | 通过 |
| [V078](GEMM_源码与构建索引.md#v078) | 9.3993 / 155.33 | 13.9375 / 157.13 | 5.0498 / 144.56 | 18.5954 / 157.03 | 通过 |
| [V079](GEMM_源码与构建索引.md#v079) | 17.1587 / 85.09 | 25.5411 / 85.75 | 9.0554 / 80.62 | 34.0593 / 85.73 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V071](GEMM_源码与构建索引.md#v071) `large_pad16_g8_pad_m_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V075](GEMM_源码与构建索引.md#v075) `role64_double_wrap_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V077](GEMM_源码与构建索引.md#v077) `role64_coop_double_wrap_reg768`：本索引首次出现该哈希。
- [V078](GEMM_源码与构建索引.md#v078) `role64_coop_single_wrap_reg768`：本索引首次出现该哈希。
- [V079](GEMM_源码与构建索引.md#v079) `role64_coop_double_plain_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_x_cooperative_prefetch/results.json>)；[environment.json](<../results/stage_x_cooperative_prefetch/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-025"></a>
## 025 / Y：早期结构化 buffer / Cache Swizzle

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_y_structured_cache_swizzle`；时间：2026-10-12T19:38:30.469707+08:00。

**改了什么及结论：** 分别对内部A、B、AB使用结构化buffer的idxen/stride路径。此时内部操作数方向仍不同于后期，B还存在提前完成等待，不能孤立解释为cache位本身无效。

**源码位置：** [role64.cpp](<../role64.cpp>)。

历史算法/同步/正确性细节：[原记录§6](<../LDS_WRAP_AND_PIPELINE.md:60>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4356 / 329.16 | 6.6737 / 328.16 | 2.1887 / 333.53 | 8.8829 / 328.73 | 库对照 |
| rocblas_mm_out | 4.4398 / 328.85 | 6.6636 / 328.66 | 2.1845 / 334.17 | 8.8755 / 329.00 | 库对照 |
| [V071](GEMM_源码与构建索引.md#v071) | 7.7763 / 187.75 | 11.8720 / 184.47 | 4.4135 / 165.41 | 15.8401 / 184.34 | 通过 |
| [V075](GEMM_源码与构建索引.md#v075) | 9.0092 / 162.06 | 13.5771 / 161.30 | 4.9511 / 147.44 | 18.1033 / 161.30 | 通过 |
| [V080](GEMM_源码与构建索引.md#v080) | 9.2741 / 157.43 | 13.8694 / 157.90 | 5.1134 / 142.76 | 18.4982 / 157.86 | 通过 |
| [V081](GEMM_源码与构建索引.md#v081) | 10.2736 / 142.11 | 15.4537 / 141.72 | 5.6388 / 129.46 | 20.6071 / 141.70 | 通过 |
| [V082](GEMM_源码与构建索引.md#v082) | 10.6449 / 137.16 | 15.8815 / 137.90 | 5.7797 / 126.30 | 21.2154 / 137.64 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V071](GEMM_源码与构建索引.md#v071) `large_pad16_g8_pad_m_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V075](GEMM_源码与构建索引.md#v075) `role64_double_wrap_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V080](GEMM_源码与构建索引.md#v080) `role64_double_wrap_cachea_reg768`：本索引首次出现该哈希。
- [V081](GEMM_源码与构建索引.md#v081) `role64_double_wrap_cacheb_reg768`：本索引首次出现该哈希。
- [V082](GEMM_源码与构建索引.md#v082) `role64_double_wrap_cacheab_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_y_structured_cache_swizzle/results.json>)；[environment.json](<../results/stage_y_structured_cache_swizzle/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-026"></a>
## 026 / Z：结构化加载的等待位置

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_z_structured_wait_tracking`；时间：2026-10-12T19:42:46.323416+08:00。

**改了什么及结论：** 将B完成等待延至消费前，或用LLVM结构化load让编译器跟踪依赖。全部正确，原型最好约164T，仍慢于通用大tile；不省略数据完成等待。

**源码位置：** [role64.cpp](<../role64.cpp>)。

历史算法/同步/正确性细节：[原记录§6](<../LDS_WRAP_AND_PIPELINE.md:60>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4449 / 328.47 | 6.6501 / 329.32 | 2.1900 / 333.34 | 8.8627 / 329.47 | 库对照 |
| rocblas_mm_out | 4.4335 / 329.32 | 6.6592 / 328.87 | 2.1852 / 334.07 | 8.8737 / 329.07 | 库对照 |
| [V071](GEMM_源码与构建索引.md#v071) | 7.7721 / 187.85 | 11.8697 / 184.51 | 4.4134 / 165.41 | 15.8423 / 184.32 | 通过 |
| [V075](GEMM_源码与构建索引.md#v075) | 9.0058 / 162.12 | 13.5790 / 161.28 | 4.9507 / 147.46 | 18.0885 / 161.43 | 通过 |
| [V083](GEMM_源码与构建索引.md#v083) | 8.8823 / 164.37 | 13.3662 / 163.85 | 4.9053 / 148.82 | 17.8475 / 163.61 | 通过 |
| [V084](GEMM_源码与构建索引.md#v084) | 9.0350 / 161.60 | 13.6759 / 160.14 | 5.0011 / 145.97 | 18.2247 / 160.22 | 通过 |
| [V085](GEMM_源码与构建索引.md#v085) | 9.4178 / 155.03 | 14.0836 / 155.50 | 5.1808 / 140.91 | 18.7708 / 155.56 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V071](GEMM_源码与构建索引.md#v071) `large_pad16_g8_pad_m_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V075](GEMM_源码与构建索引.md#v075) `role64_double_wrap_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V083](GEMM_源码与构建索引.md#v083) `role64_double_wrap_cacheb_latewait_reg768`：本索引首次出现该哈希。
- [V084](GEMM_源码与构建索引.md#v084) `role64_double_wrap_cacheb_llvm_reg768`：本索引首次出现该哈希。
- [V085](GEMM_源码与构建索引.md#v085) `role64_double_wrap_cacheab_llvm_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_z_structured_wait_tracking/results.json>)；[environment.json](<../results/stage_z_structured_wait_tracking/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-027"></a>
## 027 / AA：BK16 / WarpK16 缩小工作集

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_aa_k16_residency`；时间：2026-10-12T19:55:02.636750+08:00。

**改了什么及结论：** 回到通用大tile降低K工作集，Kpack4/BK16候选LDS16KiB、VGPR166，无spill却只有约116T。修复构建名_pad_m误解析为数字padding的问题，保留失败记录。

**源码位置：** [gemm.cpp](<../gemm.cpp>)；[kpack_policy.hpp](<../kpack_policy.hpp>)；[warp_variants.hpp](<../warp_variants.hpp>)；[STAGE_AA_BUILD_RECOVERY.md](<../STAGE_AA_BUILD_RECOVERY.md>)。

历史算法/同步/正确性细节：[原记录§7](<../LDS_WRAP_AND_PIPELINE.md:70>)；[原记录§8](<../LDS_WRAP_AND_PIPELINE.md:78>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4415 / 328.73 | 6.6570 / 328.98 | 2.1899 / 333.35 | 8.9246 / 327.19 | 库对照 |
| rocblas_mm_out | 4.4255 / 329.91 | 6.6539 / 329.14 | 2.1852 / 334.07 | 8.8979 / 328.17 | 库对照 |
| [V071](GEMM_源码与构建索引.md#v071) | 7.7696 / 187.91 | 11.8716 / 184.48 | 4.4151 / 165.34 | 15.8344 / 184.41 | 通过 |
| [V086](GEMM_源码与构建索引.md#v086) | 12.5502 / 116.33 | 17.3819 / 125.99 | 5.2110 / 140.09 | 23.1437 / 126.17 | 通过 |
| [V087](GEMM_源码与构建索引.md#v087) | 12.8008 / 114.06 | 17.8813 / 122.48 | 5.6586 / 129.01 | 23.9007 / 122.17 | 通过 |
| [V088](GEMM_源码与构建索引.md#v088) | 12.8594 / 113.54 | 18.0473 / 121.35 | 5.7073 / 127.91 | 24.2336 / 120.50 | 通过 |
| [V089](GEMM_源码与构建索引.md#v089) | 21.3910 / 68.25 | 31.6694 / 69.15 | 11.1500 / 65.47 | 42.2068 / 69.18 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V071](GEMM_源码与构建索引.md#v071) `large_pad16_g8_pad_m_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V086](GEMM_源码与构建索引.md#v086) `large_wk16_kpack4_g8_pad_m_reg768`：本索引首次出现该哈希。
- [V087](GEMM_源码与构建索引.md#v087) `large_wk16_kpack8_g8_pad_m_reg768`：本索引首次出现该哈希。
- [V088](GEMM_源码与构建索引.md#v088) `large_wk16_pad8_g8_pad_m_reg768`：本索引首次出现该哈希。
- [V089](GEMM_源码与构建索引.md#v089) `large_wk16_pad16_g8_pad_m_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_aa_k16_residency/results.json>)；[environment.json](<../results/stage_aa_k16_residency/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-028"></a>
## 028 / AB：priority 与编译调度屏障对照

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_ab_scheduling_controls`；时间：2026-10-12T20:01:01.100277+08:00。

**改了什么及结论：** 分别取消priority和编译调度屏障，不移除GPU真实同步。原型部分组合升至约170T，但仍不胜过通用188T。

**源码位置：** [role64.cpp](<../role64.cpp>)。

历史算法/同步/正确性细节：[原记录§8](<../LDS_WRAP_AND_PIPELINE.md:78>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4625 / 327.18 | 6.6666 / 328.51 | 2.1927 / 332.93 | 8.9192 / 327.39 | 库对照 |
| rocblas_mm_out | 4.4605 / 327.32 | 6.6461 / 329.52 | 2.1868 / 333.83 | 8.9008 / 328.07 | 库对照 |
| [V071](GEMM_源码与构建索引.md#v071) | 7.7853 / 187.54 | 11.8748 / 184.43 | 4.4138 / 165.39 | 15.8345 / 184.41 | 通过 |
| [V075](GEMM_源码与构建索引.md#v075) | 9.0098 / 162.05 | 13.5785 / 161.29 | 4.9553 / 147.32 | 18.0948 / 161.37 | 通过 |
| [V090](GEMM_源码与构建索引.md#v090) | 8.8457 / 165.05 | 13.2237 / 165.61 | 4.8415 / 150.78 | 17.6173 / 165.75 | 通过 |
| [V091](GEMM_源码与构建索引.md#v091) | 9.0043 / 162.15 | 13.5705 / 161.38 | 4.9511 / 147.44 | 18.0799 / 161.51 | 通过 |
| [V092](GEMM_源码与构建索引.md#v092) | 8.5703 / 170.36 | 12.8949 / 169.84 | 4.7459 / 153.82 | 17.1829 / 169.94 | 通过 |
| [V093](GEMM_源码与构建索引.md#v093) | 8.9906 / 162.39 | 13.5507 / 161.62 | 4.9460 / 147.60 | 18.0556 / 161.72 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V071](GEMM_源码与构建索引.md#v071) `large_pad16_g8_pad_m_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V075](GEMM_源码与构建索引.md#v075) `role64_double_wrap_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V090](GEMM_源码与构建索引.md#v090) `role64_double_wrap_noprio_reg768`：本索引首次出现该哈希。
- [V091](GEMM_源码与构建索引.md#v091) `role64_double_wrap_nosched_reg768`：本索引首次出现该哈希。
- [V092](GEMM_源码与构建索引.md#v092) `role64_double_wrap_nosched_noprio_reg768`：本索引首次出现该哈希。
- [V093](GEMM_源码与构建索引.md#v093) `role64_double_wrap_postprio_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_ab_scheduling_controls/results.json>)；[environment.json](<../results/stage_ab_scheduling_controls/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-029"></a>
## 029 / AC：对齐 rocBLAS 的内部操作数方向

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_ac_blas_operand_orientation`；时间：2026-10-12T20:07:21.723985+08:00。

**改了什么及结论：** 实际BLAS输入指针证明内部A=权重W、B=激活X；改成W@X.T的列主序计算并直接写回原X@W.T地址，不增加transpose kernel。对照feature/token分组，并独立测真实M18880。

**源码位置：** [role64.cpp](<../role64.cpp>)；[probe_orientation.py](<../probe_orientation.py>)；[analyze_orientation.py](<../analyze_orientation.py>)。

历史算法/同步/正确性细节：[原记录§8](<../LDS_WRAP_AND_PIPELINE.md:78>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4371 / 329.05 | 6.6629 / 328.69 | 2.1982 / 332.10 | 8.8945 / 328.30 | 库对照 |
| rocblas_mm_out | 4.4294 / 329.62 | 6.6410 / 329.77 | 2.1900 / 333.33 | 8.8900 / 328.46 | 库对照 |
| [V071](GEMM_源码与构建索引.md#v071) | 7.7734 / 187.82 | 11.8747 / 184.43 | 4.4153 / 165.34 | 15.8441 / 184.30 | 通过 |
| [V075](GEMM_源码与构建索引.md#v075) | 9.0085 / 162.07 | 13.5764 / 161.31 | 4.9519 / 147.42 | 18.0893 / 161.42 | 通过 |
| [V094](GEMM_源码与构建索引.md#v094) | 8.4780 / 172.21 | 12.6268 / 173.44 | 4.4398 / 164.42 | 16.8367 / 173.43 | 通过 |
| [V095](GEMM_源码与构建索引.md#v095) | 8.4466 / 172.85 | 12.4962 / 175.26 | 4.4230 / 165.05 | 16.6630 / 175.24 | 通过 |
| [V096](GEMM_源码与构建索引.md#v096) | 8.0190 / 182.07 | 11.8657 / 184.57 | 4.2267 / 172.71 | 15.8110 / 184.68 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V071](GEMM_源码与构建索引.md#v071) `large_pad16_g8_pad_m_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V075](GEMM_源码与构建索引.md#v075) `role64_double_wrap_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V094](GEMM_源码与构建索引.md#v094) `role64_double_wrap_trans_reg768`：本索引首次出现该哈希。
- [V095](GEMM_源码与构建索引.md#v095) `role64_double_wrap_trans_groupn_reg768`：本索引首次出现该哈希。
- [V096](GEMM_源码与构建索引.md#v096) `role64_double_wrap_trans_groupn_nosched_noprio_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_ac_blas_operand_orientation/results.json>)；[environment.json](<../results/stage_ac_blas_operand_orientation/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-030"></a>
## 030 / AC：对齐 rocBLAS 的内部操作数方向

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_ac_native_m18880`；时间：2026-10-12T20:12:20.244362+08:00。

**改了什么及结论：** 实际BLAS输入指针证明内部A=权重W、B=激活X；改成W@X.T的列主序计算并直接写回原X@W.T地址，不增加transpose kernel。对照feature/token分组，并独立测真实M18880。

**源码位置：** [role64.cpp](<../role64.cpp>)；[probe_orientation.py](<../probe_orientation.py>)；[analyze_orientation.py](<../analyze_orientation.py>)。

历史算法/同步/正确性细节：[原记录§8](<../LDS_WRAP_AND_PIPELINE.md:78>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4541 / 326.68 | 6.6711 / 327.18 | 2.1857 / 332.87 | 8.8651 / 328.27 | 库对照 |
| rocblas_mm_out | 4.4443 / 327.41 | 6.6380 / 328.81 | 2.1834 / 333.22 | 8.8670 / 328.20 | 库对照 |
| [V071](GEMM_源码与构建索引.md#v071) | 7.7616 / 187.47 | 11.8711 / 183.86 | 4.4102 / 164.97 | 15.8333 / 183.80 | 通过 |
| [V096](GEMM_源码与构建索引.md#v096) | 8.0155 / 181.53 | 11.8536 / 184.13 | 4.2188 / 172.45 | 15.7856 / 184.36 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V071](GEMM_源码与构建索引.md#v071) `large_pad16_g8_pad_m_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V096](GEMM_源码与构建索引.md#v096) `role64_double_wrap_trans_groupn_nosched_noprio_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

原始证据：[原始 results.json](<../results/stage_ac_native_m18880/results.json>)；[environment.json](<../results/stage_ac_native_m18880/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-031"></a>
## 031 / AD：通用 CK 的列主序输出

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_ad_generic_column_output`；时间：2026-10-12T20:22:20.716979+08:00。

**改了什么及结论：** 把交换内部操作数和ColumnMajor C view迁回通用模板，增加按N分组的partitioner。没有额外转置算子；却降到约50–54T，后续定位为向量加载被padding启发式禁用。

**源码位置：** [gemm.cpp](<../gemm.cpp>)；[grouped_n_partitioner.hpp](<../grouped_n_partitioner.hpp>)。

历史算法/同步/正确性细节：[原记录§9](<../LDS_WRAP_AND_PIPELINE.md:90>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4749 / 326.27 | 6.6616 / 328.76 | 2.1990 / 331.97 | 8.9186 / 327.41 | 库对照 |
| rocblas_mm_out | 4.4642 / 327.05 | 6.6536 / 329.15 | 2.1837 / 334.31 | 8.8821 / 328.76 | 库对照 |
| [V071](GEMM_源码与构建索引.md#v071) | 7.7807 / 187.65 | 11.8756 / 184.41 | 4.4135 / 165.40 | 15.8324 / 184.43 | 通过 |
| [V097](GEMM_源码与构建索引.md#v097) | 28.8012 / 50.69 | 42.3680 / 51.69 | 14.6070 / 49.98 | 56.4958 / 51.69 | 通过 |
| [V098](GEMM_源码与构建索引.md#v098) | 28.8751 / 50.56 | 42.3614 / 51.70 | 14.6016 / 50.00 | 56.4883 / 51.69 | 通过 |
| [V099](GEMM_源码与构建索引.md#v099) | 27.7049 / 52.70 | 40.2335 / 54.43 | 13.7821 / 52.97 | 53.6283 / 54.45 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V071](GEMM_源码与构建索引.md#v071) `large_pad16_g8_pad_m_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V097](GEMM_源码与构建索引.md#v097) `large_pad16_g8_trans_pad_m_reg768`：本索引首次出现该哈希。
- [V098](GEMM_源码与构建索引.md#v098) `large_pad16_g8_trans_groupn_pad_m_reg768`：本索引首次出现该哈希。
- [V099](GEMM_源码与构建索引.md#v099) `large_kpack8_g8_trans_groupn_pad_m_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_ad_generic_column_output/results.json>)；[environment.json](<../results/stage_ad_generic_column_output/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-032"></a>
## 032 / AE：VGPR 起点奇偶对照

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_ae_vgpr_allocation_shift`；时间：2026-10-12T20:37:20.653127+08:00。

**改了什么及结论：** 让额外活跃值移动MMAC输入寄存器起点，测试AD回退是否由简单对齐解释。约50T退化仍在，不能归因于首条寄存器起点奇偶。

**源码位置：** [gemm.cpp](<../gemm.cpp>)。

历史算法/同步/正确性细节：[原记录§9](<../LDS_WRAP_AND_PIPELINE.md:90>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4431 / 328.61 | 6.6640 / 328.64 | 2.1985 / 332.05 | 8.9061 / 327.87 | 库对照 |
| rocblas_mm_out | 4.4283 / 329.70 | 6.6608 / 328.79 | 2.1856 / 334.01 | 8.8814 / 328.78 | 库对照 |
| [V071](GEMM_源码与构建索引.md#v071) | 7.7793 / 187.68 | 11.8745 / 184.43 | 4.4138 / 165.39 | 15.8401 / 184.34 | 通过 |
| [V098](GEMM_源码与构建索引.md#v098) | 28.8895 / 50.54 | 42.3529 / 51.71 | 14.5974 / 50.01 | 56.4960 / 51.69 | 通过 |
| [V100](GEMM_源码与构建索引.md#v100) | 28.8174 / 50.66 | 42.2645 / 51.82 | 14.5682 / 50.11 | 56.3607 / 51.81 | 通过 |
| [V101](GEMM_源码与构建索引.md#v101) | 28.8296 / 50.64 | 42.3158 / 51.75 | 14.5879 / 50.04 | 56.4354 / 51.74 | 通过 |
| [V102](GEMM_源码与构建索引.md#v102) | 28.9162 / 50.49 | 42.4535 / 51.59 | 14.6280 / 49.91 | 56.6026 / 51.59 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V071](GEMM_源码与构建索引.md#v071) `large_pad16_g8_pad_m_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V098](GEMM_源码与构建索引.md#v098) `large_pad16_g8_trans_groupn_pad_m_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V100](GEMM_源码与构建索引.md#v100) `large_pad16_g8_trans_groupn_lpad1_pad_m_reg768`：本索引首次出现该哈希。
- [V101](GEMM_源码与构建索引.md#v101) `large_pad16_g8_trans_groupn_lpad2_pad_m_reg768`：本索引首次出现该哈希。
- [V102](GEMM_源码与构建索引.md#v102) `large_pad16_g8_trans_groupn_lpad3_pad_m_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_ae_vgpr_allocation_shift/results.json>)；[environment.json](<../results/stage_ae_vgpr_allocation_shift/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-033"></a>
## 033 / AF：恢复连续 K 的 vec8 读取

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_af_contiguous_vector_restore`；时间：2026-10-13T10:22:57.325041+08:00。

**改了什么及结论：** 私有Problem覆盖VectorSizeB=8，修正kPadN把连续K方向降为scalar的推导，同时保留尾行保护。增加输入16-byte对齐/K%8契约；ISA ushort32→0、dwordx4 4→8，真实四shape约203–213T。

**源码位置：** [gemm.cpp](<../gemm.cpp>)。

历史算法/同步/正确性细节：[原记录§9](<../LDS_WRAP_AND_PIPELINE.md:90>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18944, 5376, 7168]；qkv=[18944, 10752, 5376]；out_proj=[18944, 5376, 3584]；fc1=[18944, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4622 / 327.19 | 6.6741 / 328.14 | 2.1885 / 333.57 | 8.8978 / 328.17 | 库对照 |
| rocblas_mm_out | 4.4592 / 327.42 | 6.6508 / 329.29 | 2.1842 / 334.22 | 8.9170 / 327.47 | 库对照 |
| [V071](GEMM_源码与构建索引.md#v071) | 7.7846 / 187.55 | 11.8715 / 184.48 | 4.4148 / 165.35 | 15.8368 / 184.38 | 通过 |
| [V098](GEMM_源码与构建索引.md#v098) | 28.8823 / 50.55 | 42.3285 / 51.74 | 14.5931 / 50.02 | 56.4965 / 51.69 | 通过 |
| [V103](GEMM_源码与构建索引.md#v103) | 7.0399 / 207.39 | 10.2381 / 213.91 | 3.5857 / 203.59 | 13.7026 / 213.10 | 通过 |
| [V104](GEMM_源码与构建索引.md#v104) | 7.0398 / 207.39 | 10.2502 / 213.66 | 3.5851 / 203.62 | 13.6807 / 213.44 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V071](GEMM_源码与构建索引.md#v071) `large_pad16_g8_pad_m_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V098](GEMM_源码与构建索引.md#v098) `large_pad16_g8_trans_groupn_pad_m_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V103](GEMM_源码与构建索引.md#v103) `large_pad16_g8_trans_groupn_aligned_pad_m_reg768`：本索引首次出现该哈希。
- [V104](GEMM_源码与构建索引.md#v104) `large_pad16_g8_trans_groupn_vecb_pad_m_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_af_contiguous_vector_restore/results.json>)；[environment.json](<../results/stage_af_contiguous_vector_restore/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-034"></a>
## 034 / AF：恢复连续 K 的 vec8 读取

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_af_native_m18880`；时间：2026-10-13T10:25:06.080544+08:00。

**改了什么及结论：** 私有Problem覆盖VectorSizeB=8，修正kPadN把连续K方向降为scalar的推导，同时保留尾行保护。增加输入16-byte对齐/K%8契约；ISA ushort32→0、dwordx4 4→8，真实四shape约203–213T。

**源码位置：** [gemm.cpp](<../gemm.cpp>)。

历史算法/同步/正确性细节：[原记录§9](<../LDS_WRAP_AND_PIPELINE.md:90>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4364 / 327.99 | 6.6605 / 327.70 | 2.1874 / 332.60 | 8.8628 / 328.36 | 库对照 |
| rocblas_mm_out | 4.4483 / 327.11 | 6.6472 / 328.35 | 2.1856 / 332.89 | 8.8622 / 328.38 | 库对照 |
| [V071](GEMM_源码与构建索引.md#v071) | 7.7563 / 187.60 | 11.8684 / 183.90 | 4.4120 / 164.90 | 15.8239 / 183.91 | 通过 |
| [V104](GEMM_源码与构建索引.md#v104) | 7.0587 / 206.14 | 10.2657 / 212.61 | 3.5826 / 203.08 | 13.6956 / 212.49 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V071](GEMM_源码与构建索引.md#v071) `large_pad16_g8_pad_m_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V104](GEMM_源码与构建索引.md#v104) `large_pad16_g8_trans_groupn_vecb_pad_m_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

原始证据：[原始 results.json](<../results/stage_af_native_m18880/results.json>)；[environment.json](<../results/stage_af_native_m18880/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-035"></a>
## 035 / AG：正确 A/B 方向下重测 cache

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_ag_transposed_cache_swizzle`；时间：2026-10-13T10:26:28.766070+08:00。

**改了什么及结论：** 在A=权重、B=激活的转置原型上重新比较结构化A/B/AB；所有配置正确但更慢。AF的收益来自向量化恢复，与本轮cache无关。

**源码位置：** [role64.cpp](<../role64.cpp>)。

历史算法/同步/正确性细节：[原记录§10](<../LDS_WRAP_AND_PIPELINE.md:109>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4297 / 328.49 | 6.6425 / 328.58 | 2.1914 / 332.00 | 8.9091 / 326.65 | 库对照 |
| rocblas_mm_out | 4.4313 / 328.37 | 6.6346 / 328.98 | 2.1873 / 332.62 | 8.8702 / 328.08 | 库对照 |
| [V104](GEMM_源码与构建索引.md#v104) | 7.0589 / 206.14 | 10.2435 / 213.08 | 3.5861 / 202.88 | 13.6515 / 213.18 | 通过 |
| [V096](GEMM_源码与构建索引.md#v096) | 8.0093 / 181.68 | 11.8512 / 184.17 | 4.2181 / 172.48 | 15.7853 / 184.36 | 通过 |
| [V105](GEMM_源码与构建索引.md#v105) | 8.1055 / 179.52 | 11.9944 / 181.97 | 4.2770 / 170.11 | 15.9848 / 182.06 | 通过 |
| [V106](GEMM_源码与构建索引.md#v106) | 8.2465 / 176.45 | 12.2911 / 177.58 | 4.3591 / 166.90 | 16.3360 / 178.15 | 通过 |
| [V107](GEMM_源码与构建索引.md#v107) | 8.3756 / 173.73 | 12.4836 / 174.84 | 4.4218 / 164.54 | 16.6488 / 174.80 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V104](GEMM_源码与构建索引.md#v104) `large_pad16_g8_trans_groupn_vecb_pad_m_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V096](GEMM_源码与构建索引.md#v096) `role64_double_wrap_trans_groupn_nosched_noprio_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V105](GEMM_源码与构建索引.md#v105) `role64_double_wrap_trans_groupn_nosched_noprio_cachea_reg768`：本索引首次出现该哈希。
- [V106](GEMM_源码与构建索引.md#v106) `role64_double_wrap_trans_groupn_nosched_noprio_cacheb_llvm_reg768`：本索引首次出现该哈希。
- [V107](GEMM_源码与构建索引.md#v107) `role64_double_wrap_trans_groupn_nosched_noprio_cacheab_llvm_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_ag_transposed_cache_swizzle/results.json>)；[environment.json](<../results/stage_ag_transposed_cache_swizzle/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-036"></a>
## 036 / AH：向量化恢复后的 tile 遍历

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_ah_vector_group_tile`；时间：2026-10-13T10:31:53.842830+08:00。

**改了什么及结论：** 在vec8修复的通用版本上测试token分组4/8/16/32及128×512宽token tile。保留四shape结果，未当作最终分工流水的结论。

**源码位置：** [gemm.cpp](<../gemm.cpp>)；[grouped_partitioner.hpp](<../grouped_partitioner.hpp>)。

历史算法/同步/正确性细节：[原记录§11](<../LDS_WRAP_AND_PIPELINE.md:131>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4447 / 327.38 | 6.6506 / 328.19 | 2.2020 / 330.40 | 8.8947 / 327.18 | 库对照 |
| rocblas_mm_out | 4.4357 / 328.04 | 6.6642 / 327.52 | 2.1965 / 331.23 | 8.8888 / 327.40 | 库对照 |
| [V104](GEMM_源码与构建索引.md#v104) | 7.0550 / 206.25 | 10.2746 / 212.43 | 3.5913 / 202.59 | 13.6281 / 213.54 | 通过 |
| [V108](GEMM_源码与构建索引.md#v108) | 6.9789 / 208.50 | 10.3141 / 211.62 | 3.6226 / 200.84 | 13.7180 / 212.14 | 通过 |
| [V109](GEMM_源码与构建索引.md#v109) | 7.1945 / 202.25 | 10.4407 / 209.05 | 3.6031 / 201.92 | 13.8055 / 210.80 | 通过 |
| [V110](GEMM_源码与构建索引.md#v110) | 7.5367 / 193.07 | 10.5858 / 206.18 | 3.6610 / 198.73 | 14.2952 / 203.58 | 通过 |
| [V111](GEMM_源码与构建索引.md#v111) | 8.6997 / 167.26 | 12.6982 / 171.88 | 4.3940 / 165.58 | 16.9256 / 171.94 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V104](GEMM_源码与构建索引.md#v104) `large_pad16_g8_trans_groupn_vecb_pad_m_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V108](GEMM_源码与构建索引.md#v108) `large_pad16_g8_trans_groupn4_vecb_pad_m_reg768`：本索引首次出现该哈希。
- [V109](GEMM_源码与构建索引.md#v109) `large_pad16_g8_trans_groupn16_vecb_pad_m_reg768`：本索引首次出现该哈希。
- [V110](GEMM_源码与构建索引.md#v110) `large_pad16_g8_trans_groupn32_vecb_pad_m_reg768`：本索引首次出现该哈希。
- [V111](GEMM_源码与构建索引.md#v111) `tokenwide_pad16_g8_trans_groupn_vecb_pad_m_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_ah_vector_group_tile/results.json>)；[environment.json](<../results/stage_ah_vector_group_tile/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-037"></a>
## 037 / AI：K32 相位预取与 1×8 wave 分区

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_ai_phase_register_prefetch`；时间：2026-10-13T10:36:14.665199+08:00。

**改了什么及结论：** 新rolephase.cpp让权重经LDS、激活直接进入compute VGPR，把B预取寿命从整K64缩到K32。1×8计算分区约213–229T；2×4/4×2虽然减少DS读，却增加激活逻辑重复加载并显著回退。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§12](<../LDS_WRAP_AND_PIPELINE.md:135>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4588 / 326.34 | 6.6557 / 327.94 | 2.1907 / 332.11 | 8.8940 / 327.21 | 库对照 |
| rocblas_mm_out | 4.4548 / 326.63 | 6.6611 / 327.67 | 2.1844 / 333.06 | 8.8517 / 328.77 | 库对照 |
| [V104](GEMM_源码与构建索引.md#v104) | 7.0380 / 206.75 | 10.2618 / 212.69 | 3.5854 / 202.92 | 13.7072 / 212.31 | 通过 |
| [V096](GEMM_源码与构建索引.md#v096) | 8.0126 / 181.60 | 11.8548 / 184.11 | 4.2231 / 172.28 | 15.7997 / 184.19 | 通过 |
| [V112](GEMM_源码与构建索引.md#v112) | 6.4423 / 225.87 | 9.5414 / 228.75 | 3.4236 / 212.51 | 12.7138 / 228.90 | 通过 |
| [V113](GEMM_源码与构建索引.md#v113) | 8.7197 / 166.87 | 12.8075 / 170.42 | 4.4327 / 164.13 | 17.1473 / 169.72 | 通过 |
| [V114](GEMM_源码与构建索引.md#v114) | 15.8898 / 91.57 | 23.3502 / 93.47 | 8.0982 / 89.84 | 31.1283 / 93.49 | 通过 |
| [V115](GEMM_源码与构建索引.md#v115) | 15.8785 / 91.64 | 23.3466 / 93.49 | 8.0991 / 89.83 | 31.1214 / 93.51 | 通过 |

**xprof / DTK 表现：** 与AK及rocBLAS另轮比较VM/LGKM/barrier/LDS等待；AI为旧phase对照。 证据P01：[results/wait_phase_20261013/analysis.json](<../results/wait_phase_20261013/analysis.json>)；[LDS_WRAP_AND_PIPELINE.md](<../LDS_WRAP_AND_PIPELINE.md>)。

**候选对应关系与配置：**

- [V104](GEMM_源码与构建索引.md#v104) `large_pad16_g8_trans_groupn_vecb_pad_m_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V096](GEMM_源码与构建索引.md#v096) `role64_double_wrap_trans_groupn_nosched_noprio_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V112](GEMM_源码与构建索引.md#v112) `rolephase_m1_reg768`：本索引首次出现该哈希。
- [V113](GEMM_源码与构建索引.md#v113) `rolephase_m2_reg768`：本索引首次出现该哈希。
- [V114](GEMM_源码与构建索引.md#v114) `rolephase_m4_reg768`：本索引首次出现该哈希。
- [V115](GEMM_源码与构建索引.md#v115) `rolephase_m4_priority_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_ai_phase_register_prefetch/results.json>)；[environment.json](<../results/stage_ai_phase_register_prefetch/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-038"></a>
## 038 / AJ：K32 相位流水的 cache 对照

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_aj_phase_cache_swizzle`；时间：2026-10-13T10:41:51.036624+08:00。

**改了什么及结论：** 分别给新phase流水的A/B/AB加结构化cache，同轮raw控制保持一致。FC2 raw225.8T，A/B/AB约220.6/214.3/209.9T，均不采用。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§12](<../LDS_WRAP_AND_PIPELINE.md:135>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4518 / 326.85 | 6.6548 / 327.98 | 2.1948 / 331.49 | 8.8771 / 327.83 | 库对照 |
| rocblas_mm_out | 4.4457 / 327.30 | 6.6287 / 329.27 | 2.1906 / 332.12 | 8.8825 / 327.63 | 库对照 |
| [V112](GEMM_源码与构建索引.md#v112) | 6.4438 / 225.81 | 9.5400 / 228.79 | 3.4205 / 212.70 | 12.7139 / 228.90 | 通过 |
| [V116](GEMM_源码与构建索引.md#v116) | 6.5974 / 220.56 | 9.7716 / 223.37 | 3.5276 / 206.24 | 13.0266 / 223.40 | 通过 |
| [V117](GEMM_源码与构建索引.md#v117) | 6.7902 / 214.29 | 10.1239 / 215.59 | 3.6129 / 201.38 | 13.4715 / 216.03 | 通过 |
| [V118](GEMM_源码与构建索引.md#v118) | 6.9309 / 209.94 | 10.2405 / 213.14 | 3.6084 / 201.63 | 13.6655 / 212.96 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V112](GEMM_源码与构建索引.md#v112) `rolephase_m1_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V116](GEMM_源码与构建索引.md#v116) `rolephase_m1_cachea_reg768`：本索引首次出现该哈希。
- [V117](GEMM_源码与构建索引.md#v117) `rolephase_m1_cacheb_reg768`：本索引首次出现该哈希。
- [V118](GEMM_源码与构建索引.md#v118) `rolephase_m1_cacheab_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_aj_phase_cache_swizzle/results.json>)；[environment.json](<../results/stage_aj_phase_cache_swizzle/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-039"></a>
## 039 / AK：B 寄存器双 bank + 显式局部等待

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_ak_banked_wait_schedule`；时间：2026-10-13T10:50:46.559334+08:00。

**改了什么及结论：** 两个固定B bank轮换；先发更年轻future，再vmcnt(2)完成旧current，末相位vmcnt(0)。保持startup/barrier/terminal drain，避免不必要的bank复制；约223–240T，另做72组广审计。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)；[stress_phase.py](<../stress_phase.py>)。

历史算法/同步/正确性细节：[原记录§13](<../LDS_WRAP_AND_PIPELINE.md:168>)；[原记录§15](<../LDS_WRAP_AND_PIPELINE.md:195>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4380 / 327.87 | 6.6614 / 327.65 | 2.1876 / 332.57 | 8.9121 / 326.54 | 库对照 |
| rocblas_mm_out | 4.4295 / 328.50 | 6.6562 / 327.91 | 2.1831 / 333.26 | 8.8793 / 327.75 | 库对照 |
| [V112](GEMM_源码与构建索引.md#v112) | 6.4467 / 225.71 | 9.5379 / 228.84 | 3.4200 / 212.73 | 12.7091 / 228.98 | 通过 |
| [V119](GEMM_源码与构建索引.md#v119) | 7.0282 / 207.04 | 10.4522 / 208.82 | 3.6943 / 196.93 | 13.9372 / 208.81 | 通过 |
| [V120](GEMM_源码与构建索引.md#v120) | 7.1802 / 202.65 | 10.5625 / 206.64 | 3.7641 / 193.28 | 14.0829 / 206.65 | 通过 |
| [V121](GEMM_源码与构建索引.md#v121) | 6.0861 / 239.08 | 9.1013 / 239.81 | 3.2694 / 222.53 | 12.1351 / 239.82 | 通过 |
| [V122](GEMM_源码与构建索引.md#v122) | 6.8043 / 213.85 | 10.1277 / 215.51 | 3.6082 / 201.63 | 13.4943 / 215.66 | 通过 |

**xprof / DTK 表现：** 双bank方案VM等待均值由364.24M降至115.24M，但旧phase范围很宽；LDS issue升至155.86M，不能解释成净延迟下降69%。另作为P02的旧对照。 证据P01：[results/wait_phase_20261013/analysis.json](<../results/wait_phase_20261013/analysis.json>)；[LDS_WRAP_AND_PIPELINE.md](<../LDS_WRAP_AND_PIPELINE.md>)。

**候选对应关系与配置：**

- [V112](GEMM_源码与构建索引.md#v112) `rolephase_m1_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V119](GEMM_源码与构建索引.md#v119) `rolephase_m1_priority_reg768`：本索引首次出现该哈希。
- [V120](GEMM_源码与构建索引.md#v120) `rolephase_m1_bank_reg768`：本索引首次出现该哈希。
- [V121](GEMM_源码与构建索引.md#v121) `rolephase_m1_bank_asmb_reg768`：本索引首次出现该哈希。
- [V122](GEMM_源码与构建索引.md#v122) `rolephase_m1_bank_asmb_priority_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_ak_banked_wait_schedule/results.json>)；[environment.json](<../results/stage_ak_banked_wait_schedule/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-040"></a>
## 040 / AL：权重 K32 / 32KiB LDS

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_al_weight_k32_buffer`；时间：2026-10-13T11:00:09.406909+08:00。

**改了什么及结论：** 硬件probe验证8192元素新wrap后，权重双buffer从64KiB减到32KiB，但每K64增加READY/ACK。仅约176–188T；资源小不等于实际更快，精确rocBLAS ELF仍为64KiB。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)；[lds_wrap_probe.cpp](<../lds_wrap_probe.cpp>)；[inspect_reference_resources.py](<../inspect_reference_resources.py>)。

历史算法/同步/正确性细节：[原记录§14](<../LDS_WRAP_AND_PIPELINE.md:187>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4355 / 328.06 | 6.6739 / 327.04 | 2.1978 / 331.03 | 8.9013 / 326.94 | 库对照 |
| rocblas_mm_out | 4.4137 / 329.67 | 6.6423 / 328.59 | 2.1913 / 332.02 | 8.8596 / 328.48 | 库对照 |
| [V112](GEMM_源码与构建索引.md#v112) | 6.4424 / 225.86 | 9.5420 / 228.74 | 3.4208 / 212.68 | 12.7073 / 229.02 | 通过 |
| [V121](GEMM_源码与构建索引.md#v121) | 6.0875 / 239.03 | 9.1028 / 239.78 | 3.2712 / 222.41 | 12.1358 / 239.80 | 通过 |
| [V123](GEMM_源码与构建索引.md#v123) | 7.8747 / 184.78 | 11.7283 / 186.10 | 4.1666 / 174.61 | 15.6327 / 186.16 | 通过 |
| [V124](GEMM_源码与构建索引.md#v124) | 7.7912 / 186.76 | 11.6114 / 187.97 | 4.1232 / 176.45 | 15.4795 / 188.00 | 通过 |
| [V125](GEMM_源码与构建索引.md#v125) | 7.8428 / 185.53 | 11.6857 / 186.78 | 4.1485 / 175.38 | 15.5770 / 186.82 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V112](GEMM_源码与构建索引.md#v112) `rolephase_m1_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V121](GEMM_源码与构建索引.md#v121) `rolephase_m1_bank_asmb_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V123](GEMM_源码与构建索引.md#v123) `rolephase_m1_weight32_reg768`：本索引首次出现该哈希。
- [V124](GEMM_源码与构建索引.md#v124) `rolephase_m1_weight32_bank_asmb_reg768`：本索引首次出现该哈希。
- [V125](GEMM_源码与构建索引.md#v125) `rolephase_m1_weight32_bank_asmb_priority_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_al_weight_k32_buffer/results.json>)；[environment.json](<../results/stage_al_weight_k32_buffer/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-041"></a>
## 041 / AM：producer 发射推迟与相位对照

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_am_ready_wait_and_phase_stagger`；时间：2026-10-13T11:21:48.062899+08:00。

**改了什么及结论：** 推迟producer下一A加载到READY后ACK前，或让上半wave组交换K32相位。推迟方案回退，交换相位的小浮动不作为确定收益；不能据原生不同wave组直接推断K顺序倒置。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§16](<../LDS_WRAP_AND_PIPELINE.md:210>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4341 / 328.16 | 6.6599 / 327.73 | 2.1982 / 330.97 | 8.9225 / 326.16 | 库对照 |
| rocblas_mm_out | 4.4359 / 328.03 | 6.6437 / 328.53 | 2.1845 / 333.05 | 8.8974 / 327.08 | 库对照 |
| [V121](GEMM_源码与构建索引.md#v121) | 6.0865 / 239.07 | 9.1020 / 239.80 | 3.2712 / 222.41 | 12.1358 / 239.80 | 通过 |
| [V126](GEMM_源码与构建索引.md#v126) | 6.4419 / 225.88 | 9.5102 / 229.50 | 3.4092 / 213.41 | 12.6902 / 229.32 | 通过 |
| [V127](GEMM_源码与构建索引.md#v127) | 6.0787 / 239.37 | 9.0963 / 239.95 | 3.2675 / 222.66 | 12.1259 / 240.00 | 通过 |
| [V128](GEMM_源码与构建索引.md#v128) | 6.4656 / 225.05 | 9.5429 / 228.72 | 3.4246 / 212.45 | 12.7163 / 228.85 | 通过 |
| [V129](GEMM_源码与构建索引.md#v129) | 6.5177 / 223.25 | 9.5464 / 228.63 | 3.4244 / 212.46 | 12.7882 / 227.57 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V121](GEMM_源码与构建索引.md#v121) `rolephase_m1_bank_asmb_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V126](GEMM_源码与构建索引.md#v126) `rolephase_m1_readywait_bank_asmb_reg768`：本索引首次出现该哈希。
- [V127](GEMM_源码与构建索引.md#v127) `rolephase_m1_swap_bank_asmb_reg768`：本索引首次出现该哈希。
- [V128](GEMM_源码与构建索引.md#v128) `rolephase_m1_readywait_swap_bank_asmb_reg768`：本索引首次出现该哈希。
- [V129](GEMM_源码与构建索引.md#v129) `rolephase_m1_readywait_bank_asmb_priority_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_am_ready_wait_and_phase_stagger/results.json>)；[environment.json](<../results/stage_am_ready_wait_and_phase_stagger/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-042"></a>
## 042 / AN：显式 DS 顺序 / 渐进 LGKM wait

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_an_ordered_lds_reads`；时间：2026-10-13T11:29:38.878592+08:00。

**改了什么及结论：** 按消费顺序发射ds_read_b128，比较完整或逐渐降低的LGKM等待及post-priority。正确但FC2约208–226T，落后239T控制。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§16](<../LDS_WRAP_AND_PIPELINE.md:210>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4623 / 326.08 | 6.6473 / 328.35 | 2.1990 / 330.85 | 8.8716 / 328.03 | 库对照 |
| rocblas_mm_out | 4.4510 / 326.92 | 6.6459 / 328.42 | 2.1888 / 332.40 | 8.8808 / 327.69 | 库对照 |
| [V121](GEMM_源码与构建索引.md#v121) | 6.0881 / 239.00 | 9.1032 / 239.76 | 3.2721 / 222.35 | 12.1356 / 239.80 | 通过 |
| [V130](GEMM_源码与构建索引.md#v130) | 6.6919 / 217.44 | 9.9499 / 219.36 | 3.5593 / 204.41 | 13.2616 / 219.44 | 通过 |
| [V131](GEMM_源码与构建索引.md#v131) | 7.0012 / 207.83 | 10.4644 / 208.58 | 3.7318 / 194.96 | 13.9494 / 208.62 | 通过 |
| [V132](GEMM_源码与构建索引.md#v132) | 6.4392 / 225.97 | 9.6270 / 226.72 | 3.4507 / 210.84 | 12.8331 / 226.77 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V121](GEMM_源码与构建索引.md#v121) `rolephase_m1_bank_asmb_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V130](GEMM_源码与构建索引.md#v130) `rolephase_m1_bank_asmb_dsasm_reg768`：本索引首次出现该哈希。
- [V131](GEMM_源码与构建索引.md#v131) `rolephase_m1_bank_asmb_dsasm_postprio_reg768`：本索引首次出现该哈希。
- [V132](GEMM_源码与构建索引.md#v132) `rolephase_m1_bank_asmb_dsprogress_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_an_ordered_lds_reads/results.json>)；[environment.json](<../results/stage_an_ordered_lds_reads/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-043"></a>
## 043 / AO：额外活跃 VGPR 对齐试探

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_ao_phase_vgpr_alignment`；时间：2026-10-13T11:34:01.909701+08:00。

**改了什么及结论：** 保持1/2/3个附加VGPR活跃，尝试移动寄存器分配。实际首条MMAC操作数起点没有改变，计时也无收益，因此不构成所有对齐策略的反证。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§16](<../LDS_WRAP_AND_PIPELINE.md:210>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4325 / 328.28 | 6.6626 / 327.59 | 2.1892 / 332.33 | 8.9020 / 326.91 | 库对照 |
| rocblas_mm_out | 4.4311 / 328.38 | 6.6462 / 328.40 | 2.1865 / 332.74 | 8.8759 / 327.87 | 库对照 |
| [V121](GEMM_源码与构建索引.md#v121) | 6.0878 / 239.02 | 9.1037 / 239.75 | 3.2704 / 222.46 | 12.1364 / 239.79 | 通过 |
| [V133](GEMM_源码与构建索引.md#v133) | 6.1066 / 238.28 | 9.1333 / 238.97 | 3.2806 / 221.77 | 12.1761 / 239.01 | 通过 |
| [V134](GEMM_源码与构建索引.md#v134) | 6.0931 / 238.81 | 9.1116 / 239.54 | 3.2728 / 222.30 | 12.1476 / 239.57 | 通过 |
| [V135](GEMM_源码与构建索引.md#v135) | 6.0831 / 239.20 | 9.0983 / 239.89 | 3.2688 / 222.57 | 12.1296 / 239.92 | 通过 |
| [V136](GEMM_源码与构建索引.md#v136) | 6.7288 / 216.25 | 9.9938 / 218.40 | 3.5701 / 203.79 | 13.3177 / 218.52 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V121](GEMM_源码与构建索引.md#v121) `rolephase_m1_bank_asmb_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V133](GEMM_源码与构建索引.md#v133) `rolephase_m1_bank_asmb_lpad1_reg768`：本索引首次出现该哈希。
- [V134](GEMM_源码与构建索引.md#v134) `rolephase_m1_bank_asmb_lpad2_reg768`：本索引首次出现该哈希。
- [V135](GEMM_源码与构建索引.md#v135) `rolephase_m1_bank_asmb_lpad3_reg768`：本索引首次出现该哈希。
- [V136](GEMM_源码与构建索引.md#v136) `rolephase_m1_bank_asmb_dsasm_lpad1_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_ao_phase_vgpr_alignment/results.json>)；[environment.json](<../results/stage_ao_phase_vgpr_alignment/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-044"></a>
## 044 / AP：DS 中间插入未来 B 预取

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_ap_mixed_prefetch_and_structured_buffer`；时间：2026-10-13T11:40:33.611278+08:00。

**改了什么及结论：** 每相位16次A LDS读拆成8次DS、future B、8次DS；跨wave同步和数学不变。FC2约246.6T，上半wave才交错及结构化cache对照不胜出。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§17](<../LDS_WRAP_AND_PIPELINE.md:216>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4804 / 324.76 | 6.6725 / 327.11 | 2.1889 / 332.37 | 8.9015 / 326.93 | 库对照 |
| rocblas_mm_out | 4.4512 / 326.90 | 6.6636 / 327.55 | 2.1834 / 333.21 | 8.8820 / 327.65 | 库对照 |
| [V121](GEMM_源码与构建索引.md#v121) | 6.0867 / 239.06 | 9.1024 / 239.79 | 3.2699 / 222.50 | 12.1371 / 239.78 | 通过 |
| [V137](GEMM_源码与构建索引.md#v137) | 5.9007 / 246.59 | 8.8488 / 246.66 | 3.1809 / 228.72 | 11.7948 / 246.73 | 通过 |
| [V138](GEMM_源码与构建索引.md#v138) | 6.2747 / 231.90 | 9.3869 / 232.52 | 3.3628 / 216.35 | 12.5108 / 232.61 | 通过 |
| [V139](GEMM_源码与构建索引.md#v139) | 6.2002 / 234.68 | 9.2553 / 235.82 | 3.3200 / 219.14 | 12.3378 / 235.87 | 通过 |
| [V140](GEMM_源码与构建索引.md#v140) | 7.3035 / 199.23 | 10.8071 / 201.96 | 3.7063 / 196.30 | 14.4131 / 201.91 | 通过 |
| [V141](GEMM_源码与构建索引.md#v141) | 7.3953 / 196.76 | 10.9319 / 199.66 | 3.7270 / 195.21 | 14.5946 / 199.40 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V121](GEMM_源码与构建索引.md#v121) `rolephase_m1_bank_asmb_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V137](GEMM_源码与构建索引.md#v137) `rolephase_m1_bank_asmb_mid_reg768`：本索引首次出现该哈希。
- [V138](GEMM_源码与构建索引.md#v138) `rolephase_m1_bank_asmb_midgroup_reg768`：本索引首次出现该哈希。
- [V139](GEMM_源码与构建索引.md#v139) `rolephase_m1_bank_asmb_cachea_reg768`：本索引首次出现该哈希。
- [V140](GEMM_源码与构建索引.md#v140) `rolephase_m1_bank_asmb_cacheb_reg768`：本索引首次出现该哈希。
- [V141](GEMM_源码与构建索引.md#v141) `rolephase_m1_bank_asmb_cacheab_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_ap_mixed_prefetch_and_structured_buffer/results.json>)；[environment.json](<../results/stage_ap_mixed_prefetch_and_structured_buffer/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-045"></a>
## 045 / AQ：移除热循环 terminal 预取分支

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_aq_branchfree_prefetch_and_unroll`；时间：2026-10-13T11:43:47.446804+08:00。

**改了什么及结论：** 每相位都发future，包括末尾无用预取；用buffer边界保护，保持pending bank活跃直到vmcnt(0)再写回。unroll2与always同SHA，不能当两次独立改进。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§17](<../LDS_WRAP_AND_PIPELINE.md:216>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4427 / 327.52 | 6.6390 / 328.76 | 2.1859 / 332.83 | 8.9034 / 326.86 | 库对照 |
| rocblas_mm_out | 4.4443 / 327.41 | 6.6314 / 329.14 | 2.1832 / 333.25 | 8.8944 / 327.19 | 库对照 |
| [V121](GEMM_源码与构建索引.md#v121) | 6.0828 / 239.21 | 9.1022 / 239.79 | 3.2696 / 222.52 | 12.1341 / 239.83 | 通过 |
| [V142](GEMM_源码与构建索引.md#v142) | 5.9847 / 243.14 | 8.9409 / 244.12 | 3.2086 / 226.75 | 11.9187 / 244.17 | 通过 |
| [V143](GEMM_源码与构建索引.md#v143) | 7.1698 / 202.95 | 10.6202 / 205.52 | 3.7098 / 196.11 | 14.0571 / 207.03 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V121](GEMM_源码与构建索引.md#v121) `rolephase_m1_bank_asmb_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V142](GEMM_源码与构建索引.md#v142) `rolephase_m1_bank_asmb_always_reg768`：本索引首次出现该哈希。
- [V143](GEMM_源码与构建索引.md#v143) `rolephase_m1_bank_asmb_always_cacheb_reg768`：本索引首次出现该哈希。
- [V142](GEMM_源码与构建索引.md#v142) `rolephase_m1_bank_asmb_always_unroll2_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

本轮同SHA去重：`rolephase_m1_bank_asmb_always_unroll2_reg768` → `rolephase_m1_bank_asmb_always_reg768`。

原始证据：[原始 results.json](<../results/stage_aq_branchfree_prefetch_and_unroll/results.json>)；[environment.json](<../results/stage_aq_branchfree_prefetch_and_unroll/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-046"></a>
## 046 / AR：交错预取重建与独立复测

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_ar_mixed_prefetch_rebuild_verify`；时间：2026-10-13T11:46:48.137402+08:00。

**改了什么及结论：** 重建AP的mixed路径，核对新库SHA并独立72组/216次审计。约229–247T，说明主线收益可以复现；不把旧hash正确性证据套到新库。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)；[stress_phase.py](<../stress_phase.py>)。

历史算法/同步/正确性细节：[原记录§17](<../LDS_WRAP_AND_PIPELINE.md:216>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4459 / 327.29 | 6.6568 / 327.88 | 2.1870 / 332.67 | 8.8813 / 327.67 | 库对照 |
| rocblas_mm_out | 4.4335 / 328.20 | 6.6590 / 327.77 | 2.1847 / 333.02 | 8.8932 / 327.24 | 库对照 |
| [V121](GEMM_源码与构建索引.md#v121) | 6.0882 / 239.00 | 9.1017 / 239.80 | 3.2700 / 222.49 | 12.1342 / 239.83 | 通过 |
| [V144](GEMM_源码与构建索引.md#v144) | 5.9059 / 246.38 | 8.8580 / 246.40 | 3.1831 / 228.56 | 11.8057 / 246.51 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V121](GEMM_源码与构建索引.md#v121) `rolephase_m1_bank_asmb_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V144](GEMM_源码与构建索引.md#v144) `rolephase_m1_bank_asmb_mid_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_ar_mixed_prefetch_rebuild_verify/results.json>)；[environment.json](<../results/stage_ar_mixed_prefetch_rebuild_verify/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-047"></a>
## 047 / AS：mixed + always 组合

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_as_combined_mixed_branchfree`；时间：2026-10-13T11:51:42.966257+08:00。

**改了什么及结论：** 把中间预取与无条件future组合，末尾完整drain保留；另测progressive DS等待。组合更快，额外progressive回退，不把两项单独百分比相加。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§17](<../LDS_WRAP_AND_PIPELINE.md:216>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4347 / 328.11 | 6.6593 / 327.76 | 2.1890 / 332.36 | 8.9080 / 326.69 | 库对照 |
| rocblas_mm_out | 4.4222 / 329.04 | 6.6429 / 328.57 | 2.1855 / 332.90 | 8.8954 / 327.16 | 库对照 |
| [V121](GEMM_源码与构建索引.md#v121) | 6.0873 / 239.04 | 9.1020 / 239.80 | 3.2706 / 222.45 | 12.1386 / 239.74 | 通过 |
| [V144](GEMM_源码与构建索引.md#v144) | 5.9075 / 246.31 | 8.8586 / 246.39 | 3.1843 / 228.48 | 11.8076 / 246.47 | 通过 |
| [V142](GEMM_源码与构建索引.md#v142) | 5.9856 / 243.10 | 8.9421 / 244.09 | 3.2094 / 226.69 | 11.9221 / 244.10 | 通过 |
| [V145](GEMM_源码与构建索引.md#v145) | 5.8611 / 248.26 | 8.7534 / 249.35 | 3.1419 / 231.56 | 11.6612 / 249.56 | 通过 |
| [V146](GEMM_源码与构建索引.md#v146) | 6.3671 / 228.53 | 9.4843 / 230.13 | 3.3857 / 214.89 | 12.6225 / 230.56 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V121](GEMM_源码与构建索引.md#v121) `rolephase_m1_bank_asmb_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V144](GEMM_源码与构建索引.md#v144) `rolephase_m1_bank_asmb_mid_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V142](GEMM_源码与构建索引.md#v142) `rolephase_m1_bank_asmb_always_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V145](GEMM_源码与构建索引.md#v145) `rolephase_m1_bank_asmb_always_mid_reg768`：本索引首次出现该哈希。
- [V146](GEMM_源码与构建索引.md#v146) `rolephase_m1_bank_asmb_always_mid_dsprogress_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_as_combined_mixed_branchfree/results.json>)；[environment.json](<../results/stage_as_combined_mixed_branchfree/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-048"></a>
## 048 / AT：组合流水独立复测

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_at_combined_prefetch_independent`；时间：2026-10-13T11:54:12.621712+08:00。

**改了什么及结论：** 独立确认AS库和72组/216次审计，约232–250T；与同轮旧phase/交错路径比较。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)；[stress_phase.py](<../stress_phase.py>)。

历史算法/同步/正确性细节：[原记录§17](<../LDS_WRAP_AND_PIPELINE.md:216>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4182 / 329.34 | 6.6442 / 328.50 | 2.1899 / 332.22 | 8.9108 / 326.59 | 库对照 |
| rocblas_mm_out | 4.4259 / 328.77 | 6.6406 / 328.68 | 2.1857 / 332.87 | 8.8970 / 327.10 | 库对照 |
| [V121](GEMM_源码与构建索引.md#v121) | 6.0893 / 238.96 | 9.1041 / 239.74 | 3.2711 / 222.41 | 12.1356 / 239.80 | 通过 |
| [V144](GEMM_源码与构建索引.md#v144) | 5.9083 / 246.28 | 8.8583 / 246.39 | 3.1844 / 228.47 | 11.8091 / 246.43 | 通过 |
| [V145](GEMM_源码与构建索引.md#v145) | 5.8634 / 248.17 | 8.7523 / 249.38 | 3.1425 / 231.52 | 11.6616 / 249.55 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V121](GEMM_源码与构建索引.md#v121) `rolephase_m1_bank_asmb_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V144](GEMM_源码与构建索引.md#v144) `rolephase_m1_bank_asmb_mid_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V145](GEMM_源码与构建索引.md#v145) `rolephase_m1_bank_asmb_always_mid_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

原始证据：[原始 results.json](<../results/stage_at_combined_prefetch_independent/results.json>)；[environment.json](<../results/stage_at_combined_prefetch_independent/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-049"></a>
## 049 / AU：有界 buffer 输出

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_au_bounded_buffer_output`；时间：2026-10-13T12:03:34.475898+08:00。

**改了什么及结论：** epilogue采用buffer_store_short，把全部token偏移放可参与bounds check的voffset，soffset固定0；保持CK BF16 RNE。正确处理M尾行，约244–258T；早期不安全soffset版本未执行GPU。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§18](<../LDS_WRAP_AND_PIPELINE.md:226>)；[原记录§19](<../LDS_WRAP_AND_PIPELINE.md:243>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4389 / 327.80 | 6.6634 / 327.56 | 2.2020 / 330.40 | 8.9058 / 326.77 | 库对照 |
| rocblas_mm_out | 4.4276 / 328.64 | 6.6381 / 328.80 | 2.1888 / 332.39 | 8.8906 / 327.33 | 库对照 |
| [V121](GEMM_源码与构建索引.md#v121) | 6.0902 / 238.92 | 9.1041 / 239.74 | 3.2725 / 222.32 | 12.1376 / 239.77 | 通过 |
| [V145](GEMM_源码与构建索引.md#v145) | 5.8624 / 248.21 | 8.7571 / 249.24 | 3.1494 / 231.01 | 11.6683 / 249.41 | 通过 |
| [V147](GEMM_源码与构建索引.md#v147) | 5.9442 / 244.79 | 8.8164 / 247.57 | 3.1191 / 233.25 | 11.7520 / 247.63 | 通过 |
| [V148](GEMM_源码与构建索引.md#v148) | 5.7721 / 252.09 | 8.5809 / 254.36 | 3.0374 / 239.53 | 11.4363 / 254.47 | 通过 |
| [V149](GEMM_源码与构建索引.md#v149) | 5.6942 / 255.54 | 8.4492 / 258.33 | 2.9821 / 243.97 | 11.2637 / 258.37 | 通过 |

**xprof / DTK 表现：** 相对AK，工具报告SALU52.18M→29.61M、VALU258.28M→238.41M；LDS issue却156.07M→173.70M。AU更快，不代表所有等待都下降。 证据P02：[XPROF_ANALYSIS.md](<../XPROF_ANALYSIS.md>)；[results/xprof_au_metrics_20261013/analysis.json](<../results/xprof_au_metrics_20261013/analysis.json>)。

**候选对应关系与配置：**

- [V121](GEMM_源码与构建索引.md#v121) `rolephase_m1_bank_asmb_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V145](GEMM_源码与构建索引.md#v145) `rolephase_m1_bank_asmb_always_mid_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V147](GEMM_源码与构建索引.md#v147) `rolephase_m1_bank_asmb_bufferepi_reg768`：本索引首次出现该哈希。
- [V148](GEMM_源码与构建索引.md#v148) `rolephase_m1_bank_asmb_mid_bufferepi_reg768`：本索引首次出现该哈希。
- [V149](GEMM_源码与构建索引.md#v149) `rolephase_m1_bank_asmb_always_mid_bufferepi_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_au_bounded_buffer_output/results.json>)；[environment.json](<../results/stage_au_bounded_buffer_output/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-050"></a>
## 050 / AV：xprof 指导的预取点4/12

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_av_xprof_guided_prefetch_points`；时间：2026-10-13T12:26:47.244867+08:00。

**改了什么及结论：** 根据AU LDS issue等待较高，测试future B放在第4/12次DS后及wave组4/12错开，控制为点8。各shape均回退，保留点8，counter方向不能直接推出收益。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§19](<../LDS_WRAP_AND_PIPELINE.md:243>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4643 / 325.94 | 6.6612 / 327.66 | 2.1914 / 331.99 | 8.8949 / 327.17 | 库对照 |
| rocblas_mm_out | 4.4589 / 326.33 | 6.6459 / 328.42 | 2.1849 / 332.99 | 8.8963 / 327.12 | 库对照 |
| [V149](GEMM_源码与构建索引.md#v149) | 5.6960 / 255.46 | 8.4532 / 258.20 | 2.9829 / 243.91 | 11.2780 / 258.04 | 通过 |
| [V150](GEMM_源码与构建索引.md#v150) | 5.8901 / 247.04 | 8.7248 / 250.16 | 3.0592 / 237.82 | 11.6293 / 250.24 | 通过 |
| [V151](GEMM_源码与构建索引.md#v151) | 5.8925 / 246.94 | 8.7143 / 250.47 | 3.0715 / 236.87 | 11.6131 / 250.59 | 通过 |
| [V152](GEMM_源码与构建索引.md#v152) | 6.0374 / 241.01 | 8.9413 / 244.11 | 3.1315 / 232.33 | 11.9143 / 244.26 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V149](GEMM_源码与构建索引.md#v149) `rolephase_m1_bank_asmb_always_mid_bufferepi_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V150](GEMM_源码与构建索引.md#v150) `rolephase_m1_bank_asmb_always_mid4_bufferepi_reg768`：本索引首次出现该哈希。
- [V151](GEMM_源码与构建索引.md#v151) `rolephase_m1_bank_asmb_always_mid12_bufferepi_reg768`：本索引首次出现该哈希。
- [V152](GEMM_源码与构建索引.md#v152) `rolephase_m1_bank_asmb_always_midsplit_bufferepi_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_av_xprof_guided_prefetch_points/results.json>)；[environment.json](<../results/stage_av_xprof_guided_prefetch_points/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-051"></a>
## 051 / AW：限制跨相位调度与 math burst

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_aw_mmac_burst_boundaries`；时间：2026-10-13T12:36:36.933198+08:00。

**改了什么及结论：** 添加math末尾编译fence、LGKM归零burst及首条MMAC后priority组合。资源不变、同步未删，全部回退；不能直接复制原生某条指令就认定加速。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§20](<../LDS_WRAP_AND_PIPELINE.md:249>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4179 / 329.36 | 6.6543 / 328.00 | 2.1918 / 331.93 | 8.8888 / 327.40 | 库对照 |
| rocblas_mm_out | 4.4185 / 329.31 | 6.6586 / 327.79 | 2.1880 / 332.51 | 8.8879 / 327.43 | 库对照 |
| [V149](GEMM_源码与构建索引.md#v149) | 5.6957 / 255.47 | 8.4425 / 258.53 | 2.9821 / 243.97 | 11.2758 / 258.09 | 通过 |
| [V153](GEMM_源码与构建索引.md#v153) | 5.7965 / 251.03 | 8.5824 / 254.31 | 3.0290 / 240.19 | 11.4396 / 254.40 | 通过 |
| [V154](GEMM_源码与构建索引.md#v154) | 6.0606 / 240.09 | 8.9618 / 243.55 | 3.1576 / 230.41 | 11.9426 / 243.68 | 通过 |
| [V155](GEMM_源码与构建索引.md#v155) | 6.2300 / 233.56 | 9.2201 / 236.73 | 3.2211 / 225.87 | 12.2900 / 236.79 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V149](GEMM_源码与构建索引.md#v149) `rolephase_m1_bank_asmb_always_mid_bufferepi_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V153](GEMM_源码与构建索引.md#v153) `rolephase_m1_bank_asmb_always_mid_bufferepi_mathfence_reg768`：本索引首次出现该哈希。
- [V154](GEMM_源码与构建索引.md#v154) `rolephase_m1_bank_asmb_always_mid_bufferepi_burst_reg768`：本索引首次出现该哈希。
- [V155](GEMM_源码与构建索引.md#v155) `rolephase_m1_bank_asmb_always_mid_bufferepi_burst_postprio_reg768`：本索引首次出现该哈希。
- [V149](GEMM_源码与构建索引.md#v149) `rolephase_m1_bank_asmb_always_mid_bufferepi_compat_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

本轮同SHA去重：`rolephase_m1_bank_asmb_always_mid_bufferepi_compat_reg768` → `rolephase_m1_bank_asmb_always_mid_bufferepi_reg768`。

原始证据：[原始 results.json](<../results/stage_aw_mmac_burst_boundaries/results.json>)；[environment.json](<../results/stage_aw_mmac_burst_boundaries/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-052"></a>
## 052 / AX：完整 K64 激活双 bank

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_ax_k64_activation_banks`；时间：2026-10-13T12:41:58.021885+08:00。

**改了什么及结论：** 新增独立compute路径，按phase0/1预取整K64 B，保留odd tail和所有完成等待。phase0出现2VGPR spill/12B scratch，phase1无spill仍慢，不能把退化全部归于spill。

**源码位置：** [phase_k64_body.hpp](<../phase_k64_body.hpp>)；[rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§21](<../LDS_WRAP_AND_PIPELINE.md:262>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4432 / 327.48 | 6.6445 / 328.49 | 2.1898 / 332.24 | 8.9137 / 326.48 | 库对照 |
| rocblas_mm_out | 4.4526 / 326.79 | 6.6466 / 328.39 | 2.1865 / 332.75 | 8.9031 / 326.87 | 库对照 |
| [V149](GEMM_源码与构建索引.md#v149) | 5.6966 / 255.43 | 8.4486 / 258.34 | 2.9829 / 243.91 | 11.2763 / 258.08 | 通过 |
| [V156](GEMM_源码与构建索引.md#v156) | 6.3805 / 228.05 | 9.4446 / 231.10 | 3.3300 / 218.48 | 12.5626 / 231.65 | 通过 |
| [V157](GEMM_源码与构建索引.md#v157) | 6.6157 / 219.94 | 9.8818 / 220.87 | 3.3365 / 218.05 | 13.2545 / 219.56 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V149](GEMM_源码与构建索引.md#v149) `rolephase_m1_bank_asmb_always_mid_bufferepi_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V156](GEMM_源码与构建索引.md#v156) `rolephase_m1_bank_asmb_always_mid_bufferepi_b64p0_reg768`：本索引首次出现该哈希。
- [V157](GEMM_源码与构建索引.md#v157) `rolephase_m1_bank_asmb_always_mid_bufferepi_b64p1_reg768`：本索引首次出现该哈希。
- [V149](GEMM_源码与构建索引.md#v149) `rolephase_m1_bank_asmb_always_mid_bufferepi_compat_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

本轮同SHA去重：`rolephase_m1_bank_asmb_always_mid_bufferepi_compat_reg768` → `rolephase_m1_bank_asmb_always_mid_bufferepi_reg768`。

原始证据：[原始 results.json](<../results/stage_ax_k64_activation_banks/results.json>)；[environment.json](<../results/stage_ax_k64_activation_banks/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-053"></a>
## 053 / AY：branchless BF16 RNE

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_ay_branchless_bf16_output`；时间：2026-10-13T12:45:55.111839+08:00。

**改了什么及结论：** 整数mask/select替代条件控制；有限值仍执行ties-to-even，Inf/NaN保留CK payload/sign语义。1507328位模式probe通过，两种表达式同SHA；减少输出控制分支而不减少数学。

**源码位置：** [output_rne.hpp](<../output_rne.hpp>)；[rolephase.cpp](<../rolephase.cpp>)；[rne_probe.cpp](<../rne_probe.cpp>)。

历史算法/同步/正确性细节：[原记录§22](<../LDS_WRAP_AND_PIPELINE.md:276>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4643 / 325.94 | 6.6481 / 328.31 | 2.1904 / 332.16 | 8.8628 / 328.36 | 库对照 |
| rocblas_mm_out | 4.4482 / 327.12 | 6.6432 / 328.55 | 2.1890 / 332.36 | 8.8792 / 327.75 | 库对照 |
| [V149](GEMM_源码与构建索引.md#v149) | 5.6960 / 255.46 | 8.4515 / 258.26 | 2.9825 / 243.94 | 11.2679 / 258.27 | 通过 |
| [V158](GEMM_源码与构建索引.md#v158) | 5.6431 / 257.85 | 8.3931 / 260.05 | 2.9359 / 247.81 | 11.1578 / 260.82 | 通过 |

**xprof / DTK 表现：** 作为cadence的旧RNE对照；独立RNE位模式/ISA变化与profile不是同一种证据。 证据P03：[results/cadence_counter_analysis.json](<../results/cadence_counter_analysis.json>)；[LDS_WRAP_AND_PIPELINE.md](<../LDS_WRAP_AND_PIPELINE.md>)。

**候选对应关系与配置：**

- [V149](GEMM_源码与构建索引.md#v149) `rolephase_m1_bank_asmb_always_mid_bufferepi_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V158](GEMM_源码与构建索引.md#v158) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_reg768`：本索引首次出现该哈希。
- [V158](GEMM_源码与构建索引.md#v158) `rolephase_m1_bank_asmb_always_mid_bufferepi_selectrne_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V149](GEMM_源码与构建索引.md#v149) `rolephase_m1_bank_asmb_always_mid_bufferepi_compat_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

本轮同SHA去重：`rolephase_m1_bank_asmb_always_mid_bufferepi_selectrne_reg768` → `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_reg768`；`rolephase_m1_bank_asmb_always_mid_bufferepi_compat_reg768` → `rolephase_m1_bank_asmb_always_mid_bufferepi_reg768`。

原始证据：[原始 results.json](<../results/stage_ay_branchless_bf16_output/results.json>)；[environment.json](<../results/stage_ay_branchless_bf16_output/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-054"></a>
## 054 / AZ：RNE 独立验证

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_az_branchless_rne_independent`；时间：2026-10-13T12:48:31.417446+08:00。

**改了什么及结论：** 独立72组/216次GEMM验证并重复计时，较AU耗时降低约0.7%–1.6%，达到248–261T。不能将浮点随机误差通过泛化为与rocBLAS任意输入逐bit一致。

**源码位置：** [output_rne.hpp](<../output_rne.hpp>)；[rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§22](<../LDS_WRAP_AND_PIPELINE.md:276>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4593 / 326.30 | 6.6621 / 327.62 | 2.1872 / 332.64 | 8.8806 / 327.70 | 库对照 |
| rocblas_mm_out | 4.4413 / 327.62 | 6.6333 / 329.04 | 2.1838 / 333.16 | 8.8900 / 327.35 | 库对照 |
| [V149](GEMM_源码与构建索引.md#v149) | 5.6943 / 255.53 | 8.4460 / 258.42 | 2.9827 / 243.92 | 11.2722 / 258.17 | 通过 |
| [V158](GEMM_源码与构建索引.md#v158) | 5.6423 / 257.89 | 8.3862 / 260.27 | 2.9356 / 247.83 | 11.1640 / 260.68 | 通过 |

**xprof / DTK 表现：** 旧RNE对照参与后续cadence采集，不是每次独立性能复测都重新采xprof。 证据P03：[results/cadence_counter_analysis.json](<../results/cadence_counter_analysis.json>)；[LDS_WRAP_AND_PIPELINE.md](<../LDS_WRAP_AND_PIPELINE.md>)。

**候选对应关系与配置：**

- [V149](GEMM_源码与构建索引.md#v149) `rolephase_m1_bank_asmb_always_mid_bufferepi_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V158](GEMM_源码与构建索引.md#v158) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

原始证据：[原始 results.json](<../results/stage_az_branchless_rne_independent/results.json>)；[environment.json](<../results/stage_az_branchless_rne_independent/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-055"></a>
## 055 / BA：producer/consumer cadence

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_ba_producer_consumer_cadence`；时间：2026-10-13T12:59:23.505911+08:00。

**改了什么及结论：** 由初装A0/A1改为startup只装A0，producer在当前tile开始预取另一槽A(tile+1)，覆盖当前两个K32数学相位。每K64两个匹配barrier、startup与terminal等待保持，保护读完再复用。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§23](<../LDS_WRAP_AND_PIPELINE.md:297>)；[原记录§24](<../LDS_WRAP_AND_PIPELINE.md:303>)；[原记录§25](<../LDS_WRAP_AND_PIPELINE.md:324>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4121 / 329.79 | 6.6611 / 327.67 | 2.1868 / 332.70 | 8.9028 / 326.88 | 库对照 |
| rocblas_mm_out | 4.4279 / 328.62 | 6.6582 / 327.81 | 2.1833 / 333.23 | 8.8989 / 327.03 | 库对照 |
| [V158](GEMM_源码与构建索引.md#v158) | 5.6417 / 257.92 | 8.3885 / 260.19 | 2.9354 / 247.85 | 11.1711 / 260.51 | 通过 |
| [V159](GEMM_源码与构建索引.md#v159) | 5.3599 / 271.48 | 7.9611 / 274.16 | 2.8113 / 258.79 | 10.6519 / 273.21 | 通过 |
| [V160](GEMM_源码与构建索引.md#v160) | 5.3895 / 269.99 | 7.9987 / 272.87 | 2.8239 / 257.63 | 10.6990 / 272.00 | 通过 |

**xprof / DTK 表现：** LGKM131.35M→50.66M，barrier600.11M→460.05M；LDS issue173.17M→276.02M上升。同步节奏改善而非单一等待全部下降。 证据P03：[results/cadence_counter_analysis.json](<../results/cadence_counter_analysis.json>)；[LDS_WRAP_AND_PIPELINE.md](<../LDS_WRAP_AND_PIPELINE.md>)。

**候选对应关系与配置：**

- [V158](GEMM_源码与构建索引.md#v158) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V159](GEMM_源码与构建索引.md#v159) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_reg768`：本索引首次出现该哈希。
- [V160](GEMM_源码与构建索引.md#v160) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_postprio_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_ba_producer_consumer_cadence/results.json>)；[environment.json](<../results/stage_ba_producer_consumer_cadence/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-056"></a>
## 056 / BB：cadence 独立复测

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_bb_cadence_independent`；时间：2026-10-13T13:02:29.811632+08:00。

**改了什么及结论：** 独立72组广审计和计时确认约259–274T，相对RNE降低4.2%–5.2%。资源未变；counter体现barrier/LGKM下降但LDS/VMEM issue上升。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§24](<../LDS_WRAP_AND_PIPELINE.md:303>)；[原记录§25](<../LDS_WRAP_AND_PIPELINE.md:324>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4330 / 328.24 | 6.6586 / 327.79 | 2.1839 / 333.14 | 8.9002 / 326.98 | 库对照 |
| rocblas_mm_out | 4.4273 / 328.66 | 6.6618 / 327.64 | 2.1835 / 333.20 | 8.8861 / 327.50 | 库对照 |
| [V158](GEMM_源码与构建索引.md#v158) | 5.6427 / 257.87 | 8.3948 / 260.00 | 2.9353 / 247.86 | 11.1623 / 260.71 | 通过 |
| [V159](GEMM_源码与构建索引.md#v159) | 5.3599 / 271.48 | 7.9607 / 274.17 | 2.8112 / 258.80 | 10.6538 / 273.16 | 通过 |

**xprof / DTK 表现：** 对应BA同库cadence counter；性能采用本阶段无profiler复测，counter来自单独采集。 证据P03：[results/cadence_counter_analysis.json](<../results/cadence_counter_analysis.json>)；[LDS_WRAP_AND_PIPELINE.md](<../LDS_WRAP_AND_PIPELINE.md>)。

**候选对应关系与配置：**

- [V158](GEMM_源码与构建索引.md#v158) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V159](GEMM_源码与构建索引.md#v159) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V158](GEMM_源码与构建索引.md#v158) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_compat_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

本轮同SHA去重：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_compat_reg768` → `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_reg768`。

原始证据：[原始 results.json](<../results/stage_bb_cadence_independent/results.json>)；[environment.json](<../results/stage_bb_cadence_independent/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-057"></a>
## 057 / BC：pow2 stride + row remainder

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_bc_native_pow2_b`；时间：2026-10-13T13:27:26.059303+08:00。

**改了什么及结论：** 结构化B stride取K字节跨度的下取2次幂，余数留voffset；record count用真实行数，terminal强制越界。比较bias0/16及raw末尾VM0对照，探针正确但GEMM仍慢。

**源码位置：** [structured_b.hpp](<../structured_b.hpp>)；[structured_b_probe.cpp](<../structured_b_probe.cpp>)；[rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§26](<../LDS_WRAP_AND_PIPELINE.md:342>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4562 / 326.53 | 6.6608 / 327.68 | 2.1908 / 332.08 | 8.8624 / 328.37 | 库对照 |
| rocblas_mm_out | 4.4397 / 327.74 | 6.6339 / 329.01 | 2.1870 / 332.67 | 8.8780 / 327.80 | 库对照 |
| [V159](GEMM_源码与构建索引.md#v159) | 5.3604 / 271.45 | 7.9622 / 274.12 | 2.8117 / 258.75 | 10.6528 / 273.18 | 通过 |
| [V161](GEMM_源码与构建索引.md#v161) | 5.3949 / 269.72 | 7.9837 / 273.39 | 2.8235 / 257.68 | 10.6847 / 272.37 | 通过 |
| [V162](GEMM_源码与构建索引.md#v162) | 5.5149 / 263.84 | 8.2461 / 264.69 | 2.8805 / 252.57 | 11.0156 / 264.19 | 通过 |
| [V163](GEMM_源码与构建索引.md#v163) | 5.5142 / 263.88 | 8.2421 / 264.82 | 2.8815 / 252.49 | 11.0150 / 264.20 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V159](GEMM_源码与构建索引.md#v159) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V161](GEMM_源码与构建索引.md#v161) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_finalwait_reg768`：本索引首次出现该哈希。
- [V162](GEMM_源码与构建索引.md#v162) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pow2b_reg768`：本索引首次出现该哈希。
- [V163](GEMM_源码与构建索引.md#v163) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pow2b_bias16_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_bc_native_pow2_b/results.json>)；[environment.json](<../results/stage_bc_native_pow2_b/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-058"></a>
## 058 / BD：A 标量 base / 显式 uniform wave

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_bd_scalar_addressing`；时间：2026-10-13T13:32:38.536186+08:00。

**改了什么及结论：** A每tile公共K偏移放resource base并等量缩bound；另用readfirstlane显式SGPR wave索引。A base几乎无收益，scalar-wave增VGPR并回退，不自动采用。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§27](<../LDS_WRAP_AND_PIPELINE.md:359>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4185 / 329.32 | 6.6560 / 327.92 | 2.1876 / 332.57 | 8.9165 / 326.38 | 库对照 |
| rocblas_mm_out | 4.4351 / 328.08 | 6.6490 / 328.26 | 2.1849 / 332.99 | 8.8802 / 327.72 | 库对照 |
| [V159](GEMM_源码与构建索引.md#v159) | 5.3623 / 271.36 | 7.9616 / 274.14 | 2.8119 / 258.74 | 10.6537 / 273.16 | 通过 |
| [V164](GEMM_源码与构建索引.md#v164) | 5.3578 / 271.59 | 7.9636 / 274.08 | 2.8108 / 258.84 | 10.6536 / 273.16 | 通过 |
| [V165](GEMM_源码与构建索引.md#v165) | 5.8094 / 250.47 | 8.6286 / 252.95 | 3.0450 / 238.93 | 11.5399 / 252.18 | 通过 |
| [V166](GEMM_源码与构建索引.md#v166) | 5.8001 / 250.87 | 8.6146 / 253.37 | 3.0409 / 239.25 | 11.5235 / 252.54 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V159](GEMM_源码与构建索引.md#v159) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V164](GEMM_源码与构建索引.md#v164) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_abase_reg768`：本索引首次出现该哈希。
- [V165](GEMM_源码与构建索引.md#v165) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_swave_reg768`：本索引首次出现该哈希。
- [V166](GEMM_源码与构建索引.md#v166) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_abase_swave_reg768`：本索引首次出现该哈希。
- [V159](GEMM_源码与构建索引.md#v159) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_compat_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

本轮同SHA去重：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_compat_reg768` → `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_reg768`。

原始证据：[原始 results.json](<../results/stage_bd_scalar_addressing/results.json>)；[environment.json](<../results/stage_bd_scalar_addressing/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-059"></a>
## 059 / BE：scalar-wave 上的 live-pad

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_be_scalar_wave_livepads`；时间：2026-10-13T13:40:23.232922+08:00。

**改了什么及结论：** live-pad1/2/3只移动首条MMAC的A起点，C/B不变。部分恢复BD退化但仍慢，不能称完整寄存器对齐实验。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§28](<../LDS_WRAP_AND_PIPELINE.md:372>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4378 / 327.88 | 6.6456 / 328.43 | 2.1902 / 332.19 | 8.8658 / 328.25 | 库对照 |
| rocblas_mm_out | 4.4233 / 328.96 | 6.6415 / 328.63 | 2.1847 / 333.01 | 8.8423 / 329.12 | 库对照 |
| [V159](GEMM_源码与构建索引.md#v159) | 5.3609 / 271.43 | 7.9616 / 274.15 | 2.8125 / 258.68 | 10.6532 / 273.17 | 通过 |
| [V165](GEMM_源码与构建索引.md#v165) | 5.8091 / 250.48 | 8.6285 / 252.96 | 3.0456 / 238.89 | 11.5403 / 252.17 | 通过 |
| [V167](GEMM_源码与构建索引.md#v167) | 5.7934 / 251.16 | 8.6070 / 253.59 | 3.0392 / 239.39 | 11.5110 / 252.82 | 通过 |
| [V168](GEMM_源码与构建索引.md#v168) | 5.7912 / 251.26 | 8.6011 / 253.76 | 3.0370 / 239.56 | 11.5020 / 253.02 | 通过 |
| [V169](GEMM_源码与构建索引.md#v169) | 5.7759 / 251.92 | 8.5769 / 254.48 | 3.0289 / 240.20 | 11.4706 / 253.71 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V159](GEMM_源码与构建索引.md#v159) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V165](GEMM_源码与构建索引.md#v165) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_swave_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V167](GEMM_源码与构建索引.md#v167) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_swave_lpad1_reg768`：本索引首次出现该哈希。
- [V168](GEMM_源码与构建索引.md#v168) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_swave_lpad2_reg768`：本索引首次出现该哈希。
- [V169](GEMM_源码与构建索引.md#v169) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_swave_lpad3_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_be_scalar_wave_livepads/results.json>)；[environment.json](<../results/stage_be_scalar_wave_livepads/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-060"></a>
## 060 / BF：wave 组 barrier 错位

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_bf_wave_group_barrier_stagger`；时间：2026-10-13T13:52:08.452333+08:00。

**改了什么及结论：** 一组数学后barrier，另一组读完A并LGKM0后先barrier再数学，让private VGPR保护已读数据；保持K顺序。动态分组版本回退，post-priority只部分恢复。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§29](<../LDS_WRAP_AND_PIPELINE.md:378>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4674 / 325.71 | 6.6608 / 327.68 | 2.1865 / 332.75 | 8.8983 / 327.05 | 库对照 |
| rocblas_mm_out | 4.4561 / 326.54 | 6.6568 / 327.88 | 2.1844 / 333.06 | 8.8527 / 328.73 | 库对照 |
| [V159](GEMM_源码与构建索引.md#v159) | 5.3610 / 271.42 | 7.9620 / 274.13 | 2.8122 / 258.71 | 10.6540 / 273.15 | 通过 |
| [V170](GEMM_源码与构建索引.md#v170) | 5.9810 / 243.29 | 8.7430 / 249.64 | 3.0198 / 240.92 | 11.6811 / 249.14 | 通过 |
| [V171](GEMM_源码与构建索引.md#v171) | 5.6879 / 255.82 | 8.2879 / 263.35 | 2.8326 / 256.85 | 11.0207 / 264.06 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V159](GEMM_源码与构建索引.md#v159) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V170](GEMM_源码与构建索引.md#v170) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_stagger_reg768`：本索引首次出现该哈希。
- [V171](GEMM_源码与构建索引.md#v171) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_stagger_postprio_reg768`：本索引首次出现该哈希。
- [V159](GEMM_源码与构建索引.md#v159) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_compat_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

本轮同SHA去重：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_compat_reg768` → `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_reg768`。

原始证据：[原始 results.json](<../results/stage_bf_wave_group_barrier_stagger/results.json>)；[environment.json](<../results/stage_bf_wave_group_barrier_stagger/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-061"></a>
## 061 / BG：显式双 K64 tile 展开

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_bg_explicit_pair_tiles`；时间：2026-10-13T14:07:08.379881+08:00。

**改了什么及结论：** consume_pair_tile用编译期slot0/slot1展开，odd尾单独slot0；B仍K32双bank。静态MMAC128→256只是循环展开，动态数学不翻倍。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§30](<../LDS_WRAP_AND_PIPELINE.md:390>)；[原记录§32](<../LDS_WRAP_AND_PIPELINE.md:426>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4717 / 325.40 | 6.6497 / 328.23 | 2.1843 / 333.07 | 8.9019 / 326.92 | 库对照 |
| rocblas_mm_out | 4.4574 / 326.44 | 6.6440 / 328.51 | 2.1840 / 333.13 | 8.8720 / 328.02 | 库对照 |
| [V159](GEMM_源码与构建索引.md#v159) | 5.3604 / 271.45 | 7.9615 / 274.15 | 2.8112 / 258.80 | 10.6544 / 273.14 | 通过 |
| [V172](GEMM_源码与构建索引.md#v172) | 5.3047 / 274.30 | 7.8330 / 278.64 | 2.7664 / 263.00 | 10.4807 / 277.67 | 通过 |
| [V173](GEMM_源码与构建索引.md#v173) | 5.8356 / 249.35 | 8.6412 / 252.59 | 2.9950 / 242.92 | 11.5242 / 252.53 | 通过 |

**xprof / DTK 表现：** SALU17.82M→13.71M、VALU238.42M→230.03M，动态LDS计数不变；LDS issue仍上升。 证据P04：[results/pair_tiles_analysis_20261013.json](<../results/pair_tiles_analysis_20261013.json>)；[LDS_WRAP_AND_PIPELINE.md](<../LDS_WRAP_AND_PIPELINE.md>)。

**候选对应关系与配置：**

- [V159](GEMM_源码与构建索引.md#v159) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V172](GEMM_源码与构建索引.md#v172) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_reg768`：本索引首次出现该哈希。
- [V173](GEMM_源码与构建索引.md#v173) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_stagger_reg768`：本索引首次出现该哈希。
- [V159](GEMM_源码与构建索引.md#v159) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_compat_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

本轮同SHA去重：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_compat_reg768` → `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_reg768`。

原始证据：[原始 results.json](<../results/stage_bg_explicit_pair_tiles/results.json>)；[environment.json](<../results/stage_bg_explicit_pair_tiles/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-062"></a>
## 062 / BH：双 tile 独立复测

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_bh_pair_tiles_independent`；时间：2026-10-13T14:14:06.467889+08:00。

**改了什么及结论：** 102组/306次增加奇数K64 tile尾部覆盖，约263–279T，相对cadence再降约1%–1.6%。固定LDS槽和减少控制指令有效，非减少数学工作。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)；[stress_phase.py](<../stress_phase.py>)。

历史算法/同步/正确性细节：[原记录§30](<../LDS_WRAP_AND_PIPELINE.md:390>)；[原记录§32](<../LDS_WRAP_AND_PIPELINE.md:426>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4486 / 327.09 | 6.6382 / 328.80 | 2.1898 / 332.25 | 8.8956 / 327.15 | 库对照 |
| rocblas_mm_out | 4.4449 / 327.36 | 6.6475 / 328.34 | 2.1868 / 332.70 | 8.8947 / 327.18 | 库对照 |
| [V159](GEMM_源码与构建索引.md#v159) | 5.3588 / 271.53 | 7.9627 / 274.11 | 2.8129 / 258.64 | 10.6570 / 273.08 | 通过 |
| [V172](GEMM_源码与构建索引.md#v172) | 5.3047 / 274.30 | 7.8341 / 278.60 | 2.7678 / 262.86 | 10.4817 / 277.64 | 通过 |

**xprof / DTK 表现：** 对应双tile库counter及完整SQTT，不能把静态MMAC翻倍解释成动态数学翻倍。 证据P04：[results/pair_tiles_analysis_20261013.json](<../results/pair_tiles_analysis_20261013.json>)；[LDS_WRAP_AND_PIPELINE.md](<../LDS_WRAP_AND_PIPELINE.md>)。

**候选对应关系与配置：**

- [V159](GEMM_源码与构建索引.md#v159) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V172](GEMM_源码与构建索引.md#v172) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

原始证据：[原始 results.json](<../results/stage_bh_pair_tiles_independent/results.json>)；[environment.json](<../results/stage_bh_pair_tiles_independent/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-063"></a>
## 063 / BI：A 搬运 m0 burst 初稿

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_bi_scalar_m0_burst`；时间：2026-10-13T14:18:55.813685+08:00。

**改了什么及结论：** 八次direct-to-LDS放入同一asm块，单次m0读取后标量递增0x1000；另配A base。计时退化，随后发现asm契约漏写m0/SCC clobber，不能仅凭数值绿灯接受。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)；[inspect_asm_contract.py](<../inspect_asm_contract.py>)。

历史算法/同步/正确性细节：[原记录§31](<../LDS_WRAP_AND_PIPELINE.md:409>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4179 / 329.36 | 6.6566 / 327.89 | 2.1868 / 332.70 | 8.8800 / 327.72 | 库对照 |
| rocblas_mm_out | 4.4158 / 329.52 | 6.6547 / 327.98 | 2.1850 / 332.98 | 8.8960 / 327.13 | 库对照 |
| [V172](GEMM_源码与构建索引.md#v172) | 5.3127 / 273.89 | 7.8341 / 278.61 | 2.7668 / 262.96 | 10.4791 / 277.71 | 通过 |
| [V174](GEMM_源码与构建索引.md#v174) | 5.3838 / 270.27 | 7.9957 / 272.98 | 2.8192 / 258.07 | 10.6962 / 272.08 | 通过 |
| [V175](GEMM_源码与构建索引.md#v175) | 5.4386 / 267.55 | 8.0774 / 270.21 | 2.8434 / 255.87 | 10.8018 / 269.41 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V172](GEMM_源码与构建索引.md#v172) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V174](GEMM_源码与构建索引.md#v174) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_aburst_reg768`：本索引首次出现该哈希。
- [V175](GEMM_源码与构建索引.md#v175) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_aburst_abase_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_bi_scalar_m0_burst/results.json>)；[environment.json](<../results/stage_bi_scalar_m0_burst/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-064"></a>
## 064 / BJ：补齐 m0/SCC clobber

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_bj_aburst_declared_clobbers`；时间：2026-10-13T14:26:41.429291+08:00。

**改了什么及结论：** 显式声明memory/m0/scc，检查LLVM IR，重新测同一burst。当前机器码与BI恰好相同但语义契约修复必要；四shape仍回退。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)；[inspect_asm_contract.py](<../inspect_asm_contract.py>)。

历史算法/同步/正确性细节：[原记录§31](<../LDS_WRAP_AND_PIPELINE.md:409>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4586 / 326.35 | 6.6807 / 326.71 | 2.1858 / 332.85 | 8.8985 / 327.04 | 库对照 |
| rocblas_mm_out | 4.4570 / 326.47 | 6.6787 / 326.81 | 2.1847 / 333.01 | 8.8770 / 327.83 | 库对照 |
| [V172](GEMM_源码与构建索引.md#v172) | 5.3046 / 274.31 | 7.8334 / 278.63 | 2.7676 / 262.88 | 10.4823 / 277.63 | 通过 |
| [V174](GEMM_源码与构建索引.md#v174) | 5.3848 / 270.22 | 7.9952 / 272.99 | 2.8200 / 257.99 | 10.6981 / 272.03 | 通过 |
| [V175](GEMM_源码与构建索引.md#v175) | 5.4390 / 267.53 | 8.0756 / 270.28 | 2.8439 / 255.83 | 10.8025 / 269.40 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V172](GEMM_源码与构建索引.md#v172) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V174](GEMM_源码与构建索引.md#v174) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_aburst_cc_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V175](GEMM_源码与构建索引.md#v175) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_aburst_abase_cc_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V172](GEMM_源码与构建索引.md#v172) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_compat_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

本轮同SHA去重：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_compat_reg768` → `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_reg768`。

原始证据：[原始 results.json](<../results/stage_bj_aburst_declared_clobbers/results.json>)；[environment.json](<../results/stage_bj_aburst_declared_clobbers/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-065"></a>
## 065 / BK：二维 grid 与 token group

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_bk_grid_and_token_groups`；时间：2026-10-13T14:57:56.493926+08:00。

**改了什么及结论：** 比较token group1/2/4/16及grid2d，展平grid后沿用同一tile映射。单FC2略有改善但FC1大幅回退，不全局采用。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§33](<../LDS_WRAP_AND_PIPELINE.md:442>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4296 / 328.49 | 6.6624 / 327.60 | 2.1914 / 332.00 | 8.8984 / 327.05 | 库对照 |
| rocblas_mm_out | 4.4327 / 328.26 | 6.6515 / 328.14 | 2.1856 / 332.88 | 8.8802 / 327.72 | 库对照 |
| [V172](GEMM_源码与构建索引.md#v172) | 5.3105 / 274.00 | 7.8341 / 278.61 | 2.7677 / 262.87 | 10.4802 / 277.68 | 通过 |
| [V176](GEMM_源码与构建索引.md#v176) | 5.3126 / 273.89 | 8.0145 / 272.34 | 2.7978 / 260.04 | 12.1563 / 239.40 | 通过 |
| [V177](GEMM_源码与构建索引.md#v177) | 5.3064 / 274.21 | 7.9776 / 273.59 | 2.7936 / 260.43 | 10.7050 / 271.85 | 通过 |
| [V178](GEMM_源码与构建索引.md#v178) | 5.3041 / 274.33 | 7.8933 / 276.52 | 2.7952 / 260.28 | 10.5771 / 275.14 | 通过 |
| [V179](GEMM_源码与构建索引.md#v179) | 5.3508 / 271.94 | 7.8595 / 277.71 | 2.7786 / 261.84 | 10.5247 / 276.51 | 通过 |
| [V180](GEMM_源码与构建索引.md#v180) | 5.2853 / 275.31 | 7.8496 / 278.06 | 2.7718 / 262.48 | 10.5026 / 277.09 | 通过 |
| [V181](GEMM_源码与构建索引.md#v181) | 5.2508 / 277.12 | 7.9619 / 274.13 | 2.7748 / 262.20 | 12.2348 / 237.86 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V172](GEMM_源码与构建索引.md#v172) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V176](GEMM_源码与构建索引.md#v176) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_g1_reg768`：本索引首次出现该哈希。
- [V177](GEMM_源码与构建索引.md#v177) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_g2_reg768`：本索引首次出现该哈希。
- [V178](GEMM_源码与构建索引.md#v178) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_g4_reg768`：本索引首次出现该哈希。
- [V179](GEMM_源码与构建索引.md#v179) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_g16_reg768`：本索引首次出现该哈希。
- [V180](GEMM_源码与构建索引.md#v180) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_grid2d_reg768`：本索引首次出现该哈希。
- [V181](GEMM_源码与构建索引.md#v181) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_grid2d_g1_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_bk_grid_and_token_groups/results.json>)；[environment.json](<../results/stage_bk_grid_and_token_groups/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-066"></a>
## 066 / BL：静态 wave 分组 + post-priority

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_bl_static_wave_groups`；时间：2026-10-13T15:09:52.146667+08:00。

**改了什么及结论：** 把完整消费循环按编译期stagger_tag生成两份，wave组选择移到K循环外；每相位首条MMAC后setprio1，结束复位。只有二者组合胜出，单独静态分组或数学前提priority仍慢。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§34](<../LDS_WRAP_AND_PIPELINE.md:448>)；[原记录§35](<../LDS_WRAP_AND_PIPELINE.md:467>)；[原记录§37](<../LDS_WRAP_AND_PIPELINE.md:494>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4312 / 328.38 | 6.6638 / 327.54 | 2.1924 / 331.85 | 8.8905 / 327.34 | 库对照 |
| rocblas_mm_out | 4.4286 / 328.57 | 6.6556 / 327.94 | 2.1868 / 332.70 | 8.8689 / 328.13 | 库对照 |
| [V172](GEMM_源码与构建索引.md#v172) | 5.3117 / 273.94 | 7.8357 / 278.55 | 2.7684 / 262.80 | 10.4835 / 277.60 | 通过 |
| [V173](GEMM_源码与构建索引.md#v173) | 5.8291 / 249.63 | 8.6359 / 252.74 | 2.9961 / 242.83 | 11.5581 / 251.79 | 通过 |
| [V182](GEMM_源码与构建索引.md#v182) | 5.5648 / 261.48 | 8.3192 / 262.36 | 2.8926 / 251.52 | 11.1471 / 261.07 | 通过 |
| [V183](GEMM_源码与构建索引.md#v183) | 4.9026 / 296.80 | 7.2529 / 300.93 | 2.5703 / 283.06 | 9.7084 / 299.76 | 通过 |
| [V184](GEMM_源码与构建索引.md#v184) | 5.6280 / 258.54 | 8.3413 / 261.66 | 2.8811 / 252.52 | 11.1398 / 261.24 | 通过 |

**xprof / DTK 表现：** 相对双tile，LDS issue286.79M→78.68M，barrier467.91M→333.81M；VM完成等待124.34M→200.99M上升。 证据P05：[LDS_WRAP_AND_PIPELINE.md](<../LDS_WRAP_AND_PIPELINE.md>)。

**候选对应关系与配置：**

- [V172](GEMM_源码与构建索引.md#v172) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V173](GEMM_源码与构建索引.md#v173) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_stagger_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V182](GEMM_源码与构建索引.md#v182) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_reg768`：本索引首次出现该哈希。
- [V183](GEMM_源码与构建索引.md#v183) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768`：本索引首次出现该哈希。
- [V184](GEMM_源码与构建索引.md#v184) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_priority_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_bl_static_wave_groups/results.json>)；[environment.json](<../results/stage_bl_static_wave_groups/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-067"></a>
## 067 / BM：静态分组独立复测

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_bm_static_wave_independent`；时间：2026-10-13T15:13:27.682603+08:00。

**改了什么及结论：** 102组/306次审计和独立计时确认约283–301T，耗时下降7.1%–7.5%。LDS issue/barrier下降，VM完成等待上升，不能把计数下降直接当时间收益。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§34](<../LDS_WRAP_AND_PIPELINE.md:448>)；[原记录§35](<../LDS_WRAP_AND_PIPELINE.md:467>)；[原记录§37](<../LDS_WRAP_AND_PIPELINE.md:494>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4577 / 326.42 | 6.6409 / 328.67 | 2.1922 / 331.88 | 8.8629 / 328.36 | 库对照 |
| rocblas_mm_out | 4.4534 / 326.74 | 6.6332 / 329.05 | 2.1905 / 332.13 | 8.8988 / 327.03 | 库对照 |
| [V172](GEMM_源码与构建索引.md#v172) | 5.3005 / 274.52 | 7.8318 / 278.69 | 2.7667 / 262.96 | 10.4792 / 277.71 | 通过 |
| [V183](GEMM_源码与构建索引.md#v183) | 4.9027 / 296.79 | 7.2502 / 301.05 | 2.5710 / 282.98 | 9.7076 / 299.78 | 通过 |

**xprof / DTK 表现：** 同库独立复测证明耗时下降约7.1%–7.5%；72.6%的LDS等待聚合下降不是kernel加速百分比。 证据P05：[LDS_WRAP_AND_PIPELINE.md](<../LDS_WRAP_AND_PIPELINE.md>)。

**候选对应关系与配置：**

- [V172](GEMM_源码与构建索引.md#v172) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V183](GEMM_源码与构建索引.md#v183) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

原始证据：[原始 results.json](<../results/stage_bm_static_wave_independent/results.json>)；[environment.json](<../results/stage_bm_static_wave_independent/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-068"></a>
## 068 / BN：新流水的 LGKM / DS / 预取对照

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_bn_static_wave_lds_schedule`；时间：2026-10-13T15:18:14.303977+08:00。

**改了什么及结论：** 比较builtin LGKM0、显式DS、渐进等待和预取点4/12。点4仅部分shape小幅改善，其他多数回退，没有独立确认的配置不替换主线。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§36](<../LDS_WRAP_AND_PIPELINE.md:481>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4576 / 326.43 | 6.6648 / 327.48 | 2.1968 / 331.18 | 8.8838 / 327.58 | 库对照 |
| rocblas_mm_out | 4.4605 / 326.21 | 6.6355 / 328.93 | 2.1880 / 332.52 | 8.8815 / 327.67 | 库对照 |
| [V183](GEMM_源码与构建索引.md#v183) | 4.9050 / 296.66 | 7.2537 / 300.90 | 2.5726 / 282.80 | 9.7090 / 299.74 | 通过 |
| [V185](GEMM_源码与构建索引.md#v185) | 5.3305 / 272.97 | 7.9917 / 273.11 | 2.7652 / 263.11 | 10.6966 / 272.07 | 通过 |
| [V186](GEMM_源码与构建索引.md#v186) | 5.7883 / 251.39 | 8.6329 / 252.83 | 3.0281 / 240.26 | 11.5185 / 252.65 | 通过 |
| [V187](GEMM_源码与构建索引.md#v187) | 5.0543 / 287.89 | 7.4236 / 294.01 | 2.6481 / 274.75 | 9.9363 / 292.88 | 通过 |
| [V188](GEMM_源码与构建索引.md#v188) | 4.9231 / 295.56 | 7.1555 / 305.03 | 2.5678 / 283.33 | 9.5765 / 303.89 | 通过 |
| [V189](GEMM_源码与构建索引.md#v189) | 5.3485 / 272.06 | 7.8785 / 277.04 | 2.7056 / 268.91 | 10.4653 / 278.08 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V183](GEMM_源码与构建索引.md#v183) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V185](GEMM_源码与构建索引.md#v185) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_builtinlgkm_reg768`：本索引首次出现该哈希。
- [V186](GEMM_源码与构建索引.md#v186) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_dsasm_reg768`：本索引首次出现该哈希。
- [V187](GEMM_源码与构建索引.md#v187) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_dsprogress_reg768`：本索引首次出现该哈希。
- [V188](GEMM_源码与构建索引.md#v188) `rolephase_m1_bank_asmb_always_mid4_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768`：本索引首次出现该哈希。
- [V189](GEMM_源码与构建索引.md#v189) `rolephase_m1_bank_asmb_always_mid12_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_bn_static_wave_lds_schedule/results.json>)；[environment.json](<../results/stage_bn_static_wave_lds_schedule/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-069"></a>
## 069 / BO：K32 B resource 标量 rebase

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_bo_static_wave_b_prefetch`；时间：2026-10-13T15:26:46.206564+08:00。

**改了什么及结论：** 公共K32偏移加入B指针并从byte bound等量扣除，voffset保留row*K及片内位置；尾部物理范围不变。比较pow2及预取点，raw rebase胜出。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§38](<../LDS_WRAP_AND_PIPELINE.md:505>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4325 / 328.28 | 6.6610 / 327.67 | 2.1901 / 332.20 | 8.8814 / 327.67 | 库对照 |
| rocblas_mm_out | 4.4296 / 328.49 | 6.6456 / 328.43 | 2.1843 / 333.09 | 8.8740 / 327.94 | 库对照 |
| [V183](GEMM_源码与构建索引.md#v183) | 4.9027 / 296.80 | 7.2556 / 300.82 | 2.5708 / 283.01 | 9.7092 / 299.73 | 通过 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8689 / 298.85 | 7.0889 / 307.89 | 2.5449 / 285.88 | 9.4964 / 306.45 | 通过 |
| [V191](GEMM_源码与构建索引.md#v191) | 5.0663 / 287.21 | 7.5125 / 290.53 | 2.5719 / 282.88 | 10.0525 / 289.50 | 通过 |
| [V192](GEMM_源码与构建索引.md#v192) | 4.8819 / 298.06 | 7.2095 / 302.74 | 2.5612 / 284.06 | 9.6448 / 301.74 | 通过 |
| [V193](GEMM_源码与构建索引.md#v193) | 4.9347 / 294.87 | 7.2303 / 301.87 | 2.5767 / 282.36 | 9.6679 / 301.01 | 通过 |

**xprof / DTK 表现：** 最终22ef另采四shape与rocBLAS；该数据描述B rebase最终库，不是BO所有对照候选。 证据P06：[results/profile_final_bbase_four_20261013/ANALYSIS.md](<../results/profile_final_bbase_four_20261013/ANALYSIS.md>)。

**候选对应关系与配置：**

- [V183](GEMM_源码与构建索引.md#v183) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：本索引首次出现该哈希。
- [V191](GEMM_源码与构建索引.md#v191) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_pow2b_reg768`：本索引首次出现该哈希。
- [V192](GEMM_源码与构建索引.md#v192) `rolephase_m1_bank_asmb_always_mid6_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768`：本索引首次出现该哈希。
- [V193](GEMM_源码与构建索引.md#v193) `rolephase_m1_bank_asmb_always_midgroup_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_bo_static_wave_b_prefetch/results.json>)；[environment.json](<../results/stage_bo_static_wave_b_prefetch/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-070"></a>
## 070 / BP：B rebase 独立复测

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_bp_scalar_b_base_independent`；时间：2026-10-13T15:31:54.708254+08:00。

**改了什么及结论：** 102组/306次包含单行、odd K、tail/guard的审计；独立约286–308T，相对静态分组耗时再降0.7%–2.3%。该22ef库成为后续稳定对照。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§38](<../LDS_WRAP_AND_PIPELINE.md:505>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4598 / 326.26 | 6.6709 / 327.19 | 2.1867 / 332.71 | 8.8862 / 327.49 | 库对照 |
| rocblas_mm_out | 4.4476 / 327.16 | 6.6427 / 328.58 | 2.1833 / 333.23 | 8.8592 / 328.49 | 库对照 |
| [V183](GEMM_源码与构建索引.md#v183) | 4.9051 / 296.65 | 7.2567 / 300.77 | 2.5729 / 282.77 | 9.7113 / 299.67 | 通过 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8710 / 298.73 | 7.0924 / 307.74 | 2.5482 / 285.51 | 9.4943 / 306.52 | 通过 |

**xprof / DTK 表现：** 22ef四shape VM完成等待/rocBLAS约2.00/2.19/5.92/2.38，out_proj差距突出；只作方向，不是ms比例。 证据P06：[results/profile_final_bbase_four_20261013/ANALYSIS.md](<../results/profile_final_bbase_four_20261013/ANALYSIS.md>)。

**候选对应关系与配置：**

- [V183](GEMM_源码与构建索引.md#v183) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

原始证据：[原始 results.json](<../results/stage_bp_scalar_b_base_independent/results.json>)；[environment.json](<../results/stage_bp_scalar_b_base_independent/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-071"></a>
## 071 / BQ：FP class RNE / 新预取点

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_bq_fpclass_rne_and_b_prefetch`；时间：2026-10-13T15:34:37.771266+08:00。

**改了什么及结论：** 用isnan分类仍保留CK payload，1507328位模式probe通过；完整GEMM仅0.1%–0.2%浮动，无独立收益确认。点4/6配B rebase也不胜出。

**源码位置：** [output_rne.hpp](<../output_rne.hpp>)；[rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§39](<../LDS_WRAP_AND_PIPELINE.md:520>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4524 / 326.81 | 6.6444 / 328.49 | 2.1879 / 332.54 | 8.8786 / 327.77 | 库对照 |
| rocblas_mm_out | 4.4372 / 327.93 | 6.6515 / 328.14 | 2.1849 / 332.98 | 8.8642 / 328.31 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8726 / 298.63 | 7.0862 / 308.01 | 2.5485 / 285.48 | 9.4964 / 306.45 | 通过 |
| [V194](GEMM_源码与构建索引.md#v194) | 4.8658 / 299.05 | 7.0811 / 308.23 | 2.5424 / 286.16 | 9.4894 / 306.68 | 通过 |
| [V195](GEMM_源码与构建索引.md#v195) | 4.8941 / 297.32 | 7.1303 / 306.11 | 2.5573 / 284.50 | 9.6303 / 302.19 | 通过 |
| [V196](GEMM_源码与构建索引.md#v196) | 4.9379 / 294.68 | 7.2085 / 302.78 | 2.5839 / 281.56 | 9.6618 / 301.20 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V194](GEMM_源码与构建索引.md#v194) `rolephase_m1_bank_asmb_always_mid_bufferepi_fpclassrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：本索引首次出现该哈希。
- [V195](GEMM_源码与构建索引.md#v195) `rolephase_m1_bank_asmb_always_mid4_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：本索引首次出现该哈希。
- [V196](GEMM_源码与构建索引.md#v196) `rolephase_m1_bank_asmb_always_mid6_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_bq_fpclass_rne_and_b_prefetch/results.json>)；[environment.json](<../results/stage_bq_fpclass_rne_and_b_prefetch/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-072"></a>
## 072 / BS：累加器子 bank 约束

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_bs_accumulator_subbanks`；时间：2026-10-13T15:41:35.357172+08:00。

**改了什么及结论：** 在循环头尾或每相位用asm约束16/32寄存器子组，代替BR失败的大bank约束。头尾方案产生大量spill，phase-only无净收益；未实际实现全部操作数对齐。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§40](<../LDS_WRAP_AND_PIPELINE.md:528>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4266 / 328.71 | 6.6580 / 327.82 | 2.1997 / 330.74 | 8.8784 / 327.78 | 库对照 |
| rocblas_mm_out | 4.4283 / 328.59 | 6.6602 / 327.71 | 2.1889 / 332.37 | 8.8760 / 327.87 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8704 / 298.76 | 7.0932 / 307.71 | 2.5453 / 285.84 | 9.5024 / 306.26 | 通过 |
| [V197](GEMM_源码与构建索引.md#v197) | 8.7963 / 165.42 | 13.0351 / 167.44 | 4.6076 / 157.90 | 16.8179 / 173.04 | 通过 |
| [V198](GEMM_源码与构建索引.md#v198) | 4.8805 / 298.14 | 7.1081 / 307.06 | 2.5540 / 284.87 | 9.5188 / 305.73 | 通过 |
| [V199](GEMM_源码与构建索引.md#v199) | 11.8331 / 122.97 | 17.6488 / 123.67 | 6.6002 / 110.23 | 22.7149 / 128.12 | 通过 |
| [V200](GEMM_源码与构建索引.md#v200) | 4.8710 / 298.73 | 7.1079 / 307.07 | 2.5543 / 284.83 | 9.5161 / 305.82 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V197](GEMM_源码与构建索引.md#v197) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack1_aw32_reg768`：本索引首次出现该哈希。
- [V198](GEMM_源码与构建索引.md#v198) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack2_aw32_reg768`：本索引首次出现该哈希。
- [V199](GEMM_源码与构建索引.md#v199) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack3_aw32_reg768`：本索引首次出现该哈希。
- [V200](GEMM_源码与构建索引.md#v200) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack2_aw16_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_bs_accumulator_subbanks/results.json>)；[environment.json](<../results/stage_bs_accumulator_subbanks/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-073"></a>
## 073 / BT：新基线 producer / priority 重测

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_bt_producer_priority_balance`；时间：2026-10-13T15:48:10.567087+08:00。

**改了什么及结论：** 在22ef基线重试A burst、A base、A cache以及priority2/3。大多回退或0.1%浮动，无稳定收益；不能把更大priority理解为更快。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§41](<../LDS_WRAP_AND_PIPELINE.md:536>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4712 / 325.43 | 6.6621 / 327.62 | 2.1992 / 330.83 | 8.9069 / 326.73 | 库对照 |
| rocblas_mm_out | 4.4706 / 325.48 | 6.6480 / 328.31 | 2.1854 / 332.91 | 8.8921 / 327.28 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8694 / 298.82 | 7.0923 / 307.75 | 2.5486 / 285.46 | 9.4995 / 306.35 | 通过 |
| [V201](GEMM_源码与构建索引.md#v201) | 4.9105 / 296.32 | 7.2026 / 303.03 | 2.5683 / 283.28 | 9.6535 / 301.46 | 通过 |
| [V202](GEMM_源码与构建索引.md#v202) | 4.9190 / 295.81 | 7.1419 / 305.61 | 2.5440 / 285.99 | 9.5534 / 304.62 | 通过 |
| [V203](GEMM_源码与构建索引.md#v203) | 5.2493 / 277.20 | 7.6423 / 285.60 | 2.7381 / 265.71 | 10.2561 / 283.75 | 通过 |
| [V204](GEMM_源码与构建索引.md#v204) | 4.8720 / 298.66 | 7.0871 / 307.97 | 2.5464 / 285.72 | 9.4962 / 306.46 | 通过 |
| [V205](GEMM_源码与构建索引.md#v205) | 4.8698 / 298.80 | 7.0861 / 308.01 | 2.5458 / 285.78 | 9.5014 / 306.29 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V201](GEMM_源码与构建索引.md#v201) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_aburst_reg768`：本索引首次出现该哈希。
- [V202](GEMM_源码与构建索引.md#v202) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_abase_reg768`：本索引首次出现该哈希。
- [V203](GEMM_源码与构建索引.md#v203) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_cachea_reg768`：本索引首次出现该哈希。
- [V204](GEMM_源码与构建索引.md#v204) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pv2_reg768`：本索引首次出现该哈希。
- [V205](GEMM_源码与构建索引.md#v205) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pv3_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_bt_producer_priority_balance/results.json>)；[environment.json](<../results/stage_bt_producer_priority_balance/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-074"></a>
## 074 / BU：保留低号寄存器改变起点

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_bu_low_register_constraints`；时间：2026-10-13T15:52:53.808736+08:00。

**改了什么及结论：** 用保守clobber约束v0/v1/v2，使首条C/A/B起点从mod4=1移到2/3/0，实际VGPR225→228。全首条对齐确实发生但无加速，未证明所有寄存器bank问题不存在。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§41](<../LDS_WRAP_AND_PIPELINE.md:536>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4670 / 325.74 | 6.6412 / 328.65 | 2.1886 / 332.43 | 8.8911 / 327.31 | 库对照 |
| rocblas_mm_out | 4.4510 / 326.91 | 6.6465 / 328.39 | 2.1852 / 332.93 | 8.8670 / 328.20 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8736 / 298.57 | 7.0907 / 307.82 | 2.5501 / 285.30 | 9.4929 / 306.56 | 通过 |
| [V206](GEMM_源码与构建索引.md#v206) | 4.8783 / 298.28 | 7.1155 / 306.74 | 2.5526 / 285.02 | 9.5422 / 304.98 | 通过 |
| [V207](GEMM_源码与构建索引.md#v207) | 4.8787 / 298.26 | 7.1010 / 307.37 | 2.5533 / 284.94 | 9.5300 / 305.37 | 通过 |
| [V208](GEMM_源码与构建索引.md#v208) | 4.8787 / 298.26 | 7.1007 / 307.38 | 2.5537 / 284.90 | 9.5385 / 305.10 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V206](GEMM_源码与构建索引.md#v206) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reserve1_reg768`：本索引首次出现该哈希。
- [V207](GEMM_源码与构建索引.md#v207) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reserve2_reg768`：本索引首次出现该哈希。
- [V208](GEMM_源码与构建索引.md#v208) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reserve3_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_bu_low_register_constraints/results.json>)；[environment.json](<../results/stage_bu_low_register_constraints/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-075"></a>
## 075 / BV：native cadence 上的 K64 B bank

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_bv_native_k64_activation_banks`；时间：2026-10-13T16:07:55.714915+08:00。

**改了什么及结论：** 在现静态wave同步节奏中预取完整K64，分别phase0/1发future，入口VM0。处理terminal K+32的bound下溢及SGPR descriptor约束；四候选家族总体慢于K32。

**源码位置：** [phase_k64_native.hpp](<../phase_k64_native.hpp>)；[rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§42](<../LDS_WRAP_AND_PIPELINE.md:551>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4216 / 329.09 | 6.6467 / 328.38 | 2.2112 / 329.03 | 8.8799 / 327.73 | 库对照 |
| rocblas_mm_out | 4.4151 / 329.57 | 6.6426 / 328.58 | 2.1915 / 331.99 | 8.8849 / 327.54 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8655 / 299.06 | 7.0907 / 307.82 | 2.5491 / 285.41 | 9.4998 / 306.34 | 通过 |
| [V209](GEMM_源码与构建索引.md#v209) | 6.5138 / 223.39 | 9.5916 / 227.56 | 3.3846 / 214.95 | 12.8389 / 226.67 | 通过 |
| [V210](GEMM_源码与构建索引.md#v210) | 5.6150 / 259.14 | 8.2855 / 263.43 | 2.9343 / 247.95 | 11.0620 / 263.08 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V209](GEMM_源码与构建索引.md#v209) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p0_reg768`：本索引首次出现该哈希。
- [V210](GEMM_源码与构建索引.md#v210) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p1_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_bv_native_k64_activation_banks/results.json>)；[environment.json](<../results/stage_bv_native_k64_activation_banks/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-076"></a>
## 076 / BW：K64 B 完成等待延后

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_bw_native_k64_late_wait`；时间：2026-10-13T16:12:43.466685+08:00。

**改了什么及结论：** 把current等待移到DS之后，按实际issue序列使用VM4或VM0，保留startup与terminal drain。phase1恢复部分吞吐至265–283T但仍落后；各102组审计通过且无spill。

**源码位置：** [phase_k64_native.hpp](<../phase_k64_native.hpp>)；[rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§42](<../LDS_WRAP_AND_PIPELINE.md:551>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4469 / 327.22 | 6.6389 / 328.77 | 2.1866 / 332.72 | 8.9107 / 326.59 | 库对照 |
| rocblas_mm_out | 4.4323 / 328.29 | 6.6501 / 328.21 | 2.1849 / 332.99 | 8.9201 / 326.25 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8692 / 298.84 | 7.0903 / 307.83 | 2.5468 / 285.67 | 9.4938 / 306.54 | 通过 |
| [V211](GEMM_源码与构建索引.md#v211) | 6.5647 / 221.65 | 9.7043 / 224.91 | 3.4087 / 213.43 | 12.9541 / 224.65 | 通过 |
| [V212](GEMM_源码与构建索引.md#v212) | 5.2238 / 278.55 | 7.7141 / 282.94 | 2.7447 / 265.07 | 10.3485 / 281.22 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V211](GEMM_源码与构建索引.md#v211) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p0_late_reg768`：本索引首次出现该哈希。
- [V212](GEMM_源码与构建索引.md#v212) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p1_late_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_bw_native_k64_late_wait/results.json>)；[environment.json](<../results/stage_bw_native_k64_late_wait/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-077"></a>
## 077 / BX：固定 N/K 特化

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_bx_fixed_nk_specialization`；时间：2026-10-13T17:23:26.823443+08:00。

**改了什么及结论：** 只固定N、只固定K、固定N+K，各保留运行时M和非目标回退，并禁止整个长K循环自动展开。变化很小且不一致，未采用；多kernel库资源不能只读首实例。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)；[kernel_metadata.py](<../kernel_metadata.py>)。

历史算法/同步/正确性细节：[原记录§43](<../LDS_WRAP_AND_PIPELINE.md:568>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4368 / 327.96 | 6.6627 / 327.59 | 2.1878 / 332.55 | 8.9154 / 326.42 | 库对照 |
| rocblas_mm_out | 4.4398 / 327.74 | 6.6623 / 327.61 | 2.1851 / 332.95 | 8.8791 / 327.76 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8711 / 298.72 | 7.0873 / 307.96 | 2.5486 / 285.47 | 9.4920 / 306.59 | 通过 |
| [V213](GEMM_源码与构建索引.md#v213) | 4.8633 / 299.20 | 7.1125 / 306.87 | 2.5425 / 286.15 | 9.5442 / 304.92 | 通过 |
| [V214](GEMM_源码与构建索引.md#v214) | 4.8612 / 299.33 | 7.1136 / 306.82 | 2.5559 / 284.66 | 9.5315 / 305.32 | 通过 |
| [V215](GEMM_源码与构建索引.md#v215) | 4.8696 / 298.81 | 7.0893 / 307.88 | 2.5483 / 285.50 | 9.4905 / 306.64 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V213](GEMM_源码与构建索引.md#v213) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_fixednk_reg768`：本索引首次出现该哈希。
- [V214](GEMM_源码与构建索引.md#v214) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_fixedk_reg768`：本索引首次出现该哈希。
- [V215](GEMM_源码与构建索引.md#v215) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_fixedn_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_bx_fixed_nk_specialization/results.json>)；[environment.json](<../results/stage_bx_fixed_nk_specialization/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-078"></a>
## 078 / BY：最终 raw 基线的 CTA 遍历

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_by_final_bbase_cta_traversal`；时间：2026-10-13T17:27:02.508305+08:00。

**改了什么及结论：** 在22ef上重试group1/2/4而不是拿BK旧流水结果代替。部分out_proj小幅改善，QKV/FC1明显变慢，未采用全局改变。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§43](<../LDS_WRAP_AND_PIPELINE.md:568>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4459 / 327.28 | 6.6517 / 328.13 | 2.1893 / 332.32 | 8.8709 / 328.06 | 库对照 |
| rocblas_mm_out | 4.4534 / 326.74 | 6.6623 / 327.61 | 2.1866 / 332.72 | 8.8794 / 327.74 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8726 / 298.63 | 7.0890 / 307.89 | 2.5458 / 285.79 | 9.4972 / 306.43 | 通过 |
| [V216](GEMM_源码与构建索引.md#v216) | 5.1514 / 282.46 | 8.0644 / 270.65 | 2.5294 / 287.63 | 11.8886 / 244.79 | 通过 |
| [V217](GEMM_源码与构建索引.md#v217) | 5.0253 / 289.56 | 8.0038 / 272.70 | 2.5326 / 287.27 | 10.6578 / 273.06 | 通过 |
| [V218](GEMM_源码与构建索引.md#v218) | 4.9627 / 293.21 | 7.2479 / 301.14 | 2.5526 / 285.02 | 9.6855 / 300.47 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V216](GEMM_源码与构建索引.md#v216) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_g1_reg768`：本索引首次出现该哈希。
- [V217](GEMM_源码与构建索引.md#v217) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_g2_reg768`：本索引首次出现该哈希。
- [V218](GEMM_源码与构建索引.md#v218) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_g4_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_by_final_bbase_cta_traversal/results.json>)；[environment.json](<../results/stage_by_final_bbase_cta_traversal/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-079"></a>
## 079 / BZ：K64 地址 + 四 MMAC 指令窗口

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_bz_native_b64_addressing_and_mmac_window`；时间：2026-10-13T18:30:12.551689+08:00。

**改了什么及结论：** 比较去掉K32 rebase的native K64、pow2 K64及四MMAC同asm窗口。使用early-clobber防止输出覆盖后续输入；六候选正确却都慢。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)；[phase_k64_native.hpp](<../phase_k64_native.hpp>)；[mmac_group4.hpp](<../mmac_group4.hpp>)。

历史算法/同步/正确性细节：[原记录§43](<../LDS_WRAP_AND_PIPELINE.md:568>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4331 / 328.24 | 6.6588 / 327.78 | 2.1912 / 332.04 | 8.9008 / 326.96 | 库对照 |
| rocblas_mm_out | 4.4235 / 328.95 | 6.6587 / 327.79 | 2.1875 / 332.60 | 8.9002 / 326.98 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8643 / 299.14 | 7.0853 / 308.05 | 2.5452 / 285.86 | 9.4944 / 306.51 | 通过 |
| [V219](GEMM_源码与构建索引.md#v219) | 6.3081 / 230.67 | 9.3254 / 234.05 | 3.2755 / 222.11 | 12.4540 / 233.67 | 通过 |
| [V220](GEMM_源码与构建索引.md#v220) | 5.3225 / 273.38 | 7.8732 / 277.22 | 2.7927 / 260.52 | 10.5327 / 276.30 | 通过 |
| [V221](GEMM_源码与构建索引.md#v221) | 5.4730 / 265.87 | 8.4716 / 257.64 | 2.8701 / 253.49 | 11.3410 / 256.61 | 通过 |
| [V222](GEMM_源码与构建索引.md#v222) | 5.4744 / 265.80 | 8.2820 / 263.54 | 2.7044 / 269.02 | 11.0817 / 262.61 | 通过 |
| [V223](GEMM_源码与构建索引.md#v223) | 4.9071 / 296.53 | 7.1599 / 304.84 | 2.5695 / 283.14 | 9.5946 / 303.32 | 通过 |
| [V224](GEMM_源码与构建索引.md#v224) | 5.3021 / 274.43 | 7.7740 / 280.76 | 2.7477 / 264.78 | 10.4276 / 279.08 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V219](GEMM_源码与构建索引.md#v219) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p0_late_reg768`：本索引首次出现该哈希。
- [V220](GEMM_源码与构建索引.md#v220) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_reg768`：本索引首次出现该哈希。
- [V221](GEMM_源码与构建索引.md#v221) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p0_late_pow2b_reg768`：本索引首次出现该哈希。
- [V222](GEMM_源码与构建索引.md#v222) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_pow2b_reg768`：本索引首次出现该哈希。
- [V223](GEMM_源码与构建索引.md#v223) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mmacg4_reg768`：本索引首次出现该哈希。
- [V224](GEMM_源码与构建索引.md#v224) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mmacg4_dsasm_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_bz_native_b64_addressing_and_mmac_window/results.json>)；[environment.json](<../results/stage_bz_native_b64_addressing_and_mmac_window/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-080"></a>
## 080 / CA：静态 wave 组比例与预取点

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_ca_wave_group_prefetch_balance`；时间：2026-10-13T18:33:46.873263+08:00。

**改了什么及结论：** 把4/4分组改2/6或6/2，另比较两组各自的4/8/12预取点。改变发射竞争但没有整体稳定收益。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§43](<../LDS_WRAP_AND_PIPELINE.md:568>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4141 / 329.65 | 6.6696 / 327.25 | 2.1923 / 331.87 | 8.9007 / 326.96 | 库对照 |
| rocblas_mm_out | 4.4031 / 330.47 | 6.6509 / 328.17 | 2.1869 / 332.68 | 8.8983 / 327.05 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8641 / 299.15 | 7.0922 / 307.75 | 2.5478 / 285.55 | 9.4995 / 306.35 | 通过 |
| [V225](GEMM_源码与构建索引.md#v225) | 5.8076 / 250.55 | 8.5970 / 253.88 | 2.9614 / 245.68 | 11.4682 / 253.76 | 通过 |
| [V226](GEMM_源码与构建索引.md#v226) | 5.1356 / 283.33 | 7.5879 / 287.65 | 2.6667 / 272.82 | 10.1425 / 286.93 | 通过 |
| [V227](GEMM_源码与构建索引.md#v227) | 4.8647 / 299.11 | 7.1059 / 307.16 | 2.5503 / 285.28 | 9.5153 / 305.84 | 通过 |
| [V228](GEMM_源码与构建索引.md#v228) | 4.8942 / 297.31 | 7.1931 / 303.43 | 2.5619 / 283.99 | 9.6306 / 302.18 | 通过 |
| [V229](GEMM_源码与构建索引.md#v229) | 4.8847 / 297.89 | 7.1281 / 306.20 | 2.5586 / 284.35 | 9.5449 / 304.89 | 通过 |
| [V230](GEMM_源码与构建索引.md#v230) | 4.8523 / 299.87 | 7.1236 / 306.40 | 2.5433 / 286.07 | 9.5148 / 305.86 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V225](GEMM_源码与构建索引.md#v225) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_sf2_reg768`：本索引首次出现该哈希。
- [V226](GEMM_源码与构建索引.md#v226) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_sf6_reg768`：本索引首次出现该哈希。
- [V227](GEMM_源码与构建索引.md#v227) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_gp4x8_reg768`：本索引首次出现该哈希。
- [V228](GEMM_源码与构建索引.md#v228) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_gp8x4_reg768`：本索引首次出现该哈希。
- [V229](GEMM_源码与构建索引.md#v229) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_gp8x12_reg768`：本索引首次出现该哈希。
- [V230](GEMM_源码与构建索引.md#v230) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_gp12x8_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_ca_wave_group_prefetch_balance/results.json>)；[environment.json](<../results/stage_ca_wave_group_prefetch_balance/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-081"></a>
## 081 / CB：相位之间更早预取 K64 B

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_cb_native_prefetch_between_math_phases`；时间：2026-10-13T18:37:46.581469+08:00。

**改了什么及结论：** 把future B放到下一相位DS开始的点0/4。后续ISA核对指出原生实际为8DS→B→8DS，因此本轮是时机对照，不是精确复刻原生；未胜出。

**源码位置：** [phase_k64_native.hpp](<../phase_k64_native.hpp>)；[rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§43](<../LDS_WRAP_AND_PIPELINE.md:568>)；[原记录§44](<../LDS_WRAP_AND_PIPELINE.md:574>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4528 / 326.78 | 6.6469 / 328.37 | 2.1881 / 332.51 | 8.8824 / 327.63 | 库对照 |
| rocblas_mm_out | 4.4536 / 326.72 | 6.6585 / 327.80 | 2.1828 / 333.30 | 8.8880 / 327.43 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8724 / 298.64 | 7.0867 / 307.99 | 2.5473 / 285.62 | 9.4925 / 306.58 | 通过 |
| [V220](GEMM_源码与构建索引.md#v220) | 5.3248 / 273.26 | 7.8728 / 277.24 | 2.7942 / 260.37 | 10.5265 / 276.46 | 通过 |
| [V231](GEMM_源码与构建索引.md#v231) | 5.6008 / 259.80 | 8.2483 / 264.62 | 2.9149 / 249.60 | 11.0170 / 264.15 | 通过 |
| [V232](GEMM_源码与构建索引.md#v232) | 5.4658 / 266.22 | 8.2204 / 265.51 | 2.8542 / 254.91 | 10.9655 / 265.39 | 通过 |
| [V233](GEMM_源码与构建索引.md#v233) | 5.6530 / 257.40 | 8.5154 / 256.32 | 2.9106 / 249.96 | 11.3542 / 256.31 | 通过 |
| [V234](GEMM_源码与构建索引.md#v234) | 5.5920 / 260.21 | 8.3538 / 261.27 | 2.8296 / 257.12 | 11.1547 / 260.89 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V220](GEMM_源码与构建索引.md#v220) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V231](GEMM_源码与构建索引.md#v231) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_np0_reg768`：本索引首次出现该哈希。
- [V232](GEMM_源码与构建索引.md#v232) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_np4_reg768`：本索引首次出现该哈希。
- [V233](GEMM_源码与构建索引.md#v233) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_pow2b_np0_reg768`：本索引首次出现该哈希。
- [V234](GEMM_源码与构建索引.md#v234) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_pow2b_np4_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_cb_native_prefetch_between_math_phases/results.json>)；[environment.json](<../results/stage_cb_native_prefetch_between_math_phases/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-082"></a>
## 082 / CC：A 的两组 LDS 基地址

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_cc_two_lds_bases_and_immediate_reads`；时间：2026-10-13T18:49:33.493537+08:00。

**改了什么及结论：** 将phys(row+i*16,c)分解为两基地址加立即偏移，CPU验证全覆盖，比较C++、完整LGKM、显式单DS/成对DS。地址VGPR减少但没有净收益；同SHA候选去重。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§44](<../LDS_WRAP_AND_PIPELINE.md:574>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4398 / 327.74 | 6.6521 / 328.11 | 2.1876 / 332.58 | 8.8986 / 327.04 | 库对照 |
| rocblas_mm_out | 4.4285 / 328.57 | 6.6544 / 328.00 | 2.1860 / 332.83 | 8.8795 / 327.74 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8638 / 299.16 | 7.0944 / 307.66 | 2.5470 / 285.65 | 9.4965 / 306.45 | 通过 |
| [V235](GEMM_源码与构建索引.md#v235) | 4.8856 / 297.83 | 7.1453 / 305.46 | 2.5505 / 285.25 | 9.5735 / 303.98 | 通过 |
| [V236](GEMM_源码与构建索引.md#v236) | 4.8700 / 298.79 | 7.1253 / 306.32 | 2.5528 / 284.99 | 9.5310 / 305.34 | 通过 |
| [V237](GEMM_源码与构建索引.md#v237) | 4.9241 / 295.50 | 7.2181 / 302.38 | 2.5788 / 282.12 | 9.6736 / 300.84 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V235](GEMM_源码与构建索引.md#v235) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob1_reg768`：本索引首次出现该哈希。
- [V236](GEMM_源码与构建索引.md#v236) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob1_areadwait_reg768`：本索引首次出现该哈希。
- [V237](GEMM_源码与构建索引.md#v237) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob2_reg768`：本索引首次出现该哈希。
- [V237](GEMM_源码与构建索引.md#v237) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob3_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

本轮同SHA去重：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob3_reg768` → `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob2_reg768`。

原始证据：[原始 results.json](<../results/stage_cc_two_lds_bases_and_immediate_reads/results.json>)；[environment.json](<../results/stage_cc_two_lds_bases_and_immediate_reads/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-083"></a>
## 083 / CD：B 完成等待与 barrier 解耦

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_cd_b_wait_and_barrier_decoupling`；时间：2026-10-13T18:53:57.559728+08:00。

**改了什么及结论：** 对比K32 VM等待移到barrier后，及K64在相位末端等待的组合，保留消费前完成条件。四shape初筛正确，但不足以改善整体流水。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)；[phase_k64_native.hpp](<../phase_k64_native.hpp>)。

历史算法/同步/正确性细节：[原记录§44](<../LDS_WRAP_AND_PIPELINE.md:574>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4501 / 326.98 | 6.6564 / 327.90 | 2.1850 / 332.97 | 8.8751 / 327.90 | 库对照 |
| rocblas_mm_out | 4.4447 / 327.38 | 6.6544 / 328.00 | 2.1830 / 333.28 | 8.9021 / 326.91 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8731 / 298.60 | 7.0986 / 307.47 | 2.5464 / 285.72 | 9.5078 / 306.08 | 通过 |
| [V220](GEMM_源码与构建索引.md#v220) | 5.3272 / 273.15 | 7.8740 / 277.19 | 2.7936 / 260.43 | 10.5251 / 276.50 | 通过 |
| [V238](GEMM_源码与构建索引.md#v238) | 4.8724 / 298.64 | 7.1021 / 307.32 | 2.5475 / 285.59 | 9.5214 / 305.64 | 通过 |
| [V239](GEMM_源码与构建索引.md#v239) | 6.3223 / 230.15 | 9.3370 / 233.76 | 3.2794 / 221.85 | 12.4659 / 233.45 | 通过 |
| [V240](GEMM_源码与构建索引.md#v240) | 5.5920 / 260.21 | 8.2353 / 265.03 | 2.9152 / 249.57 | 11.0067 / 264.40 | 通过 |
| [V241](GEMM_源码与构建索引.md#v241) | 5.4651 / 266.25 | 8.1678 / 267.23 | 2.7691 / 262.74 | 10.9457 / 265.87 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V220](GEMM_源码与构建索引.md#v220) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V238](GEMM_源码与构建索引.md#v238) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_vmafter_reg768`：本索引首次出现该哈希。
- [V239](GEMM_源码与构建索引.md#v239) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p0_endwait_reg768`：本索引首次出现该哈希。
- [V240](GEMM_源码与构建索引.md#v240) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_endwait_reg768`：本索引首次出现该哈希。
- [V241](GEMM_源码与构建索引.md#v241) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_endwait_pow2b_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_cd_b_wait_and_barrier_decoupling/results.json>)；[environment.json](<../results/stage_cd_b_wait_and_barrier_decoupling/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-084"></a>
## 084 / CE：独立 C tile 的 MMAC 顺序

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_ce_independent_mmac_tile_order`；时间：2026-10-13T18:58:58.579721+08:00。

**改了什么及结论：** 按4/8/16个独立C tile分块调度数学，另配两LDS基地址，保持各累加器的K加法顺序。降低局部依赖的设想没有转化为净收益。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§44](<../LDS_WRAP_AND_PIPELINE.md:574>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4219 / 329.06 | 6.6337 / 329.02 | 2.1866 / 332.73 | 8.9021 / 326.91 | 库对照 |
| rocblas_mm_out | 4.4137 / 329.67 | 6.6343 / 328.99 | 2.1836 / 333.18 | 8.8816 / 327.66 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8666 / 298.99 | 7.0921 / 307.76 | 2.5449 / 285.88 | 9.4939 / 306.53 | 通过 |
| [V242](GEMM_源码与构建索引.md#v242) | 4.8565 / 299.62 | 7.1752 / 304.19 | 2.5576 / 284.46 | 9.5916 / 303.41 | 通过 |
| [V243](GEMM_源码与构建索引.md#v243) | 4.9555 / 293.63 | 7.3538 / 296.80 | 2.5851 / 281.43 | 9.8653 / 294.99 | 通过 |
| [V244](GEMM_源码与构建索引.md#v244) | 4.8817 / 298.07 | 7.1958 / 303.32 | 2.5679 / 283.32 | 9.6034 / 303.03 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V242](GEMM_源码与构建索引.md#v242) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mchunk4_reg768`：本索引首次出现该哈希。
- [V243](GEMM_源码与构建索引.md#v243) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mchunk8_reg768`：本索引首次出现该哈希。
- [V244](GEMM_源码与构建索引.md#v244) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mchunk8_atwob1_reg768`：本索引首次出现该哈希。
- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mchunk16_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

本轮同SHA去重：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mchunk16_reg768` → `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`。

原始证据：[原始 results.json](<../results/stage_ce_independent_mmac_tile_order/results.json>)；[environment.json](<../results/stage_ce_independent_mmac_tile_order/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-085"></a>
## 085 / CF：producer 终末分支

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_cf_producer_terminal_branch`；时间：2026-10-13T19:02:02.719212+08:00。

**改了什么及结论：** 比较无条件下一A预取及保留bound保护的终末处理，控制循环分支成本。正确性初筛通过但没有收益，不能任意删除终末完成等待。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§44](<../LDS_WRAP_AND_PIPELINE.md:574>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4157 / 329.53 | 6.6430 / 328.56 | 2.1902 / 332.18 | 8.8972 / 327.09 | 库对照 |
| rocblas_mm_out | 4.4152 / 329.56 | 6.6356 / 328.93 | 2.1871 / 332.65 | 8.8840 / 327.57 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8656 / 299.06 | 7.0896 / 307.87 | 2.5437 / 286.02 | 9.4936 / 306.54 | 通过 |
| [V245](GEMM_源码与构建索引.md#v245) | 4.8753 / 298.46 | 7.1169 / 306.68 | 2.5530 / 284.97 | 9.5301 / 305.37 | 通过 |
| [V246](GEMM_源码与构建索引.md#v246) | 4.8841 / 297.92 | 7.1138 / 306.82 | 2.5560 / 284.64 | 9.5171 / 305.78 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V245](GEMM_源码与构建索引.md#v245) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_aalways1_reg768`：本索引首次出现该哈希。
- [V246](GEMM_源码与构建索引.md#v246) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_aalways2_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_cf_producer_terminal_branch/results.json>)；[environment.json](<../results/stage_cf_producer_terminal_branch/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-086"></a>
## 086 / CG：producer 固定槽双 tile

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_cg_producer_fixed_slot_pairs`；时间：2026-10-13T19:12:06.986339+08:00。

**改了什么及结论：** 把producer也按slot0/1两tile展开，减少动态slot地址计算，保持匹配的双barrier和奇数尾。单独版本不稳定胜出，为CH组合提供基础。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§45](<../LDS_WRAP_AND_PIPELINE.md:588>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4255 / 328.79 | 6.6589 / 327.78 | 2.1910 / 332.06 | 8.8751 / 327.90 | 库对照 |
| rocblas_mm_out | 4.4364 / 327.99 | 6.6302 / 329.20 | 2.1888 / 332.39 | 8.9056 / 326.78 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8682 / 298.90 | 7.0865 / 308.00 | 2.5456 / 285.81 | 9.4949 / 306.50 | 通过 |
| [V247](GEMM_源码与构建索引.md#v247) | 4.9648 / 293.08 | 7.1745 / 304.22 | 2.5496 / 285.35 | 9.6013 / 303.10 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V247](GEMM_源码与构建索引.md#v247) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair1_reg768`：本索引首次出现该哈希。
- [V247](GEMM_源码与构建索引.md#v247) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair2_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

本轮同SHA去重：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair2_reg768` → `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair1_reg768`。

原始证据：[原始 results.json](<../results/stage_cg_producer_fixed_slot_pairs/results.json>)；[environment.json](<../results/stage_cg_producer_fixed_slot_pairs/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-087"></a>
## 087 / CH：权重 K 使用 scalar offset

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_ch_producer_scalar_offset`；时间：2026-10-13T19:17:57.072154+08:00。

**改了什么及结论：** A的tile公共K位移走soffset，row地址留voffset；单独与producer pairs组合对照。组合只改善FC2/out_proj，不能直接当四shape统一优化。

**源码位置：** [async_a_soffset.hpp](<../async_a_soffset.hpp>)；[rolephase.cpp](<../rolephase.cpp>)；[producer_soffset_probe.cpp](<../producer_soffset_probe.cpp>)。

历史算法/同步/正确性细节：[原记录§45](<../LDS_WRAP_AND_PIPELINE.md:588>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4253 / 328.81 | 6.6559 / 327.92 | 2.1889 / 332.38 | 8.9000 / 326.99 | 库对照 |
| rocblas_mm_out | 4.4210 / 329.13 | 6.6507 / 328.18 | 2.1857 / 332.87 | 8.8822 / 327.64 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8664 / 299.01 | 7.0961 / 307.58 | 2.5460 / 285.77 | 9.4916 / 306.60 | 通过 |
| [V248](GEMM_源码与构建索引.md#v248) | 4.8794 / 298.21 | 7.1470 / 305.39 | 2.5434 / 286.05 | 9.5595 / 304.43 | 通过 |
| [V249](GEMM_源码与构建索引.md#v249) | 4.8416 / 300.54 | 7.1163 / 306.71 | 2.5364 / 286.84 | 9.5333 / 305.26 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V248](GEMM_源码与构建索引.md#v248) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_asoff_reg768`：本索引首次出现该哈希。
- [V249](GEMM_源码与构建索引.md#v249) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair1_asoff_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_ch_producer_scalar_offset/results.json>)；[environment.json](<../results/stage_ch_producer_scalar_offset/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-088"></a>
## 088 / CI：producer 组合独立复测

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_ci_producer_scalar_offset_independent`；时间：2026-10-13T19:22:29.117496+08:00。

**改了什么及结论：** 复测pairs+A soffset，并按相应源码哈希核对真实计算与广审计。两投影小幅收益，其他不胜出，进入按shape分派。

**源码位置：** [async_a_soffset.hpp](<../async_a_soffset.hpp>)；[rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§45](<../LDS_WRAP_AND_PIPELINE.md:588>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4079 / 330.11 | 6.6387 / 328.77 | 2.1862 / 332.79 | 8.8898 / 327.36 | 库对照 |
| rocblas_mm_out | 4.4055 / 330.29 | 6.6376 / 328.83 | 2.1844 / 333.06 | 8.8983 / 327.05 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8685 / 298.88 | 7.0917 / 307.77 | 2.5465 / 285.70 | 9.5019 / 306.27 | 通过 |
| [V249](GEMM_源码与构建索引.md#v249) | 4.8427 / 300.47 | 7.1082 / 307.06 | 2.5358 / 286.91 | 9.5370 / 305.14 | 通过 |
| [V248](GEMM_源码与构建索引.md#v248) | 4.8720 / 298.66 | 7.1362 / 305.85 | 2.5456 / 285.81 | 9.5661 / 304.22 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V249](GEMM_源码与构建索引.md#v249) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair1_asoff_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V248](GEMM_源码与构建索引.md#v248) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_asoff_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

原始证据：[原始 results.json](<../results/stage_ci_producer_scalar_offset_independent/results.json>)；[environment.json](<../results/stage_ci_producer_scalar_offset_independent/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-089"></a>
## 089 / CJ：两投影 producer 分派

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_cj_two_projection_producer_dispatch`；时间：2026-10-13T19:28:33.241430+08:00。

**改了什么及结论：** shape_dispatch.cpp只给FC2/out_proj用producer组合，其余原raw。统一入口本身126组/378次验证含真实N/K小M尾部，与原22ef逐位一致。

**源码位置：** [shape_dispatch.cpp](<../shape_dispatch.cpp>)；[rolephase.cpp](<../rolephase.cpp>)；[async_a_soffset.hpp](<../async_a_soffset.hpp>)。

历史算法/同步/正确性细节：[原记录§45](<../LDS_WRAP_AND_PIPELINE.md:588>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4314 / 328.36 | 6.6459 / 328.42 | 2.1957 / 331.35 | 8.8997 / 327.00 | 库对照 |
| rocblas_mm_out | 4.4253 / 328.81 | 6.6374 / 328.84 | 2.1921 / 331.89 | 8.8852 / 327.53 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8661 / 299.02 | 7.0906 / 307.82 | 2.5464 / 285.72 | 9.4920 / 306.59 | 通过 |
| [V250](GEMM_源码与构建索引.md#v250) | 4.8424 / 300.49 | 7.0904 / 307.83 | 2.5363 / 286.85 | 9.4909 / 306.63 | 通过 |
| [V249](GEMM_源码与构建索引.md#v249) | 4.8411 / 300.57 | 7.1093 / 307.01 | 2.5359 / 286.89 | 9.5178 / 305.76 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V250](GEMM_源码与构建索引.md#v250) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768`：本索引首次出现该哈希。
- [V249](GEMM_源码与构建索引.md#v249) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair1_asoff_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

原始证据：[原始 results.json](<../results/stage_cj_two_projection_producer_dispatch/results.json>)；[environment.json](<../results/stage_cj_two_projection_producer_dispatch/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-090"></a>
## 090 / CK：producer 分派独立复测

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_ck_shape_dispatch_independent`；时间：2026-10-13T19:31:12.669779+08:00。

**改了什么及结论：** FC2约300.17T、out约286.64T，较原控制耗时仅降约0.48%/0.38%，其余基本不变。保留可选d2e库，不夸大为模型E2E收益。

**源码位置：** [shape_dispatch.cpp](<../shape_dispatch.cpp>)；[rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§45](<../LDS_WRAP_AND_PIPELINE.md:588>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4468 / 327.22 | 6.6588 / 327.78 | 2.1876 / 332.57 | 8.8524 / 328.74 | 库对照 |
| rocblas_mm_out | 4.4413 / 327.63 | 6.6489 / 328.27 | 2.1866 / 332.72 | 8.8510 / 328.80 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8708 / 298.74 | 7.0912 / 307.79 | 2.5479 / 285.55 | 9.4918 / 306.60 | 通过 |
| [V250](GEMM_源码与构建索引.md#v250) | 4.8475 / 300.17 | 7.0873 / 307.96 | 2.5382 / 286.64 | 9.4917 / 306.60 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V250](GEMM_源码与构建索引.md#v250) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

原始证据：[原始 results.json](<../results/stage_ck_shape_dispatch_independent/results.json>)；[environment.json](<../results/stage_ck_shape_dispatch_independent/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-091"></a>
## 091 / CL：每个 K64 bank 单次 rebase

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_cl_native_b64_single_rebase`；时间：2026-10-13T19:51:19.777789+08:00。

**改了什么及结论：** raw_b_base64.hpp每future K64只更新一次pointer/bound，两K32子相位留voffset，减少descriptor/readfirstlane重复工作并证明尾部不下溢。K64家族改善约6%，仍不如K32主线。

**源码位置：** [raw_b_base64.hpp](<../raw_b_base64.hpp>)；[phase_k64_native.hpp](<../phase_k64_native.hpp>)。

历史算法/同步/正确性细节：[原记录§46](<../LDS_WRAP_AND_PIPELINE.md:604>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4407 / 327.67 | 6.6595 / 327.75 | 2.1940 / 331.61 | 8.9092 / 326.65 | 库对照 |
| rocblas_mm_out | 4.4441 / 327.42 | 6.6546 / 327.99 | 2.1851 / 332.96 | 8.9000 / 326.99 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8691 / 298.84 | 7.0899 / 307.85 | 2.5467 / 285.68 | 9.4953 / 306.49 | 通过 |
| [V250](GEMM_源码与构建索引.md#v250) | 4.8442 / 300.38 | 7.0879 / 307.94 | 2.5352 / 286.98 | 9.4976 / 306.41 | 通过 |
| [V220](GEMM_源码与构建索引.md#v220) | 5.3239 / 273.31 | 7.8744 / 277.18 | 2.7950 / 260.30 | 10.5309 / 276.35 | 通过 |
| [V251](GEMM_源码与构建索引.md#v251) | 5.0007 / 290.98 | 7.3627 / 296.44 | 2.6223 / 277.44 | 9.8355 / 295.88 | 通过 |
| [V252](GEMM_源码与构建索引.md#v252) | 5.1992 / 279.87 | 7.7077 / 283.17 | 2.7371 / 265.81 | 10.3146 / 282.14 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V250](GEMM_源码与构建索引.md#v250) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V220](GEMM_源码与构建索引.md#v220) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V251](GEMM_源码与构建索引.md#v251) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_reg768`：本索引首次出现该哈希。
- [V252](GEMM_源码与构建索引.md#v252) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_endwait_base64_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_cl_native_b64_single_rebase/results.json>)；[environment.json](<../results/stage_cl_native_b64_single_rebase/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-092"></a>
## 092 / CM：K64 rebase 配套调度

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_cm_native_b64_rebase_schedule`；时间：2026-10-13T19:58:04.395491+08:00。

**改了什么及结论：** 重测phase0、phase1点4/12及两A基地址。两基地址VGPR244→240并略改善，其余回退；不能把家族内收益当作超越主线。

**源码位置：** [phase_k64_native.hpp](<../phase_k64_native.hpp>)；[rolephase.cpp](<../rolephase.cpp>)；[raw_b_base64.hpp](<../raw_b_base64.hpp>)。

历史算法/同步/正确性细节：[原记录§46](<../LDS_WRAP_AND_PIPELINE.md:604>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4059 / 330.26 | 6.6566 / 327.89 | 2.1879 / 332.52 | 8.8819 / 327.65 | 库对照 |
| rocblas_mm_out | 4.4027 / 330.50 | 6.6456 / 328.43 | 2.1844 / 333.07 | 8.8793 / 327.75 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8672 / 298.96 | 7.0872 / 307.97 | 2.5459 / 285.77 | 9.4945 / 306.51 | 通过 |
| [V250](GEMM_源码与构建索引.md#v250) | 4.8487 / 300.10 | 7.0885 / 307.91 | 2.5343 / 287.07 | 9.4950 / 306.50 | 通过 |
| [V251](GEMM_源码与构建索引.md#v251) | 5.0013 / 290.94 | 7.3602 / 296.55 | 2.6205 / 277.64 | 9.8331 / 295.96 | 通过 |
| [V253](GEMM_源码与构建索引.md#v253) | 6.2485 / 232.87 | 9.0940 / 240.01 | 3.1761 / 229.07 | 12.1497 / 239.53 | 通过 |
| [V254](GEMM_源码与构建索引.md#v254) | 5.0663 / 287.21 | 7.4657 / 292.36 | 2.6508 / 274.46 | 9.9850 / 291.46 | 通过 |
| [V255](GEMM_源码与构建索引.md#v255) | 5.0432 / 288.53 | 7.3985 / 295.01 | 2.6434 / 275.23 | 9.9032 / 293.86 | 通过 |
| [V256](GEMM_源码与构建索引.md#v256) | 4.9543 / 293.70 | 7.2712 / 300.17 | 2.6065 / 279.12 | 9.7341 / 298.97 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V250](GEMM_源码与构建索引.md#v250) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V251](GEMM_源码与构建索引.md#v251) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V253](GEMM_源码与构建索引.md#v253) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p0_late_base64_reg768`：本索引首次出现该哈希。
- [V254](GEMM_源码与构建索引.md#v254) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_np4_reg768`：本索引首次出现该哈希。
- [V255](GEMM_源码与构建索引.md#v255) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_np12_reg768`：本索引首次出现该哈希。
- [V256](GEMM_源码与构建索引.md#v256) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_cm_native_b64_rebase_schedule/results.json>)；[environment.json](<../results/stage_cm_native_b64_rebase_schedule/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-093"></a>
## 093 / CN：K64 B 共享 startup

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_cn_native_b64_shared_startup`；时间：2026-10-13T20:02:31.968736+08:00。

**改了什么及结论：** 将初始B/startup提到wave组分支前，减少静态重复代码；动态每wave仍经历两startup barrier。实际VGPR升到256并有spill，性能回退。

**源码位置：** [phase_k64_prepare.hpp](<../phase_k64_prepare.hpp>)；[phase_k64_native.hpp](<../phase_k64_native.hpp>)；[rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§46](<../LDS_WRAP_AND_PIPELINE.md:604>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4537 / 326.72 | 6.6517 / 328.13 | 2.1878 / 332.54 | 8.9180 / 326.32 | 库对照 |
| rocblas_mm_out | 4.4475 / 327.17 | 6.6353 / 328.94 | 2.1852 / 332.94 | 8.8927 / 327.25 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8749 / 298.48 | 7.0892 / 307.88 | 2.5500 / 285.31 | 9.4938 / 306.53 | 通过 |
| [V250](GEMM_源码与构建索引.md#v250) | 4.8494 / 300.05 | 7.0876 / 307.95 | 2.5384 / 286.62 | 9.4912 / 306.62 | 通过 |
| [V251](GEMM_源码与构建索引.md#v251) | 5.0012 / 290.95 | 7.3632 / 296.42 | 2.6226 / 277.42 | 9.8306 / 296.03 | 通过 |
| [V256](GEMM_源码与构建索引.md#v256) | 4.9580 / 293.48 | 7.2743 / 300.05 | 2.6063 / 279.15 | 9.7308 / 299.07 | 通过 |
| [V257](GEMM_源码与构建索引.md#v257) | 5.0488 / 288.20 | 7.4478 / 293.06 | 2.6420 / 275.38 | 9.9668 / 291.99 | 通过 |
| [V258](GEMM_源码与构建索引.md#v258) | 5.6019 / 259.75 | 8.2718 / 263.87 | 2.9301 / 248.30 | 11.0641 / 263.03 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V250](GEMM_源码与构建索引.md#v250) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V251](GEMM_源码与构建索引.md#v251) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V256](GEMM_源码与构建索引.md#v256) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V257](GEMM_源码与构建索引.md#v257) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_bhoist_reg768`：本索引首次出现该哈希。
- [V258](GEMM_源码与构建索引.md#v258) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_bhoist_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_cn_native_b64_shared_startup/results.json>)；[environment.json](<../results/stage_cn_native_b64_shared_startup/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-094"></a>
## 094 / CO：只共享不可变 B 初始值

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_co_native_b64_immutable_initial`；时间：2026-10-13T20:12:09.694980+08:00。

**改了什么及结论：** 只把initial payload提到分支外，future bank仍留消费body内，尝试缩短可变寄存器寿命。仍有spill和退化，未采用。

**源码位置：** [phase_k64_prepare.hpp](<../phase_k64_prepare.hpp>)；[phase_k64_native.hpp](<../phase_k64_native.hpp>)；[rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§46](<../LDS_WRAP_AND_PIPELINE.md:604>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4150 / 329.58 | 6.6564 / 327.90 | 2.1905 / 332.14 | 8.9117 / 326.56 | 库对照 |
| rocblas_mm_out | 4.4293 / 328.51 | 6.6487 / 328.28 | 2.1887 / 332.41 | 8.8779 / 327.80 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8641 / 299.15 | 7.0952 / 307.62 | 2.5462 / 285.74 | 9.4914 / 306.61 | 通过 |
| [V250](GEMM_源码与构建索引.md#v250) | 4.8422 / 300.50 | 7.0960 / 307.59 | 2.5353 / 286.97 | 9.4893 / 306.68 | 通过 |
| [V256](GEMM_源码与构建索引.md#v256) | 4.9524 / 293.82 | 7.2696 / 300.24 | 2.6052 / 279.27 | 9.7299 / 299.10 | 通过 |
| [V259](GEMM_源码与构建索引.md#v259) | 4.9957 / 291.27 | 7.3677 / 296.24 | 2.6210 / 277.59 | 9.8237 / 296.24 | 通过 |
| [V260](GEMM_源码与构建索引.md#v260) | 5.5941 / 260.11 | 8.2706 / 263.90 | 2.9262 / 248.63 | 11.0659 / 262.99 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V250](GEMM_源码与构建索引.md#v250) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V256](GEMM_源码与构建索引.md#v256) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V259](GEMM_源码与构建索引.md#v259) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_binit_reg768`：本索引首次出现该哈希。
- [V260](GEMM_源码与构建索引.md#v260) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_binit_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_co_native_b64_immutable_initial/results.json>)；[environment.json](<../results/stage_co_native_b64_immutable_initial/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-095"></a>
## 095 / CP：K64 按相位局部等待

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_cp_native_b64_phase_waits`；时间：2026-10-13T20:24:04.052048+08:00。

**改了什么及结论：** 按旧issue序列phase0用vmcnt2、phase1在更新future后用vmcnt4，完成所需旧片段并允许其余在途；startup/terminal仍VM0。最好281–302T仍慢于raw K32；CL–CP共12候选广审计均通过。

**源码位置：** [phase_k64_native.hpp](<../phase_k64_native.hpp>)。

历史算法/同步/正确性细节：[原记录§46](<../LDS_WRAP_AND_PIPELINE.md:604>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4746 / 325.19 | 6.6568 / 327.88 | 2.1900 / 332.21 | 8.8899 / 327.36 | 库对照 |
| rocblas_mm_out | 4.4684 / 325.64 | 6.6504 / 328.20 | 2.1870 / 332.67 | 8.9093 / 326.64 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8753 / 298.46 | 7.0886 / 307.91 | 2.5476 / 285.58 | 9.4964 / 306.45 | 通过 |
| [V250](GEMM_源码与构建索引.md#v250) | 4.8471 / 300.20 | 7.0896 / 307.86 | 2.5381 / 286.65 | 9.4951 / 306.49 | 通过 |
| [V251](GEMM_源码与构建索引.md#v251) | 5.0034 / 290.82 | 7.3628 / 296.44 | 2.6208 / 277.61 | 9.8344 / 295.92 | 通过 |
| [V256](GEMM_源码与构建索引.md#v256) | 4.9565 / 293.57 | 7.2752 / 300.01 | 2.6041 / 279.38 | 9.7360 / 298.91 | 通过 |
| [V261](GEMM_源码与构建索引.md#v261) | 5.0103 / 290.42 | 7.3676 / 296.25 | 2.6211 / 277.57 | 9.8452 / 295.59 | 通过 |
| [V262](GEMM_源码与构建索引.md#v262) | 4.9281 / 295.26 | 7.2391 / 301.51 | 2.5872 / 281.21 | 9.6759 / 300.76 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V250](GEMM_源码与构建索引.md#v250) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V251](GEMM_源码与构建索引.md#v251) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V256](GEMM_源码与构建索引.md#v256) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V261](GEMM_源码与构建索引.md#v261) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_pwait_reg768`：本索引首次出现该哈希。
- [V262](GEMM_源码与构建索引.md#v262) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_pwait_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_cp_native_b64_phase_waits/results.json>)；[environment.json](<../results/stage_cp_native_b64_phase_waits/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-096"></a>
## 096 / CQ：相邻 feature 的 BF16 成对输出

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_cq_paired_feature_output_builtin`；时间：2026-10-13T21:18:36.504444+08:00。

**改了什么及结论：** lane XOR1交换把相邻feature打包为dword，aligned路径store128→64，C+2B回退两short，保留RNE。DS编码/raw DPP hazard初稿失败后修复为compiler-aware路径；正确版本仍慢。

**源码位置：** [paired_output.hpp](<../paired_output.hpp>)；[rolephase.cpp](<../rolephase.cpp>)；[paired_output_probe.cpp](<../paired_output_probe.cpp>)。

历史算法/同步/正确性细节：[原记录§47](<../LDS_WRAP_AND_PIPELINE.md:616>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4489 / 327.06 | 6.6542 / 328.01 | 2.1852 / 332.94 | 8.8745 / 327.93 | 库对照 |
| rocblas_mm_out | 4.4466 / 327.24 | 6.6359 / 328.91 | 2.1825 / 333.36 | 8.8723 / 328.01 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8713 / 298.70 | 7.0910 / 307.80 | 2.5479 / 285.54 | 9.4976 / 306.41 | 通过 |
| [V250](GEMM_源码与构建索引.md#v250) | 4.8441 / 300.39 | 7.0891 / 307.89 | 2.5383 / 286.63 | 9.4941 / 306.52 | 通过 |
| [V263](GEMM_源码与构建索引.md#v263) | 4.9102 / 296.34 | 7.1701 / 304.41 | 2.5868 / 281.25 | 9.5538 / 304.61 | 通过 |
| [V264](GEMM_源码与构建索引.md#v264) | 4.9453 / 294.24 | 7.2360 / 301.63 | 2.6260 / 277.06 | 9.6384 / 301.94 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V250](GEMM_源码与构建索引.md#v250) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V263](GEMM_源码与构建索引.md#v263) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pairout1_reg768`：本索引首次出现该哈希。
- [V264](GEMM_源码与构建索引.md#v264) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pairout2_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_cq_paired_feature_output_builtin/results.json>)；[environment.json](<../results/stage_cq_paired_feature_output_builtin/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-097"></a>
## 097 / CR：对齐分支提到整个 epilogue 外

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_cr_paired_output_alignment_hoist`；时间：2026-10-13T21:27:44.936962+08:00。

**改了什么及结论：** 不再每个输出重复对齐选择，两套互斥输出body在循环外选择。静态branch141→15，但VGPR225→229及调度成本增加，无净收益。

**源码位置：** [paired_output.hpp](<../paired_output.hpp>)；[rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§47](<../LDS_WRAP_AND_PIPELINE.md:616>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4260 / 328.76 | 6.6485 / 328.29 | 2.1906 / 332.12 | 8.8956 / 327.15 | 库对照 |
| rocblas_mm_out | 4.4266 / 328.71 | 6.6574 / 327.85 | 2.1878 / 332.54 | 8.8873 / 327.45 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8654 / 299.07 | 7.0890 / 307.89 | 2.5460 / 285.76 | 9.4913 / 306.62 | 通过 |
| [V250](GEMM_源码与构建索引.md#v250) | 4.8473 / 300.19 | 7.0900 / 307.85 | 2.5356 / 286.93 | 9.4920 / 306.59 | 通过 |
| [V263](GEMM_源码与构建索引.md#v263) | 4.9046 / 296.68 | 7.1675 / 304.52 | 2.5840 / 281.55 | 9.5692 / 304.12 | 通过 |
| [V265](GEMM_源码与构建索引.md#v265) | 4.8861 / 297.80 | 7.1803 / 303.98 | 2.5644 / 283.71 | 9.6002 / 303.14 | 通过 |
| [V266](GEMM_源码与构建索引.md#v266) | 4.9321 / 295.02 | 7.2323 / 301.79 | 2.5963 / 280.22 | 9.6526 / 301.49 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V250](GEMM_源码与构建索引.md#v250) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V263](GEMM_源码与构建索引.md#v263) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pairout1_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V265](GEMM_源码与构建索引.md#v265) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pairout3_reg768`：本索引首次出现该哈希。
- [V266](GEMM_源码与构建索引.md#v266) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pairout4_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_cr_paired_output_alignment_hoist/results.json>)；[environment.json](<../results/stage_cr_paired_output_alignment_hoist/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-098"></a>
## 098 / CS：feature tile256→128

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_cs_half_feature_tile`；时间：2026-10-13T21:43:17.809618+08:00。

**改了什么及结论：** 8compute+4producer不变，MT16→8、NT2，累加器128→64，LDS64→32KiB、VGPR225→129。每wave数学量减少、CTA翻倍，只175–181T；xprof支持控制/等待摊销变差。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)；[test_feature_tile.py](<../test_feature_tile.py>)。

历史算法/同步/正确性细节：[原记录§48](<../LDS_WRAP_AND_PIPELINE.md:622>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4546 / 326.65 | 6.6477 / 328.33 | 2.1882 / 332.48 | 8.9014 / 326.94 | 库对照 |
| rocblas_mm_out | 4.4541 / 326.69 | 6.6628 / 327.58 | 2.1864 / 332.76 | 8.8686 / 328.14 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8714 / 298.70 | 7.0918 / 307.77 | 2.5474 / 285.60 | 9.4935 / 306.54 | 通过 |
| [V250](GEMM_源码与构建索引.md#v250) | 4.8503 / 300.00 | 7.0936 / 307.69 | 2.5381 / 286.65 | 9.4939 / 306.53 | 通过 |
| [V267](GEMM_源码与构建索引.md#v267) | 8.1422 / 178.71 | 12.1114 / 180.21 | 4.1677 / 174.57 | 16.1540 / 180.15 | 通过 |
| [V268](GEMM_源码与构建索引.md#v268) | 8.1310 / 178.96 | 12.0790 / 180.70 | 4.1623 / 174.79 | 16.1063 / 180.69 | 通过 |

**xprof / DTK 表现：** f128点6相对22ef：waves2倍、SALU1.799倍、VMEM读1.501倍、VM等待3.061倍、barrier2.414倍；VGPR/LDS虽减半却更慢。 证据P07：[results/profile_cs_feature_tile_compare_20261013/ANALYSIS.md](<../results/profile_cs_feature_tile_compare_20261013/ANALYSIS.md>)。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V250](GEMM_源码与构建索引.md#v250) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V267](GEMM_源码与构建索引.md#v267) `rolephase_m1_bank_asmb_always_mid4_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_reg768`：本索引首次出现该哈希。
- [V268](GEMM_源码与构建索引.md#v268) `rolephase_m1_bank_asmb_always_mid6_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_cs_half_feature_tile/results.json>)；[environment.json](<../results/stage_cs_half_feature_tile/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-099"></a>
## 099 / CT：小 tile 改配4+2 wave

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_ct_half_tile_six_waves`；时间：2026-10-13T22:02:32.653965+08:00。

**改了什么及结论：** 128×256配4compute+2producer，NT4/MT8使每lane恢复128个accumulator，producer两次覆盖逻辑row/wrap组。较CS略恢复到180–185T，仍不如原tile。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)；[test_six_wave_tile.py](<../test_six_wave_tile.py>)。

历史算法/同步/正确性细节：[原记录§49](<../LDS_WRAP_AND_PIPELINE.md:630>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4340 / 328.17 | 6.6303 / 329.19 | 2.1899 / 332.22 | 8.8787 / 327.77 | 库对照 |
| rocblas_mm_out | 4.4270 / 328.69 | 6.6290 / 329.26 | 2.1875 / 332.60 | 8.8767 / 327.84 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8680 / 298.91 | 7.0907 / 307.82 | 2.5458 / 285.78 | 9.4929 / 306.56 | 通过 |
| [V250](GEMM_源码与构建索引.md#v250) | 4.8513 / 299.94 | 7.0940 / 307.67 | 2.5348 / 287.03 | 9.4897 / 306.67 | 通过 |
| [V268](GEMM_源码与构建索引.md#v268) | 8.1213 / 179.17 | 12.0779 / 180.71 | 4.1601 / 174.89 | 16.1074 / 180.67 | 通过 |
| [V269](GEMM_源码与构建索引.md#v269) | 7.8899 / 184.42 | 11.7690 / 185.46 | 4.0449 / 179.87 | 15.6930 / 185.44 | 通过 |
| [V270](GEMM_源码与构建索引.md#v270) | 7.8887 / 184.45 | 11.7668 / 185.49 | 4.0433 / 179.94 | 15.6870 / 185.52 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V250](GEMM_源码与构建索引.md#v250) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V268](GEMM_源码与构建索引.md#v268) `rolephase_m1_bank_asmb_always_mid6_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V269](GEMM_源码与构建索引.md#v269) `rolephase_m1_bank_asmb_always_mid4_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_cw4_pw2_sf2_reg768`：本索引首次出现该哈希。
- [V270](GEMM_源码与构建索引.md#v270) `rolephase_m1_bank_asmb_always_mid6_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_cw4_pw2_sf2_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_ct_half_tile_six_waves/results.json>)；[environment.json](<../results/stage_ct_half_tile_six_waves/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-100"></a>
## 100 / CU：原 tile 仅减至2 producer

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_cu_original_tile_two_producers`；时间：2026-10-13T22:08:25.751257+08:00。

**改了什么及结论：** 256×256配8compute+2producer（640线程），同64KiB LDS与B加载量；对照原producer循环和pairs+A soffset。多数shape变慢，out小收益未独立确认。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)；[test_ten_wave_tile.py](<../test_ten_wave_tile.py>)。

历史算法/同步/正确性细节：[原记录§49](<../LDS_WRAP_AND_PIPELINE.md:630>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4470 / 327.21 | 6.6387 / 328.77 | 2.1880 / 332.51 | 8.8848 / 327.55 | 库对照 |
| rocblas_mm_out | 4.4446 / 327.38 | 6.6326 / 329.07 | 2.1861 / 332.80 | 8.8864 / 327.49 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8707 / 298.74 | 7.0882 / 307.92 | 2.5477 / 285.57 | 9.4930 / 306.56 | 通过 |
| [V250](GEMM_源码与构建索引.md#v250) | 4.8543 / 299.75 | 7.0882 / 307.92 | 2.5377 / 286.70 | 9.4945 / 306.51 | 通过 |
| [V271](GEMM_源码与构建索引.md#v271) | 5.0212 / 289.79 | 7.2470 / 301.18 | 2.5837 / 281.59 | 9.7090 / 299.74 | 通过 |
| [V272](GEMM_源码与构建索引.md#v272) | 5.1419 / 282.99 | 7.2567 / 300.77 | 2.5326 / 287.27 | 9.7456 / 298.62 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V250](GEMM_源码与构建索引.md#v250) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V271](GEMM_源码与构建索引.md#v271) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_cw8_pw2_reg768`：本索引首次出现该哈希。
- [V272](GEMM_源码与构建索引.md#v272) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair1_asoff_cw8_pw2_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_cu_original_tile_two_producers/results.json>)；[environment.json](<../results/stage_cu_original_tile_two_producers/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-101"></a>
## 101 / CV：激活 B 的 K scalar offset

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_cv_activation_scalar_offset`；时间：2026-10-13T22:18:40.866102+08:00。

**改了什么及结论：** B保留resource pointer，token全部放checked voffset，公共K32位移入soffset；因soffset不参与bounds，terminal将bound设0。搬运360组/720次及GEMM审计通过，但耗时增0.2%–0.7%。

**源码位置：** [async_b_soffset.hpp](<../async_b_soffset.hpp>)；[rolephase.cpp](<../rolephase.cpp>)；[b_soffset_probe.cpp](<../b_soffset_probe.cpp>)。

历史算法/同步/正确性细节：[原记录§50](<../LDS_WRAP_AND_PIPELINE.md:636>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4596 / 326.28 | 6.6493 / 328.25 | 2.1869 / 332.69 | 8.8974 / 327.08 | 库对照 |
| rocblas_mm_out | 4.4530 / 326.77 | 6.6584 / 327.80 | 2.1834 / 333.22 | 8.9083 / 326.68 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8692 / 298.83 | 7.0910 / 307.80 | 2.5489 / 285.43 | 9.4947 / 306.51 | 通过 |
| [V250](GEMM_源码与构建索引.md#v250) | 4.8504 / 299.99 | 7.0859 / 308.02 | 2.5385 / 286.60 | 9.4953 / 306.49 | 通过 |
| [V273](GEMM_源码与构建索引.md#v273) | 4.8992 / 297.00 | 7.1290 / 306.16 | 2.5625 / 283.92 | 9.5351 / 305.21 | 通过 |
| [V274](GEMM_源码与构建索引.md#v274) | 4.8790 / 298.23 | 7.1425 / 305.59 | 2.5556 / 284.68 | 9.5503 / 304.72 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V250](GEMM_源码与构建索引.md#v250) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V273](GEMM_源码与构建索引.md#v273) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bsoff_reg768`：本索引首次出现该哈希。
- [V274](GEMM_源码与构建索引.md#v274) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bsoff_ppair1_asoff_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_cv_activation_scalar_offset/results.json>)；[environment.json](<../results/stage_cv_activation_scalar_offset/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-102"></a>
## 102 / CW：last-use 后提前 phase1 A 读取

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_cw_phase1_a_streaming`；时间：2026-10-13T22:30:40.375661+08:00。

**改了什么及结论：** phase0某fragment最后一次MMAC完成即复用av读取phase1，比较逐个/每4个发射；保护当前LDS slot至ACK1。DS与MMAC静态交错0→64但VGPR225→241，吞吐274–281T退化。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)；[test_phase1_a_stream.py](<../test_phase1_a_stream.py>)。

历史算法/同步/正确性细节：[原记录§51](<../LDS_WRAP_AND_PIPELINE.md:642>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4628 / 326.05 | 6.6582 / 327.81 | 2.1971 / 331.14 | 8.9098 / 326.63 | 库对照 |
| rocblas_mm_out | 4.4463 / 327.26 | 6.6562 / 327.91 | 2.1904 / 332.16 | 8.8922 / 327.27 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8716 / 298.69 | 7.0865 / 308.00 | 2.5485 / 285.48 | 9.4951 / 306.49 | 通过 |
| [V250](GEMM_源码与构建索引.md#v250) | 4.8545 / 299.74 | 7.0882 / 307.93 | 2.5386 / 286.59 | 9.4941 / 306.52 | 通过 |
| [V275](GEMM_源码与构建索引.md#v275) | 5.2674 / 276.25 | 7.8080 / 279.54 | 2.6512 / 274.42 | 10.4147 / 279.43 | 通过 |
| [V276](GEMM_源码与构建索引.md#v276) | 5.2336 / 278.03 | 7.7967 / 279.94 | 2.6451 / 275.05 | 10.3678 / 280.69 | 通过 |

**xprof / DTK 表现：** stream4 VM等待×1.150、barrier×1.155、LDS issue×1.254，LGKM×0.531；L2整数命中率83.33→76。更多静态交错没有改善整体性能。 证据P08：[A_PHASE1_STREAM_EXPERIMENTS_20261013.md](<../A_PHASE1_STREAM_EXPERIMENTS_20261013.md>)。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V250](GEMM_源码与构建索引.md#v250) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V275](GEMM_源码与构建索引.md#v275) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream1_reg768`：本索引首次出现该哈希。
- [V276](GEMM_源码与构建索引.md#v276) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream4_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_cw_phase1_a_streaming/results.json>)；[environment.json](<../results/stage_cw_phase1_a_streaming/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-103"></a>
## 103 / CX：缩小跨相位 A 预取窗口

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_cx_partial_a_phase1_windows`；时间：2026-10-13T22:48:07.953440+08:00。

**改了什么及结论：** 只在phase1的部分fragment窗口提前读A，比较start0/8/12、count4/8，减少CW额外活跃范围。四候选各126组正确，无净收益。

**源码位置：** [rolephase.cpp](<../rolephase.cpp>)；[test_phase1_window.py](<../test_phase1_window.py>)。

历史算法/同步/正确性细节：[原记录§52](<../LDS_WRAP_AND_PIPELINE.md:650>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4329 / 328.25 | 6.6683 / 327.31 | 2.1881 / 332.50 | 8.8808 / 327.69 | 库对照 |
| rocblas_mm_out | 4.4273 / 328.67 | 6.6579 / 327.83 | 2.1834 / 333.21 | 8.8857 / 327.51 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8647 / 299.11 | 7.0940 / 307.67 | 2.5442 / 285.96 | 9.4958 / 306.47 | 通过 |
| [V250](GEMM_源码与构建索引.md#v250) | 4.8437 / 300.41 | 7.0911 / 307.80 | 2.5348 / 287.02 | 9.4916 / 306.60 | 通过 |
| [V276](GEMM_源码与构建索引.md#v276) | 5.2310 / 278.17 | 7.7908 / 280.16 | 2.6444 / 275.12 | 10.3598 / 280.91 | 通过 |
| [V277](GEMM_源码与构建索引.md#v277) | 4.9872 / 291.76 | 7.3760 / 295.91 | 2.5730 / 282.76 | 9.8813 / 294.51 | 通过 |
| [V278](GEMM_源码与构建索引.md#v278) | 5.1266 / 283.83 | 7.6720 / 284.49 | 2.5847 / 281.48 | 10.1372 / 287.08 | 通过 |
| [V279](GEMM_源码与构建索引.md#v279) | 5.0449 / 288.43 | 7.5511 / 289.05 | 2.5663 / 283.50 | 9.9758 / 291.72 | 通过 |
| [V280](GEMM_源码与构建索引.md#v280) | 5.4054 / 269.19 | 7.8406 / 278.37 | 2.6346 / 276.15 | 10.4149 / 279.42 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V250](GEMM_源码与构建索引.md#v250) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V276](GEMM_源码与构建索引.md#v276) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream4_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V277](GEMM_源码与构建索引.md#v277) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream4_win0x4_reg768`：本索引首次出现该哈希。
- [V278](GEMM_源码与构建索引.md#v278) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream4_win0x8_reg768`：本索引首次出现该哈希。
- [V279](GEMM_源码与构建索引.md#v279) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream4_win12x4_reg768`：本索引首次出现该哈希。
- [V280](GEMM_源码与构建索引.md#v280) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream4_win8x8_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_cx_partial_a_phase1_windows/results.json>)；[environment.json](<../results/stage_cx_partial_a_phase1_windows/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-104"></a>
## 104 / CY：K64 同 row 发射顺序 / immediate

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_cy_k64_row_order_immediate`；时间：2026-10-13T23:01:40.030573+08:00。

**改了什么及结论：** 同一row的两个K32优先发射，mode2共享Vaddr以immediate0/64读取；VGPR240→237。issue顺序改变后phase0阈值必须VM1而非VM2，probe及广审计通过但仍慢1.5%–2.4%。

**源码位置：** [phase_k64_native.hpp](<../phase_k64_native.hpp>)；[raw_b_base64.hpp](<../raw_b_base64.hpp>)；[b_k64_immediate_probe.cpp](<../b_k64_immediate_probe.cpp>)。

历史算法/同步/正确性细节：[原记录§52](<../LDS_WRAP_AND_PIPELINE.md:650>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4217 / 329.08 | 6.6449 / 328.47 | 2.1880 / 332.51 | 8.8551 / 328.64 | 库对照 |
| rocblas_mm_out | 4.4185 / 329.32 | 6.6602 / 327.71 | 2.1857 / 332.87 | 8.8469 / 328.95 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8642 / 299.14 | 7.0887 / 307.90 | 2.5435 / 286.04 | 9.4985 / 306.38 | 通过 |
| [V250](GEMM_源码与构建索引.md#v250) | 4.8416 / 300.54 | 7.0893 / 307.88 | 2.5341 / 287.11 | 9.4989 / 306.37 | 通过 |
| [V256](GEMM_源码与构建索引.md#v256) | 4.9511 / 293.89 | 7.2709 / 300.19 | 2.6045 / 279.34 | 9.7342 / 298.97 | 通过 |
| [V262](GEMM_源码与构建索引.md#v262) | 4.9276 / 295.29 | 7.2363 / 301.62 | 2.5854 / 281.40 | 9.6736 / 300.84 | 通过 |
| [V281](GEMM_源码与构建索引.md#v281) | 4.9518 / 293.85 | 7.2618 / 300.56 | 2.6030 / 279.50 | 9.7300 / 299.09 | 通过 |
| [V282](GEMM_源码与构建索引.md#v282) | 4.9509 / 293.91 | 7.2580 / 300.72 | 2.6013 / 279.68 | 9.7267 / 299.20 | 通过 |
| [V283](GEMM_源码与构建索引.md#v283) | 4.9366 / 294.76 | 7.2338 / 301.73 | 2.5911 / 280.78 | 9.6816 / 300.59 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V250](GEMM_源码与构建索引.md#v250) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V256](GEMM_源码与构建索引.md#v256) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V262](GEMM_源码与构建索引.md#v262) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_pwait_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V281](GEMM_源码与构建索引.md#v281) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_brow1_reg768`：本索引首次出现该哈希。
- [V282](GEMM_源码与构建索引.md#v282) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_brow2_reg768`：本索引首次出现该哈希。
- [V283](GEMM_源码与构建索引.md#v283) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_pwait_brow2_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_cy_k64_row_order_immediate/results.json>)；[environment.json](<../results/stage_cy_k64_row_order_immediate/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-105"></a>
## 105 / CZ：K64 结构化 bit62 OFF/ON 配对

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_cz_struct64_bit62_20261014`；时间：2026-10-14T10:31:07.612410+08:00。

**改了什么及结论：** pow2 stride+row remainder、低48bit pointer每K64单次rebase，高descriptor位保持；同row两片用immediate0/64。两套等待下分别比较cache位0/1，行界限与terminal range0保留。

**源码位置：** [structured_b64.hpp](<../structured_b64.hpp>)；[phase_k64_native.hpp](<../phase_k64_native.hpp>)；[b_struct64_probe.cpp](<../b_struct64_probe.cpp>)。

历史算法/同步/正确性细节：[原记录§53](<../LDS_WRAP_AND_PIPELINE.md:654>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4589 / 326.34 | 6.6511 / 328.16 | 2.1861 / 332.80 | 8.8680 / 328.17 | 库对照 |
| rocblas_mm_out | 4.4510 / 326.92 | 6.6546 / 327.99 | 2.1831 / 333.26 | 8.8558 / 328.62 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8726 / 298.63 | 7.0839 / 308.11 | 2.5500 / 285.31 | 9.4983 / 306.39 | 通过 |
| [V250](GEMM_源码与构建索引.md#v250) | 4.8447 / 300.35 | 7.0842 / 308.10 | 2.5381 / 286.65 | 9.4985 / 306.38 | 通过 |
| [V282](GEMM_源码与构建索引.md#v282) | 4.9491 / 294.01 | 7.2580 / 300.72 | 2.6058 / 279.20 | 9.7248 / 299.25 | 通过 |
| [V283](GEMM_源码与构建索引.md#v283) | 4.9333 / 294.95 | 7.2341 / 301.72 | 2.5939 / 280.48 | 9.6814 / 300.59 | 通过 |
| [V284](GEMM_源码与构建索引.md#v284) | 5.5096 / 264.10 | 8.1254 / 268.62 | 2.8666 / 253.80 | 10.8486 / 268.25 | 通过 |
| [V285](GEMM_源码与构建索引.md#v285) | 5.0048 / 290.74 | 7.4000 / 294.95 | 2.4906 / 292.11 | 9.8916 / 294.21 | 通过 |
| [V286](GEMM_源码与构建索引.md#v286) | 5.5136 / 263.91 | 8.1421 / 268.07 | 2.8713 / 253.38 | 10.8748 / 267.61 | 通过 |
| [V287](GEMM_源码与构建索引.md#v287) | 4.8974 / 297.11 | 7.4193 / 294.18 | 2.4943 / 291.69 | 9.8991 / 293.98 | 通过 |

**xprof / DTK 表现：** 后续out_proj结构化OFF/ON/原版/库对照；ON/OFF VM完成等待×1.030，VM issue×0.642、barrier×0.632。 证据P09：[results/profile_struct64_out_proj_20261014/ANALYSIS.md](<../results/profile_struct64_out_proj_20261014/ANALYSIS.md>)。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V250](GEMM_源码与构建索引.md#v250) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V282](GEMM_源码与构建索引.md#v282) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_brow2_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V283](GEMM_源码与构建索引.md#v283) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_pwait_brow2_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V284](GEMM_源码与构建索引.md#v284) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_brow2_sb64c0_reg768`：本索引首次出现该哈希。
- [V285](GEMM_源码与构建索引.md#v285) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_brow2_sb64c1_reg768`：本索引首次出现该哈希。
- [V286](GEMM_源码与构建索引.md#v286) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_pwait_brow2_sb64c0_reg768`：本索引首次出现该哈希。
- [V287](GEMM_源码与构建索引.md#v287) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_pwait_brow2_sb64c1_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_cz_struct64_bit62_20261014/results.json>)；[environment.json](<../results/stage_cz_struct64_bit62_20261014/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-106"></a>
## 106 / DA：K64 Cache Swizzle 独立复测

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_da_struct64_independent_20261014`；时间：2026-10-14T10:34:58.434736+08:00。

**改了什么及结论：** 独立计时与C+2B审计确认ON相对结构化OFF更快，但与原raw相比仅out_proj获益；其他shape仍不能全局采用。

**源码位置：** [structured_b64.hpp](<../structured_b64.hpp>)；[phase_k64_native.hpp](<../phase_k64_native.hpp>)。

历史算法/同步/正确性细节：[原记录§53](<../LDS_WRAP_AND_PIPELINE.md:654>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4619 / 326.12 | 6.6571 / 327.86 | 2.1855 / 332.90 | 8.8806 / 327.70 | 库对照 |
| rocblas_mm_out | 4.4621 / 326.10 | 6.6532 / 328.06 | 2.1817 / 333.48 | 8.8871 / 327.46 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8702 / 298.77 | 7.0892 / 307.88 | 2.5464 / 285.72 | 9.4936 / 306.54 | 通过 |
| [V250](GEMM_源码与构建索引.md#v250) | 4.8446 / 300.35 | 7.0866 / 307.99 | 2.5380 / 286.66 | 9.4917 / 306.60 | 通过 |
| [V282](GEMM_源码与构建索引.md#v282) | 4.9480 / 294.08 | 7.2553 / 300.83 | 2.6040 / 279.40 | 9.7205 / 299.39 | 通过 |
| [V283](GEMM_源码与构建索引.md#v283) | 4.9321 / 295.03 | 7.2313 / 301.83 | 2.5921 / 280.68 | 9.6781 / 300.70 | 通过 |
| [V285](GEMM_源码与构建索引.md#v285) | 4.9881 / 291.71 | 7.4105 / 294.53 | 2.4914 / 292.02 | 9.9014 / 293.92 | 通过 |
| [V287](GEMM_源码与构建索引.md#v287) | 4.8996 / 296.98 | 7.4096 / 294.57 | 2.4939 / 291.73 | 9.8871 / 294.34 | 通过 |

**xprof / DTK 表现：** 对应K64 cache候选的out_proj采集，不把它冒充其他shape或K32 cache的counter。 证据P09：[results/profile_struct64_out_proj_20261014/ANALYSIS.md](<../results/profile_struct64_out_proj_20261014/ANALYSIS.md>)。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V250](GEMM_源码与构建索引.md#v250) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V282](GEMM_源码与构建索引.md#v282) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_brow2_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V283](GEMM_源码与构建索引.md#v283) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_pwait_brow2_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V285](GEMM_源码与构建索引.md#v285) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_brow2_sb64c1_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V287](GEMM_源码与构建索引.md#v287) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_pwait_brow2_sb64c1_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

原始证据：[原始 results.json](<../results/stage_da_struct64_independent_20261014/results.json>)；[environment.json](<../results/stage_da_struct64_independent_20261014/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-107"></a>
## 107 / DB：K64 cache 仅用于 out_proj

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_db_cache_shape_dispatch_20261014`；时间：2026-10-14T10:39:40.332804+08:00。

**改了什么及结论：** cache_dispatch.cpp三路：out用K64cache1/wait0，FC2用producer pairs+A soffset，其余原raw。统一入口正常/偏移逐位验证。

**源码位置：** [cache_dispatch.cpp](<../cache_dispatch.cpp>)；[rolephase.cpp](<../rolephase.cpp>)；[structured_b64.hpp](<../structured_b64.hpp>)。

历史算法/同步/正确性细节：[原记录§53](<../LDS_WRAP_AND_PIPELINE.md:654>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4548 / 326.63 | 6.6455 / 328.44 | 2.1877 / 332.57 | 8.8805 / 327.70 | 库对照 |
| rocblas_mm_out | 4.4510 / 326.92 | 6.6451 / 328.46 | 2.1849 / 332.98 | 8.8602 / 328.46 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8717 / 298.68 | 7.0989 / 307.46 | 2.5461 / 285.75 | 9.5057 / 306.15 | 通过 |
| [V250](GEMM_源码与构建索引.md#v250) | 4.8452 / 300.31 | 7.0867 / 307.99 | 2.5386 / 286.59 | 9.5046 / 306.19 | 通过 |
| [V288](GEMM_源码与构建索引.md#v288) | 4.8452 / 300.31 | 7.0895 / 307.87 | 2.4916 / 292.00 | 9.5043 / 306.19 | 通过 |
| [V285](GEMM_源码与构建索引.md#v285) | 4.9823 / 292.05 | 7.4067 / 294.68 | 2.4922 / 291.93 | 9.9052 / 293.80 | 通过 |

**xprof / DTK 表现：** 分派out_proj走已采集K64 cache路径；分派库自身全shape另轮counter未单独登记。 证据P09：[results/profile_struct64_out_proj_20261014/ANALYSIS.md](<../results/profile_struct64_out_proj_20261014/ANALYSIS.md>)。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V250](GEMM_源码与构建索引.md#v250) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V288](GEMM_源码与构建索引.md#v288) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_cachedispatch_reg768`：本索引首次出现该哈希。
- [V285](GEMM_源码与构建索引.md#v285) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_brow2_sb64c1_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

原始证据：[原始 results.json](<../results/stage_db_cache_shape_dispatch_20261014/results.json>)；[environment.json](<../results/stage_db_cache_shape_dispatch_20261014/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-108"></a>
## 108 / DC：K64 cache 分派独立复测

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_dc_cache_dispatch_independent_20261014`；时间：2026-10-14T10:42:54.748468+08:00。

**改了什么及结论：** out约291.86T，相对原raw耗时降2.08%，其他与旧可选基本持平；保留de1c可选库。counter不能把此收益简单解释为VM完成等待下降。

**源码位置：** [cache_dispatch.cpp](<../cache_dispatch.cpp>)；[rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§53](<../LDS_WRAP_AND_PIPELINE.md:654>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4200 / 329.21 | 6.6447 / 328.48 | 2.1880 / 332.51 | 8.9070 / 326.73 | 库对照 |
| rocblas_mm_out | 4.4234 / 328.95 | 6.6434 / 328.54 | 2.1842 / 333.09 | 8.8922 / 327.27 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8644 / 299.13 | 7.0926 / 307.74 | 2.5458 / 285.78 | 9.4947 / 306.51 | 通过 |
| [V250](GEMM_源码与构建索引.md#v250) | 4.8425 / 300.48 | 7.0918 / 307.77 | 2.5362 / 286.87 | 9.4971 / 306.43 | 通过 |
| [V288](GEMM_源码与构建索引.md#v288) | 4.8448 / 300.34 | 7.0987 / 307.47 | 2.4928 / 291.86 | 9.4968 / 306.44 | 通过 |

**xprof / DTK 表现：** 同上；本轮独立计时与K64 out_proj counter分开解释。 证据P09：[results/profile_struct64_out_proj_20261014/ANALYSIS.md](<../results/profile_struct64_out_proj_20261014/ANALYSIS.md>)。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V250](GEMM_源码与构建索引.md#v250) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V288](GEMM_源码与构建索引.md#v288) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_cachedispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

原始证据：[原始 results.json](<../results/stage_dc_cache_dispatch_independent_20261014/results.json>)；[environment.json](<../results/stage_dc_cache_dispatch_independent_20261014/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-109"></a>
## 109 / DD：把结构化 cache 搭配快的 K32 流水

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_dd_struct32_cache_pair_20261014`；时间：2026-10-14T11:17:52.658472+08:00。

**改了什么及结论：** 新增structured_b32.hpp，common K32偏移更新descriptor低48位pointer，stride按pow2拆分、logical row保护、terminal range0且最后current消费前VM0。比较OFF/ON各自加/不加producer，共四配置。

**源码位置：** [structured_b32.hpp](<../structured_b32.hpp>)；[rolephase.cpp](<../rolephase.cpp>)；[b_struct32_probe.cpp](<../b_struct32_probe.cpp>)。

历史算法/同步/正确性细节：[原记录§54](<../LDS_WRAP_AND_PIPELINE.md:664>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4172 / 329.41 | 6.6460 / 328.41 | 2.1901 / 332.20 | 8.8914 / 327.30 | 库对照 |
| rocblas_mm_out | 4.4117 / 329.82 | 6.6309 / 329.16 | 2.1861 / 332.80 | 8.8942 / 327.20 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8664 / 299.00 | 7.0932 / 307.71 | 2.5444 / 285.93 | 9.4943 / 306.52 | 通过 |
| [V250](GEMM_源码与构建索引.md#v250) | 4.8399 / 300.64 | 7.0922 / 307.75 | 2.5345 / 287.06 | 9.4966 / 306.44 | 通过 |
| [V288](GEMM_源码与构建索引.md#v288) | 4.8421 / 300.51 | 7.0925 / 307.74 | 2.4898 / 292.21 | 9.4941 / 306.52 | 通过 |
| [V289](GEMM_源码与构建索引.md#v289) | 4.8911 / 297.50 | 7.1414 / 305.63 | 2.5651 / 283.63 | 9.5501 / 304.73 | 通过 |
| [V290](GEMM_源码与构建索引.md#v290) | 4.4359 / 328.03 | 6.5355 / 333.97 | 2.2654 / 321.16 | 8.7960 / 330.85 | 通过 |
| [V291](GEMM_源码与构建索引.md#v291) | 4.9053 / 296.64 | 7.1240 / 306.38 | 2.5467 / 285.68 | 9.5345 / 305.23 | 通过 |
| [V292](GEMM_源码与构建索引.md#v292) | 4.3802 / 332.20 | 6.5680 / 332.31 | 2.2064 / 329.75 | 8.8165 / 330.08 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V250](GEMM_源码与构建索引.md#v250) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V288](GEMM_源码与构建索引.md#v288) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_cachedispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V289](GEMM_源码与构建索引.md#v289) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32c0_reg768`：本索引首次出现该哈希。
- [V290](GEMM_源码与构建索引.md#v290) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32c1_reg768`：本索引首次出现该哈希。
- [V291](GEMM_源码与构建索引.md#v291) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32c0_ppair1_asoff_reg768`：本索引首次出现该哈希。
- [V292](GEMM_源码与构建索引.md#v292) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32c1_ppair1_asoff_reg768`：本索引首次出现该哈希。

原始证据：[原始 results.json](<../results/stage_dd_struct32_cache_pair_20261014/results.json>)；[environment.json](<../results/stage_dd_struct32_cache_pair_20261014/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-110"></a>
## 110 / DE：K32 Cache Swizzle 独立复测

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_de_struct32_independent_20261014`；时间：2026-10-14T11:21:25.774611+08:00。

**改了什么及结论：** 对两个ON配置独立计时、C+2B各126组审计，确认四shape约328–334T；FC2/out配producer更好，QKV/FC1选plain。OFF/ON的SGPR44/38不同，不能把全部差额当纯cache物理贡献。

**源码位置：** [structured_b32.hpp](<../structured_b32.hpp>)；[rolephase.cpp](<../rolephase.cpp>)。

历史算法/同步/正确性细节：[原记录§54](<../LDS_WRAP_AND_PIPELINE.md:664>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4579 / 326.40 | 6.6492 / 328.25 | 2.1856 / 332.89 | 8.8753 / 327.90 | 库对照 |
| rocblas_mm_out | 4.4544 / 326.67 | 6.6356 / 328.93 | 2.1830 / 333.28 | 8.8666 / 328.22 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8724 / 298.64 | 7.0902 / 307.84 | 2.5468 / 285.67 | 9.4954 / 306.48 | 通过 |
| [V288](GEMM_源码与构建索引.md#v288) | 4.8508 / 299.97 | 7.0925 / 307.74 | 2.4901 / 292.18 | 9.4934 / 306.55 | 通过 |
| [V290](GEMM_源码与构建索引.md#v290) | 4.4640 / 325.96 | 6.5558 / 332.93 | 2.2667 / 320.97 | 8.7260 / 333.51 | 通过 |
| [V292](GEMM_源码与构建索引.md#v292) | 4.4405 / 327.68 | 6.5717 / 332.13 | 2.2048 / 329.99 | 8.8105 / 330.31 | 通过 |

**xprof / DTK 表现：** 本阶段未建立独立、明确归属的逐候选counter对照；这里不以相邻版本的counter代替。已有HIP-event性能、正确性和构建/ISA证据，若讨论访存/占用原因，属于实现分析而非该候选已实测的counter结论。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V288](GEMM_源码与构建索引.md#v288) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_cachedispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V290](GEMM_源码与构建索引.md#v290) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32c1_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V292](GEMM_源码与构建索引.md#v292) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32c1_ppair1_asoff_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

原始证据：[原始 results.json](<../results/stage_de_struct32_independent_20261014/results.json>)；[environment.json](<../results/stage_de_struct32_independent_20261014/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-111"></a>
## 111 / DF：最终 K32 统一入口

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_df_struct32_dispatch_20261014`；时间：2026-10-14T11:24:47.506050+08:00。

**改了什么及结论：** struct32_dispatch.cpp精确按四组N/K选择：FC2/out用cache+producer，QKV/FC1用cache，其他原支持尺寸回退raw。统一入口正常/偏移审计、特殊BF16、fallback与非法参数契约全部核对。

**源码位置：** [struct32_dispatch.cpp](<../struct32_dispatch.cpp>)；[rolephase.cpp](<../rolephase.cpp>)；[structured_b32.hpp](<../structured_b32.hpp>)；[async_a_soffset.hpp](<../async_a_soffset.hpp>)。

历史算法/同步/正确性细节：[原记录§54](<../LDS_WRAP_AND_PIPELINE.md:664>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4252 / 328.82 | 6.6731 / 327.08 | 2.1915 / 331.99 | 8.8894 / 327.38 | 库对照 |
| rocblas_mm_out | 4.4288 / 328.55 | 6.6445 / 328.49 | 2.1872 / 332.64 | 8.8651 / 328.27 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8647 / 299.11 | 7.0916 / 307.78 | 2.5447 / 285.90 | 9.5016 / 306.28 | 通过 |
| [V288](GEMM_源码与构建索引.md#v288) | 4.8417 / 300.53 | 7.0901 / 307.84 | 2.4911 / 292.06 | 9.5037 / 306.21 | 通过 |
| [V293](GEMM_源码与构建索引.md#v293) | 4.4249 / 328.84 | 6.5464 / 333.41 | 2.2074 / 329.59 | 8.7366 / 333.10 | 通过 |
| [V290](GEMM_源码与构建索引.md#v290) | 4.4799 / 324.80 | 6.5532 / 333.06 | 2.2659 / 321.09 | 8.7389 / 333.01 | 通过 |
| [V292](GEMM_源码与构建索引.md#v292) | 4.4200 / 329.21 | 6.5805 / 331.68 | 2.2083 / 329.46 | 8.8117 / 330.26 | 通过 |

**xprof / DTK 表现：** 最终统一库四shape同哈希采集。barrier/LDS issue均下降；FC2/QKV的VM完成等待仍上升，out_proj明显下降。 证据P10：[results/profile_struct32_four_20261014/ANALYSIS.md](<../results/profile_struct32_four_20261014/ANALYSIS.md>)。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V288](GEMM_源码与构建索引.md#v288) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_cachedispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V293](GEMM_源码与构建索引.md#v293) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32dispatch_reg768`：本索引首次出现该哈希。
- [V290](GEMM_源码与构建索引.md#v290) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32c1_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V292](GEMM_源码与构建索引.md#v292) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32c1_ppair1_asoff_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

原始证据：[原始 results.json](<../results/stage_df_struct32_dispatch_20261014/results.json>)；[environment.json](<../results/stage_df_struct32_dispatch_20261014/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

<a id="stage-112"></a>
## 112 / DG：最终统一入口独立复测

目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/results/stage_dg_struct32_dispatch_independent_20261014`；时间：2026-10-14T11:27:21.945447+08:00。

**改了什么及结论：** 四真实shape均329–333T，接近同轮实际F.linear/mm_out。两独立轮均通过门槛；最终四shape counter核对同库哈希，补采过滤遗漏的新kernel等待记录。

**源码位置：** [struct32_dispatch.cpp](<../struct32_dispatch.cpp>)；[rolephase.cpp](<../rolephase.cpp>)；[structured_b32.hpp](<../structured_b32.hpp>)。

历史算法/同步/正确性细节：[原记录§54](<../LDS_WRAP_AND_PIPELINE.md:664>)。原记录中的“当前/下一步”是当时状态，最终采用状态以本整理和最终报告为准。

MNK：fc2=[18880, 5376, 7168]；qkv=[18880, 10752, 5376]；out_proj=[18880, 5376, 3584]；fc1=[18880, 14336, 5376]。

| 实测库ID / 对照 | FC2 ms / T | QKV ms / T | out_proj ms / T | FC1 ms / T | 本轮数值初筛 |
|---|---:|---:|---:|---:|---|
| rocblas_linear | 4.4260 / 328.76 | 6.6487 / 328.28 | 2.1890 / 332.36 | 8.8831 / 327.61 | 库对照 |
| rocblas_mm_out | 4.4211 / 329.12 | 6.6322 / 329.09 | 2.1868 / 332.70 | 8.8852 / 327.53 | 库对照 |
| [V190](GEMM_源码与构建索引.md#v190) | 4.8641 / 299.15 | 7.0868 / 307.99 | 2.5450 / 285.87 | 9.4935 / 306.55 | 通过 |
| [V288](GEMM_源码与构建索引.md#v288) | 4.8430 / 300.45 | 7.0854 / 308.05 | 2.4911 / 292.05 | 9.4920 / 306.59 | 通过 |
| [V293](GEMM_源码与构建索引.md#v293) | 4.4209 / 329.14 | 6.5509 / 333.18 | 2.2064 / 329.74 | 8.7753 / 331.63 | 通过 |

**xprof / DTK 表现：** 同最终库P10，各shape每实现3样本；DTK旧过滤漏新kernel已补采9项，未把缺失当0。 证据P10：[results/profile_struct32_four_20261014/ANALYSIS.md](<../results/profile_struct32_four_20261014/ANALYSIS.md>)。

**候选对应关系与配置：**

- [V190](GEMM_源码与构建索引.md#v190) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V288](GEMM_源码与构建索引.md#v288) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_cachedispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。
- [V293](GEMM_源码与构建索引.md#v293) `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32dispatch_reg768`：已出现哈希，作为复测/控制或同码别名；不重复算作新优化。

原始证据：[原始 results.json](<../results/stage_dg_struct32_dispatch_independent_20261014/results.json>)；[environment.json](<../results/stage_dg_struct32_dispatch_independent_20261014/environment.json>)。本地缺少独立原始目录的阶段仍可从服务器原路径及PROGRESS.full_stages读取，本报告数值已核对服务器原始JSON。

## 未进入正式性能表的失败与辅助实验

112阶段并非112个全部成功的新算法。其内包含控制、复测、不支持shape与数值失败；另外还保留编译失败、搬运/数学探针、广审计和counter采集。这些不能混入正式GEMM吞吐榜。

| 记录 | 具体情况 | 源码/证据 |
|---|---|---|
| T / scheduler | DTK ILP与occupancy调度器编译崩溃，未执行GPU，不改变工具链 | [STAGE_T_COMPILER_FAILURE.md](<../STAGE_T_COMPILER_FAILURE.md>) |
| BR / 128 accumulator约束 | 无法为大v约束分配寄存器，后续BS改子bank；不是性能为零 | rolephase.cpp；下表冻结build.log |
| S / 初次BN512 | 对N5376不支持，修正bench显式unsupported，不偷偷改目标shape | [STAGE_S_INITIAL_FAILURE.md](<../STAGE_S_INITIAL_FAILURE.md>) |
| U / 12数学wave | B分区截断漏覆盖，数值失败，不计时 | [STAGE_U_CORRECTNESS.md](<../STAGE_U_CORRECTNESS.md>) |
| CQ / DS/DPP初稿 | DS编码错误、raw DPP延迟hazard先失败；改compiler-aware后完整审计通过，但性能仍退化 | [PAIRED_OUTPUT_EXPERIMENTS_20261013.md](<../PAIRED_OUTPUT_EXPERIMENTS_20261013.md>) |
| MMAC arithmetic probe | 403–461T只重复寄存器MMAC，没有完整GEMM搬运；不用于宣称目标已完成 | mmac_micro.cpp；results/mmac_arithmetic_probe |
| 短K曲线 | 固定M18880/N5376，K64–7168与F.linear对比；差额不是固定启动开销 | sweep_short_k.py；results/short_k_curve_bbase_20261013 |
| LDS wrap / RNE / 地址probe | 检查物理映射、BF16语义、越界与pointer/descriptor；不是性能基准 | *_probe.cpp、probe_*.py、test_struct*_address.py |

### 24个保留的失败构建记录

下列是构建失败的attempt，不是24个额外完成阶段。构建错误原文截取最多首两行，完整stderr在同目录build.log。

- `b_soffset_probe`：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/b_soffset_probe/20261013-221601-737170/build.json`。 `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/b_soffset_probe/20261013-221601-737170/async_b_soffset.hpp:12:18: error: invalid operand for instruction`
- `large_pad16_g8_ilp_reg768`：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_ilp_reg768/20261012-185046-495006/build.json`。 `dcc: error: unable to execute command: Aborted (core dumped)`; `dcc: error: clang frontend command failed due to signal (use -v to see invocation)`
- `large_pad16_g8_occupancy_reg768`：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_occupancy_reg768/20261012-185046-495340/build.json`。 `dcc: error: unable to execute command: Floating point exception (core dumped)`; `dcc: error: clang frontend command failed due to signal (use -v to see invocation)`
- `role64_double_wrap_asm_zero_reg768`：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_asm_zero_reg768/20261012-192255-002568/build.json`。 `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_asm_zero_reg768/20261012-192255-002568/role64.cpp:30:18: error: invalid operand for instruction`; `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_asm_zero_reg768/20261012-192255-002568/role64.cpp:30:18: error: invalid operand for instruction`
- `rolephase_m1_bank_asmb_always_mid4_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_reg768`：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid4_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_reg768/20261013-213922-840695/build.json`。 `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid4_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_reg768/20261013-213922-840695/rolephase.cpp:202:65: error: use of undeclared identifier 'H3_PHASE_DS_PROGRESS'`; `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid4_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_reg768/20261013-213922-840695/rolephase.cpp:378:5: error: static assertion failed due to requirement '(NT * MT * 4) % width == 0': Complete accumulator coverage`
- `rolephase_m1_bank_asmb_always_mid6_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_reg768`：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid6_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_reg768/20261013-213922-841040/build.json`。 `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid6_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_reg768/20261013-213922-841040/rolephase.cpp:202:65: error: use of undeclared identifier 'H3_PHASE_DS_PROGRESS'`; `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid6_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_reg768/20261013-213922-841040/rolephase.cpp:378:5: error: static assertion failed due to requirement '(NT * MT * 4) % width == 0': Complete accumulator coverage`
- `rolephase_m1_bank_asmb_always_mid_bufferepi_b64p0_reg768`：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_b64p0_reg768/20261013-124023-435409/build.json`。 `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_b64p0_reg768/20261013-124023-435409/phase_k64_body.hpp:5:16: error: use of undeclared identifier 'H3_PHASE_SWAP'`
- `rolephase_m1_bank_asmb_always_mid_bufferepi_b64p1_reg768`：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_b64p1_reg768/20261013-124023-435898/build.json`。 `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_b64p1_reg768/20261013-124023-435898/phase_k64_body.hpp:5:16: error: use of undeclared identifier 'H3_PHASE_SWAP'`
- `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_abase_reg768`：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_abase_reg768/20261013-133101-017488/build.json`。 `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_abase_reg768/20261013-133101-017488/rolephase.cpp:95:20: error: use of undeclared identifier 'H3_PHASE_CACHE_A'`
- `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_abase_swave_reg768`：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_abase_swave_reg768/20261013-133103-254231/build.json`。 `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_abase_swave_reg768/20261013-133103-254231/rolephase.cpp:95:20: error: use of undeclared identifier 'H3_PHASE_CACHE_A'`
- `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack1_reg768`：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack1_reg768/20261013-153725-849040/build.json`。 `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack1_reg768/20261013-153725-849040/rolephase.cpp:231:18: error: couldn't allocate output register for constraint 'v'`; `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack1_reg768/20261013-153725-849040/rolephase.cpp:231:18: error: couldn't allocate output register for constraint 'v'`
- `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack2_reg768`：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack2_reg768/20261013-153725-849468/build.json`。 `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack2_reg768/20261013-153725-849468/rolephase.cpp:231:18: error: couldn't allocate output register for constraint 'v'`; `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack2_reg768/20261013-153725-849468/rolephase.cpp:231:18: error: couldn't allocate output register for constraint 'v'`
- `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack3_reg768`：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack3_reg768/20261013-153729-730531/build.json`。 `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack3_reg768/20261013-153729-730531/rolephase.cpp:231:18: error: couldn't allocate output register for constraint 'v'`; `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack3_reg768/20261013-153729-730531/rolephase.cpp:231:18: error: couldn't allocate output register for constraint 'v'`
- `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob2_reg768`：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob2_reg768/20261013-184756-145053/build.json`。 `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob2_reg768/20261013-184756-145053/rolephase.cpp:568:27: error: no matching function for call to 'weight_fragment_immediate'`
- `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob3_reg768`：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob3_reg768/20261013-184756-281032/build.json`。 `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob3_reg768/20261013-184756-281032/rolephase.cpp:573:31: error: no matching function for call to 'weight_fragment_pair'`
- `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p0_reg768`：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p0_reg768/20261013-155929-638178/build.json`。 `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p0_reg768/20261013-155929-638178/phase_k64_native.hpp:5:36: error: use of undeclared identifier 'H3_PHASE_CACHE_B'`
- `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p0_reg768`：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p0_reg768/20261013-160120-788941/build.json`。 `/workspace/composable_kernel/include/ck_tile/core/arch/amd_buffer_addressing.hpp:1654:22: error: invalid operand for instruction`; `/workspace/composable_kernel/include/ck_tile/core/arch/amd_buffer_addressing.hpp:1654:22: error: invalid operand for instruction`
- `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p1_reg768`：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p1_reg768/20261013-155929-638605/build.json`。 `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p1_reg768/20261013-155929-638605/phase_k64_native.hpp:5:36: error: use of undeclared identifier 'H3_PHASE_CACHE_B'`
- `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p1_reg768`：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p1_reg768/20261013-160120-789461/build.json`。 `/workspace/composable_kernel/include/ck_tile/core/arch/amd_buffer_addressing.hpp:1654:22: error: invalid operand for instruction`; `/workspace/composable_kernel/include/ck_tile/core/arch/amd_buffer_addressing.hpp:1654:22: error: invalid operand for instruction`
- `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bsoff_ppair1_asoff_reg768`：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bsoff_ppair1_asoff_reg768/20261013-221604-077521/build.json`。 `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bsoff_ppair1_asoff_reg768/20261013-221604-077521/async_b_soffset.hpp:12:18: error: invalid operand for instruction`; `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bsoff_ppair1_asoff_reg768/20261013-221604-077521/async_b_soffset.hpp:12:18: error: invalid operand for instruction`
- `rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bsoff_reg768`：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bsoff_reg768/20261013-221601-737488/build.json`。 `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bsoff_reg768/20261013-221601-737488/async_b_soffset.hpp:12:18: error: invalid operand for instruction`; `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bsoff_reg768/20261013-221601-737488/async_b_soffset.hpp:12:18: error: invalid operand for instruction`
- `rolephase_m1_bank_asmb_always_mid_bufferepi_reg768`：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_reg768/20261013-115721-514679/build.json`。 `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_reg768/20261013-115721-514679/rolephase.cpp:287:21: error: no matching function for call to 'output_fragment'`
- `rolephase_m1_bank_asmb_bufferepi_reg768`：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_bufferepi_reg768/20261013-115719-253083/build.json`。 `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_bufferepi_reg768/20261013-115719-253083/rolephase.cpp:287:21: error: no matching function for call to 'output_fragment'`
- `rolephase_m1_bank_asmb_mid_bufferepi_reg768`：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_mid_bufferepi_reg768/20261013-115719-253495/build.json`。 `/workspace/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_mid_bufferepi_reg768/20261013-115719-253495/rolephase.cpp:287:21: error: no matching function for call to 'output_fragment'`