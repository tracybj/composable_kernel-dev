# 04_gemm：H3 BF16 GEMM 优化迭代记录

目标：基于03_gemm的CK Tile实现，达到与rocBLAS接近的性能，约330TFLOPS。**当前尚未达标，目标保持不变。**

源码目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm`。
本地工作副本：`D:/yyc_workspace/h3_ck_gemm_optimization_20261012/04_gemm`。
03_gemm与共享CK include未修改，所有候选放在04目录。

## 环境、计算与验证范围

ck_yyc，物理HCU4，gfx936；CK SHA `5c8ea51ed7ac03bdcc24aeb811d51acc57233b60`；DTK25.10.0-0；PyTorch2.9.0/HIP6.3.26113。独立GEMM，不加载H3网络。原始路径为相同环境下F.linear，前次API/trace调查确认其rocblas_gemm_ex入口。

四类TP2本地shape沿用之前四卡TP2+SP2代表数据，M=18944（由18880补齐）筛选，另有真实M=18880独立结果，N/K按模型配置。BF16 A/W/C，CK内部FP32累加与RNE输出；A[M,K]和连续W[N,K]，计算A@W.T。早期通用候选要求整tile，后续带pad_m及转置vecb候选已测真实18880，具体尾行支持以编译开关、API guard和实际正确性JSON为准；不把某个候选支持尾行外推为所有候选都支持。

每个候选先做完整输出检查：固定种子二进制分数数据对FP32参考转BF16要求逐元素相等；普通正态分布要求finite、逐元素误差≤0.0625+0.01×|参考|且relative L2<0.005。普通随机分布可能因累加顺序出现少量BF16舍入差异，不能称任意输入bitwise无损。

通过后每方法预热5次，6轮轮换顺序，每轮10次HIP event；使用6个轮均值中位数。CK预分配输出，同时记录F.linear和torch.mm(out)控制项。属于重复热缓存单算子基准，不代表完整H3性能或画质。

有效TFLOPS=2×M×N×K/(time_ms×10^9)，不使用未知硬件峰值计算MFU。

## 当前各shape最好记录

下面是候选筛选结果，不是对最终方案的独立复测；不应拿最快样本宣称目标达成。

正式记录的当前有效候选由validated_current_candidate字段给出：22ef3280...的双tile静态wave分组/post-priority/B标量resource base方案，独立BP复测及102组/306次审计通过，约286–308TFLOPS。下面跨轮最小值表可能包含筛选浮动，不代表已采用该候选或达到330目标。

| 投影 | 候选 | CK ms | CK TFLOPS | 同轮rocBLAS ms | 同轮rocBLAS TFLOPS |
|---|---|---:|---:|---:|---:|
| fc2 | large_pad16_g8_trans_groupn_vecb_pad_m_reg768 | 7.0398 | 207.39 | 4.4622 | 327.19 |
| qkv | large_pad16_g8_trans_groupn_aligned_pad_m_reg768 | 10.2381 | 213.91 | 6.6741 | 328.14 |
| out_proj | large_pad16_g8_trans_groupn_vecb_pad_m_reg768 | 3.5851 | 203.62 | 2.1885 | 333.57 |
| fc1 | large_pad16_g8_trans_groupn_vecb_pad_m_reg768 | 13.6807 | 213.44 | 8.8978 | 328.17 |

## 原生 M=18880 尾行验证

与上表M=18944分开计分，不把不同工作量的ms混入同一最快记录；检查输出尾部16行sentinel未被覆盖。

| 投影 | M | 候选 | CK ms | CK TFLOPS | 同轮rocBLAS ms |
|---|---:|---|---:|---:|---:|
| fc2 | 18880 | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair1_asoff_reg768 | 4.8411 | 300.57 | 4.4314 |
| qkv | 18880 | rolephase_m1_bank_asmb_always_mid_bufferepi_fpclassrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768 | 7.0811 | 308.23 | 6.6444 |
| out_proj | 18880 | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_g1_reg768 | 2.5294 | 287.63 | 2.1893 |
| fc1 | 18880 | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768 | 9.4893 | 306.68 | 8.9117 |

## 每轮FC2的筛选结果

| 轮次 | 本轮最快CK候选 | ms | TFLOPS | 备注 |
|---|---|---:|---:|---|
| stage_a_layout | base | 21.5086 | 67.88 | 保留全部候选/控制项原始JSON |
| stage_aa_k16_residency | large_pad16_g8_pad_m_reg768 | 7.7696 | 187.91 | 保留全部候选/控制项原始JSON |
| stage_ab_scheduling_controls | large_pad16_g8_pad_m_reg768 | 7.7853 | 187.54 | 保留全部候选/控制项原始JSON |
| stage_ac_blas_operand_orientation | large_pad16_g8_pad_m_reg768 | 7.7734 | 187.82 | 保留全部候选/控制项原始JSON |
| stage_ac_native_m18880 | large_pad16_g8_pad_m_reg768 | 7.7616 | 187.47 | 保留全部候选/控制项原始JSON |
| stage_ad_generic_column_output | large_pad16_g8_pad_m_reg768 | 7.7807 | 187.65 | 保留全部候选/控制项原始JSON |
| stage_ae_vgpr_allocation_shift | large_pad16_g8_pad_m_reg768 | 7.7793 | 187.68 | 保留全部候选/控制项原始JSON |
| stage_af_contiguous_vector_restore | large_pad16_g8_trans_groupn_vecb_pad_m_reg768 | 7.0398 | 207.39 | 保留全部候选/控制项原始JSON |
| stage_af_native_m18880 | large_pad16_g8_trans_groupn_vecb_pad_m_reg768 | 7.0587 | 206.14 | 保留全部候选/控制项原始JSON |
| stage_ag_transposed_cache_swizzle | large_pad16_g8_trans_groupn_vecb_pad_m_reg768 | 7.0589 | 206.14 | 保留全部候选/控制项原始JSON |
| stage_ah_vector_group_tile | large_pad16_g8_trans_groupn4_vecb_pad_m_reg768 | 6.9789 | 208.50 | 保留全部候选/控制项原始JSON |
| stage_ai_phase_register_prefetch | rolephase_m1_reg768 | 6.4423 | 225.87 | 保留全部候选/控制项原始JSON |
| stage_aj_phase_cache_swizzle | rolephase_m1_reg768 | 6.4438 | 225.81 | 保留全部候选/控制项原始JSON |
| stage_ak_banked_wait_schedule | rolephase_m1_bank_asmb_reg768 | 6.0861 | 239.08 | 保留全部候选/控制项原始JSON |
| stage_al_weight_k32_buffer | rolephase_m1_bank_asmb_reg768 | 6.0875 | 239.03 | 保留全部候选/控制项原始JSON |
| stage_am_ready_wait_and_phase_stagger | rolephase_m1_swap_bank_asmb_reg768 | 6.0787 | 239.37 | 保留全部候选/控制项原始JSON |
| stage_an_ordered_lds_reads | rolephase_m1_bank_asmb_reg768 | 6.0881 | 239.00 | 保留全部候选/控制项原始JSON |
| stage_ao_phase_vgpr_alignment | rolephase_m1_bank_asmb_lpad3_reg768 | 6.0831 | 239.20 | 保留全部候选/控制项原始JSON |
| stage_ap_mixed_prefetch_and_structured_buffer | rolephase_m1_bank_asmb_mid_reg768 | 5.9007 | 246.59 | 保留全部候选/控制项原始JSON |
| stage_aq_branchfree_prefetch_and_unroll | rolephase_m1_bank_asmb_always_reg768 | 5.9847 | 243.14 | 保留全部候选/控制项原始JSON |
| stage_ar_mixed_prefetch_rebuild_verify | rolephase_m1_bank_asmb_mid_reg768 | 5.9059 | 246.38 | 保留全部候选/控制项原始JSON |
| stage_as_combined_mixed_branchfree | rolephase_m1_bank_asmb_always_mid_reg768 | 5.8611 | 248.26 | 保留全部候选/控制项原始JSON |
| stage_at_combined_prefetch_independent | rolephase_m1_bank_asmb_always_mid_reg768 | 5.8634 | 248.17 | 保留全部候选/控制项原始JSON |
| stage_au_bounded_buffer_output | rolephase_m1_bank_asmb_always_mid_bufferepi_reg768 | 5.6942 | 255.54 | 保留全部候选/控制项原始JSON |
| stage_av_xprof_guided_prefetch_points | rolephase_m1_bank_asmb_always_mid_bufferepi_reg768 | 5.6960 | 255.46 | 保留全部候选/控制项原始JSON |
| stage_aw_mmac_burst_boundaries | rolephase_m1_bank_asmb_always_mid_bufferepi_reg768 | 5.6957 | 255.47 | 保留全部候选/控制项原始JSON |
| stage_ax_k64_activation_banks | rolephase_m1_bank_asmb_always_mid_bufferepi_reg768 | 5.6966 | 255.43 | 保留全部候选/控制项原始JSON |
| stage_ay_branchless_bf16_output | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_reg768 | 5.6431 | 257.85 | 保留全部候选/控制项原始JSON |
| stage_az_branchless_rne_independent | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_reg768 | 5.6423 | 257.89 | 保留全部候选/控制项原始JSON |
| stage_b_tiles | wide | 13.9020 | 105.02 | 保留全部候选/控制项原始JSON |
| stage_ba_producer_consumer_cadence | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_reg768 | 5.3599 | 271.48 | 保留全部候选/控制项原始JSON |
| stage_bb_cadence_independent | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_reg768 | 5.3599 | 271.48 | 保留全部候选/控制项原始JSON |
| stage_bc_native_pow2_b | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_reg768 | 5.3604 | 271.45 | 保留全部候选/控制项原始JSON |
| stage_bd_scalar_addressing | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_abase_reg768 | 5.3578 | 271.59 | 保留全部候选/控制项原始JSON |
| stage_be_scalar_wave_livepads | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_reg768 | 5.3609 | 271.43 | 保留全部候选/控制项原始JSON |
| stage_bf_wave_group_barrier_stagger | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_reg768 | 5.3610 | 271.42 | 保留全部候选/控制项原始JSON |
| stage_bg_explicit_pair_tiles | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_reg768 | 5.3047 | 274.30 | 保留全部候选/控制项原始JSON |
| stage_bh_pair_tiles_independent | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_reg768 | 5.3047 | 274.30 | 保留全部候选/控制项原始JSON |
| stage_bi_scalar_m0_burst | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_reg768 | 5.3127 | 273.89 | 保留全部候选/控制项原始JSON |
| stage_bj_aburst_declared_clobbers | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_reg768 | 5.3046 | 274.31 | 保留全部候选/控制项原始JSON |
| stage_bk_grid_and_token_groups | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_grid2d_g1_reg768 | 5.2508 | 277.12 | 保留全部候选/控制项原始JSON |
| stage_bl_static_wave_groups | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768 | 4.9026 | 296.80 | 保留全部候选/控制项原始JSON |
| stage_bm_static_wave_independent | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768 | 4.9027 | 296.79 | 保留全部候选/控制项原始JSON |
| stage_bn_static_wave_lds_schedule | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768 | 4.9050 | 296.66 | 保留全部候选/控制项原始JSON |
| stage_bo_static_wave_b_prefetch | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768 | 4.8689 | 298.85 | 保留全部候选/控制项原始JSON |
| stage_bp_scalar_b_base_independent | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768 | 4.8710 | 298.73 | 保留全部候选/控制项原始JSON |
| stage_bq_fpclass_rne_and_b_prefetch | rolephase_m1_bank_asmb_always_mid_bufferepi_fpclassrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768 | 4.8658 | 299.05 | 保留全部候选/控制项原始JSON |
| stage_bs_accumulator_subbanks | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768 | 4.8704 | 298.76 | 保留全部候选/控制项原始JSON |
| stage_bt_producer_priority_balance | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768 | 4.8694 | 298.82 | 保留全部候选/控制项原始JSON |
| stage_bu_low_register_constraints | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768 | 4.8736 | 298.57 | 保留全部候选/控制项原始JSON |
| stage_bv_native_k64_activation_banks | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768 | 4.8655 | 299.06 | 保留全部候选/控制项原始JSON |
| stage_bw_native_k64_late_wait | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768 | 4.8692 | 298.84 | 保留全部候选/控制项原始JSON |
| stage_bx_fixed_nk_specialization | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_fixedk_reg768 | 4.8612 | 299.33 | 保留全部候选/控制项原始JSON |
| stage_by_final_bbase_cta_traversal | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768 | 4.8726 | 298.63 | 保留全部候选/控制项原始JSON |
| stage_bz_native_b64_addressing_and_mmac_window | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768 | 4.8643 | 299.14 | 保留全部候选/控制项原始JSON |
| stage_c_raw_barrier | wide_raw_barrier | 12.4500 | 117.27 | 保留全部候选/控制项原始JSON |
| stage_ca_wave_group_prefetch_balance | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_gp12x8_reg768 | 4.8523 | 299.87 | 保留全部候选/控制项原始JSON |
| stage_cb_native_prefetch_between_math_phases | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768 | 4.8724 | 298.64 | 保留全部候选/控制项原始JSON |
| stage_cc_two_lds_bases_and_immediate_reads | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768 | 4.8638 | 299.16 | 保留全部候选/控制项原始JSON |
| stage_cd_b_wait_and_barrier_decoupling | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_vmafter_reg768 | 4.8724 | 298.64 | 保留全部候选/控制项原始JSON |
| stage_ce_independent_mmac_tile_order | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mchunk4_reg768 | 4.8565 | 299.62 | 保留全部候选/控制项原始JSON |
| stage_cf_producer_terminal_branch | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768 | 4.8656 | 299.06 | 保留全部候选/控制项原始JSON |
| stage_cg_producer_fixed_slot_pairs | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768 | 4.8682 | 298.90 | 保留全部候选/控制项原始JSON |
| stage_ch_producer_scalar_offset | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair1_asoff_reg768 | 4.8416 | 300.54 | 保留全部候选/控制项原始JSON |
| stage_ci_producer_scalar_offset_independent | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair1_asoff_reg768 | 4.8427 | 300.47 | 保留全部候选/控制项原始JSON |
| stage_cj_two_projection_producer_dispatch | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair1_asoff_reg768 | 4.8411 | 300.57 | 保留全部候选/控制项原始JSON |
| stage_ck_shape_dispatch_independent | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768 | 4.8475 | 300.17 | 保留全部候选/控制项原始JSON |
| stage_cl_native_b64_single_rebase | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768 | 4.8442 | 300.38 | 保留全部候选/控制项原始JSON |
| stage_cm_native_b64_rebase_schedule | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768 | 4.8487 | 300.10 | 保留全部候选/控制项原始JSON |
| stage_cn_native_b64_shared_startup | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768 | 4.8494 | 300.05 | 保留全部候选/控制项原始JSON |
| stage_co_native_b64_immutable_initial | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768 | 4.8422 | 300.50 | 保留全部候选/控制项原始JSON |
| stage_cp_native_b64_phase_waits | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768 | 4.8471 | 300.20 | 保留全部候选/控制项原始JSON |
| stage_cq_paired_feature_output_builtin | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768 | 4.8441 | 300.39 | 保留全部候选/控制项原始JSON |
| stage_cr_paired_output_alignment_hoist | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768 | 4.8473 | 300.19 | 保留全部候选/控制项原始JSON |
| stage_cs_half_feature_tile | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768 | 4.8503 | 300.00 | 保留全部候选/控制项原始JSON |
| stage_ct_half_tile_six_waves | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768 | 4.8513 | 299.94 | 保留全部候选/控制项原始JSON |
| stage_cu_original_tile_two_producers | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768 | 4.8543 | 299.75 | 保留全部候选/控制项原始JSON |
| stage_cv_activation_scalar_offset | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768 | 4.8504 | 299.99 | 保留全部候选/控制项原始JSON |
| stage_cw_phase1_a_streaming | rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768 | 4.8545 | 299.74 | 保留全部候选/控制项原始JSON |
| stage_d_double_buffer | wide4_raw_barrier | 12.0848 | 120.81 | 保留全部候选/控制项原始JSON |
| stage_e_lds_padding | wide_pad16 | 10.5635 | 138.21 | 保留全部候选/控制项原始JSON |
| stage_f_dynamic_double | wide4_raw_barrier | 12.0776 | 120.89 | 保留全部候选/控制项原始JSON |
| stage_g_warp_k16 | wide_pad16 | 10.5659 | 138.18 | 保留全部候选/控制项原始JSON |
| stage_h_tile_order | wide_pad16_g8 | 10.1265 | 144.18 | 保留全部候选/控制项原始JSON |
| stage_i_pack_xor | wide_pad16_g8 | 10.1170 | 144.31 | 保留全部候选/控制项原始JSON |
| stage_j_launch_bounds | wide_pad16_g8 | 10.1136 | 144.36 | 保留全部候选/控制项原始JSON |
| stage_k_n_interleave | wide_pad16_g8 | 10.1319 | 144.10 | 保留全部候选/控制项原始JSON |
| stage_l_balanced_operands | wide_mr4_pad16_g8 | 9.7622 | 149.56 | 保留全部候选/控制项原始JSON |
| stage_m_kpack_major | wide_pad16_g8 | 10.1336 | 144.08 | 保留全部候选/控制项原始JSON |
| stage_n_wave_specialized | wide_mr4_pad16_g8 | 9.7627 | 149.55 | 保留全部候选/控制项原始JSON |
| stage_o_register_pool | large_pad16_g8_reg768 | 7.7728 | 187.84 | 保留全部候选/控制项原始JSON |
| stage_p_large_pipeline | large_pad16_g8_reg768 | 7.7732 | 187.83 | 保留全部候选/控制项原始JSON |
| stage_q_specialized_layout | large_pad16_g8_reg768 | 7.7661 | 188.00 | 保留全部候选/控制项原始JSON |
| stage_r_buffer_priority | large_pad16_g8_reg768 | 7.7721 | 187.86 | 保留全部候选/控制项原始JSON |
| stage_s_aspect_k64 | large_pad16_g8_reg768 | 7.7730 | 187.83 | 保留全部候选/控制项原始JSON |
| stage_u_12_math_waves | large_pad16_g8_reg768 | 7.7729 | 187.84 | 保留全部候选/控制项原始JSON |
| stage_u_native_m18880 | large_pad16_g8_pad_m_reg768 | 7.7648 | 187.40 | 保留全部候选/控制项原始JSON |
| stage_v_k64_wrapped_pipeline | large_pad16_g8_pad_m_reg768 | 7.7707 | 187.89 | 保留全部候选/控制项原始JSON |
| stage_w_mmac_order | large_pad16_g8_pad_m_reg768 | 7.7743 | 187.80 | 保留全部候选/控制项原始JSON |
| stage_x_cooperative_prefetch | large_pad16_g8_pad_m_reg768 | 7.7756 | 187.77 | 保留全部候选/控制项原始JSON |
| stage_y_structured_cache_swizzle | large_pad16_g8_pad_m_reg768 | 7.7763 | 187.75 | 保留全部候选/控制项原始JSON |
| stage_z_structured_wait_tracking | large_pad16_g8_pad_m_reg768 | 7.7721 | 187.85 | 保留全部候选/控制项原始JSON |

## 已实施和已验证的方向

KernelWiki迁移边界、实际rocBLAS完整ISA、等待计数和编译寄存器池的调查见 `WIKI_AND_ROCBLAS_ISA.md`，原始文件位于 `results/library_reference` 与 `results/stage_waits`。
后续LDS wrap功能探针、K64分工/协作流水、cache swizzle、算术隔离和回退证据见 `LDS_WRAP_AND_PIPELINE.md`。其中算术探针不是GEMM，不纳入本报告最快记录或目标判定。

- A：保留原始128×128×32基线，对照恢复旧policy LDS swizzle。恢复布局正确，但比基线慢，未作为主方案。
- B：BM/BN/BK/wave组合；128×256明显更快，256×256遇到严重spill，不采用。单纯增加BK不保证收益。
- C：汇编显示下一块global load在当前块MMAC前被vmcnt(0)提前等待；局部inline LDS barrier改变该安排，宽tile有收益。只用于纯GEMM中跨wave共享LDS的场景，不能直接复用于跨卡通信。
- D/F：分别试展开和单主循环动态双LDS缓冲。正确性通过，较大配置未胜过当前最好，资源成本抵消收益。
- E：改变LDS物理行跨度，保留逻辑矩阵；padding16配置较好。
- G：更小warp K片段；当前测试没有改善，不采用。
- N：8计算+4加载wave的独立CK MMAC原型；计算正确，但默认768-thread约束仅分配84VGPR并大量spill，约4.6TFLOPS，明确淘汰。
- O：核实DTK支持的`--enable-num-vgprs-768=true`，只影响候选编译，不改运行库或ELF元数据。large_pad16_g8实际202VGPR、无spill，FC2约188TFLOPS；小wide配置没有明显收益。原型消除spill后约132TFLOPS，仍不胜过通用候选。
- P/Q：在确认寄存器池后验证大tile布局/双缓冲，以及原型的padding、Kpack、CK海光direct-to-LDS接口。具体完成与否以完整阶段列表及原始JSON为准，不能把编译成功当作性能验证通过。
- R：原型B使用buffer-load、计算wave显式s_setprio、组合对照。正确性通过，但仍未胜过大tile通用模板；单独修改某类指令不等于复现完整库调度。
- S：扩大/缩小tile长宽比、BK64和padding8；完成状态以阶段列表为准。
- S轮初次bench因BN512与FC2/out_proj的N=5376不整除而返回InvalidValue；保留环境在stage_s_incompatible_bn512_initial。实验bench修复为调用前明确记录unsupported_alignment并排除计时，真实kernel异常仍失败；随后完成四shape筛选。见STAGE_S_INITIAL_FAILURE.md。
- T：DTK帮助列出ILP与occupancy两种machine scheduler，但实际编译均在调度器中崩溃；未进入GPU测试，未纳入性能结果。见STAGE_T_COMPILER_FAILURE.md，保持已验证默认调度器。
- U：以扩展寄存器池编译12个数学wave的384×256候选，并开启CK原生M padding/尾行处理；对照256×256尾行版本。额外实测M=18880并加输出guard；状态以完整阶段列表及正确性JSON为准，不混合不同M的最快时间。
- U轮12数学wave候选错误、未计时：B global-load分区256/(12×16)整数截断，漏覆盖。已在04私有policy增加完整覆盖static_assert，原二进制与冻结源保留作失败证据；有效256×256候选真实M四投影及guard通过。见STAGE_U_CORRECTNESS.md。
- V：独立探针16384元素确认HCU m0 wrap布局；K64分工原型单缓冲wrap约150、双缓冲162TFLOPS，四投影正确，仍未胜过约188的通用模板。
- W：相同MMAC inline asm生成相同库SHA，排除重复计时；literal-zero ASM不支持，保留编译失败证据；首步初始化候选正确但回退。
- X：8计算wave协作搬运，四投影正确但没赢过12-wave分工原型。算术隔离探针403–461TFLOPS不能当GEMM成绩。
- Y/Z：CK结构化buffer cache swizzle与idxen路径，A/B/AB和安全等待/LLVM自动等待对照，全部四投影正确；最好原型约164TFLOPS，仍未胜过通用模板。
- AA：小K大输出tile与LDS/寄存器工作集实验。构建脚本将_pad_m误判为_pad数字，修复后保留旧构建并补编译；见STAGE_AA_BUILD_RECOVERY.md。完成状态以阶段列表为准。
- AB：仅改变wave priority和编译调度屏障，不移除GPU数据同步；原型组合约170TFLOPS，未胜过通用模板。
- AC：按BLAS列主序交换内部A/B，计算W@X.T并转置写回X@W.T；对照按feature/token分组。各投影正确，out_proj有新最好候选，整体仍未达到330TFLOPS；原生18880结果单独计分。
- AD：把交换操作数与列主序输出迁回03派生通用CK模板，另以按N分组的私有partitioner对照。没有额外transpose kernel；完成状态以阶段列表和实际GPU检查为准。
- AE：用额外活跃VGPR将MMAC输入寄存器奇偶起点分别移动；全部正确但约50TFLOPS回退不变，排除“寄存器起点奇偶”单一归因。
- AF：源代码发现kPadN将连续K方向B读取降为scalar，ISA确认32条ushort加载。私有Problem恢复vec8且保留N尾保护，另用关闭N padding的整齐shape对照；四shape正确，约204–214TFLOPS。真实18880也验证尾部guard，目标仍未达330。
- AG：按照已核实BLAS方向重新比较cache swizzle（A=权重，B=激活）。结构化A/B/AB全部正确，但均比对应raw原型慢，未纳入最快向量修复候选；不是cache swizzle带来AF收益。
- AH：恢复向量加载后测试按token方向的4/8/16/32 CTA分组及128×512宽token tile；完成情况、性能和正确性均以阶段JSON为准。
- AI：独立rolephase.cpp将激活预取缩成K32相位；1×8计算wave方案真实M18880约213–229TFLOPS，四投影正确。2×4/4×2虽减少LDS read却因其他成本显著回退，不能单看LDS静态指令数预估收益；详见LDS_WRAP_AND_PIPELINE.md及phase_isa_analysis.json。
- AJ：新相位流水的A/B/AB结构化cache swizzle独立同轮验证，状态、数值和收益以完整JSON为准，不能拿旧Y/Z或AG的结论替代。
- AK：激活寄存器双bank轮换，raw asm load显式vmcnt(NT)等待当前bank、保留未来bank；当前1×8分区NT=2。四shape正确，约223–240TFLOPS；普通LLVM双bank或priority回退。数据依赖与终末vmcnt(0)不能省略；实际源/库与ISA证明见LDS_WRAP_AND_PIPELINE.md。
- AL：K32权重双缓冲先完成8192元素独立硬件映射验证，再测实际GEMM。32KiB LDS候选正确但约176–188TFLOPS，未胜过64KiB显式等待方案；较小LDS不能单独证明有效驻留或性能改善。
- AM–AO：延后producer预取、反转相位、显式LDS顺序、额外活跃VGPR对照；无稳定收益。AO未真正改变首条MMAC寄存器起点，不能作全面对齐结论。
- AP/AR：16次LDS读取中间插入未来B预取，约229–247TFLOPS，独立复测和72组/216次审计通过。A/B cache swizzle仍回退。
- AQ：去掉热循环未来预取条件，但保留末尾bank活跃和vmcnt(0)。unroll2与always库SHA相同，汇总按SHA排除重复候选。
- AS/AT：mixed+always组合独立两轮约232–250TFLOPS，72组/216次审计通过；额外progressive LDS wait回退。
- AU：有界buffer输出，全部token偏移放voffset、soffset=0，保留CK RNE；四投影约244–258TFLOPS，独立72组/216次审计通过。早期构建错误和未执行的soffset不安全版均保留；详见LDS_WRAP_AND_PIPELINE.md。
- xprof：完整sections/SQTT及文本counter证据保存在xprof_au_*目录，性能排序不使用插桩duration；分析与限制见XPROF_ANALYSIS.md。
- AV：据xprof LDS等待方向改变未来B预取点4/12或wave组错开，四投影正确但回退；同轮原AU独立复测仍约244–258TFLOPS，不采用新候选。
- AW：限制跨相位调度、连续math burst及第一条MMAC后priority，四投影正确但均回退；资源与AU相同，不采用。开关关闭重编译compat与AU逐byte同SHA，按二进制去重。
- AX：完整K64激活双bank与phase0/1预取对照，四投影正确但回退；phase0有2VGPR spill/12B scratch，phase1无spill仍慢，不能一概归因spill。未做广尾形状审计，不外推。
- AY/AZ：branchless BF16 RNE保留CK有限数/Inf/NaN payload语义；1507328位模式与独立72组/216次GEMM审计通过。新库独立复测约248–261TFLOPS，相对同轮AU耗时下降0.7%–1.6%，仍未达330；mask/select同SHA去重。详见报告第22节和rne_isa_comparison.json。
- BA/BB：新producer/consumer cadence使A下一tile预取覆盖当前两数学相位，独立四shape约259–274TFLOPS、耗时较旧RNE降低4.23%–5.17%，72组/216次审计通过；资源不变。关闭开关重编译与旧基线逐byte相同。原生LDS地址16384元素CPU核对与现有wrap一致。新counter显示barrier/LGKM聚合等待减少但LDS issue增加，不能把计数当ms；详见第23–25节。
- BC：原生式pow2 stride+row remainder B寻址，先完成30组顺序数据与30组随机数据边界探针，四GEMM数值正确但更慢；16-byte偏置无净收益，终末VM完整等待另设控制项，不采用。
- BD：A资源base按tile标量移动几乎无收益；显式wave-uniform索引回退约8%，且改变VGPR资源与MMAC排列，不能只按地址计算次数推断收益。关闭新开关重编译仍与b44基线相同。
- BE：scalar-wave上的live-pad1/2/3实际移动了首条MMAC的A操作数位置，但B及C未移动；略恢复性能仍不胜过b44。只能作部分寄存器配置对照，不是全面排除对齐影响。
- BF：确认原生两组wave区别是barrier相对数学的错位，不是K半段反转；HIP/CK交错方案四shape正确但回退，post-priority部分恢复仍未胜出，尚未作广审计或采用。详见报告第26–29节。
- BG/BH：显式两个K64 tile展开、固定slot0/1且保持K32 B bank；实际静态MMAC128→256，不是动态数学量翻倍。四shape独立复测约263–279TFLOPS，较b44耗时下降约1%–1.6%；102组/306次审计增加5个奇数K尾部，全部通过。当前已验证候选更新为19453374...，仍未达到330。
- BI/BJ：A搬运单次m0读取+标量4KiB递增，以及A base组合，四shape正确但回退。IR核查初稿缺少m0/SCC clobber，已显式补齐并在新名字/目录重测；该版本机器码恰好不变，不能据此省掉汇编契约。未采用；详见第31节。
- H：按M分组遍历tile。部分shape有小幅收益，GROUP_M需按shape比较。
- I：轻量K-pack XOR，正确性通过，目前未超过总体最好配置；out_proj有单项改善。
- J及后续轮次：继续检查launch bounds、BF16 lane/interleave和每wave操作数复用。以results目录及下面阶段完整性为准，不把编译成功称为性能验证完成。

## 设备资源证据

来自实际编译设备ELF的元数据；VGPR count是编译资源字段，不根据它单独推算驻留率。

| 候选 | VGPR | spill字段 | LDS bytes | scratch bytes |
|---|---:|---:|---:|---:|
| paired_output_probe | 10 | 0 | 0 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p0_endwait_reg768 | 246 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mmacg4_dsasm_reg768 | 256 | 4 | 65536 | 16 |
| large_wk16_kpack8_g8_pad_m_reg768 | 168 | 0 | 16384 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream1_reg768 | 241 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768 | 230 | 0 | 65536 | 0 |
| specialized_kpack8_buffer_reg768 | 214 | 0 | 16384 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob3_reg768 | 223 | 0 | 65536 | 0 |
| large_relaxed | 128 | 475 | 32768 | 672 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_reg768 | 221 | 0 | 65536 | 0 |
| large_pad16_g8_trans_groupn_lpad3_pad_m_reg768 | 214 | 0 | 49088 | 0 |
| role64_double_wrap_cachea_reg768 | 237 | 0 | 65536 | 0 |
| specialized_async_kpack8_buffer_prio_reg768 | 214 | 0 | 16384 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p1_late_reg768 | 242 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p1_reg768 | 242 | 0 | 65536 | 0 |
| base_pad8 | 138 | 0 | 20448 | 0 |
| role64_double_wrap_noprio_reg768 | 237 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack2_aw16_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_reg768 | 240 | 0 | 65536 | 0 |
| large_kpack8_g8_reg768 | 200 | 0 | 32768 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_gp8x4_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m4_priority_reg768 | 247 | 0 | 65536 | 0 |
| role64_double_wrap_cacheab_llvm_reg768 | 238 | 0 | 65536 | 0 |
| large_pad16_g8_trans_groupn_lpad1_pad_m_reg768 | 212 | 0 | 49088 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_fixednk_reg768 | 225 | 0 | 65536 | 0 |
| wide_mr4_pad16_g8_reg768 | 117 | 0 | 36800 | 0 |
| specialized_pad16_reg768 | 218 | 0 | 24576 | 0 |
| rolephase_m1_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_stagger_postprio_reg768 | 223 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mmacg4_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_dsasm_reg768 | 256 | 6 | 65536 | 20 |
| specialized_reg768 | 218 | 0 | 16384 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_np0_reg768 | 246 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_b64p1_reg768 | 242 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pow2b_bias16_reg768 | 227 | 0 | 65536 | 0 |
| rolephase_m1_priority_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_pow2b_np0_reg768 | 250 | 0 | 65536 | 0 |
| swizzle | 153 | 0 | 16384 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob1_areadwait_reg768 | 221 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_midgroup_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768 | 228 | 0 | 65536 | 0 |
| large_pad16_g8_trans_groupn_vecb_pad_m_reg768 | 202 | 0 | 49088 | 0 |
| rolephase_m1_bank_asmb_always_mid4_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_vmafter_reg768 | 225 | 0 | 65536 | 0 |
| role64_coop_double_wrap_reg768 | 249 | 0 | 65536 | 0 |
| wide4_raw_barrier | 252 | 0 | 24576 | 0 |
| rolephase_m1_bank_asmb_lpad1_reg768 | 224 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pairout3_reg768 | 229 | 0 | 65536 | 0 |
| wide4_pad8 | 251 | 0 | 30688 | 0 |
| large_pad16_g8_pad_m_reg768 | 202 | 0 | 49088 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_compat_reg768 | 221 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_pwait_reg768 | 240 | 0 | 65536 | 0 |
| rolephase_m1_readywait_swap_bank_asmb_reg768 | 226 | 0 | 65536 | 0 |
| k64 | 191 | 0 | 32768 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p0_late_reg768 | 242 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p0_reg768 | 242 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack2_aw32_reg768 | 225 | 0 | 65536 | 0 |
| large_wk16_kpack4_g8_pad_m_reg768 | 166 | 0 | 16384 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_swave_lpad3_reg768 | 248 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream4_reg768 | 241 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_g2_reg768 | 225 | 0 | 65536 | 0 |
| taller384_pad16_g8_pad_m_reg768 | 197 | 0 | 61376 | 0 |
| rolephase_m1_bank_asmb_always_mid4_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_cw4_pw2_sf2_reg768 | 210 | 0 | 32768 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pairout2_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_bhoist_reg768 | 256 | 3 | 65536 | 16 |
| wide4_double | 256 | 163 | 49152 | 288 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_aburst_abase_reg768 | 226 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_g4_reg768 | 226 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_stagger_reg768 | 226 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_priority_reg768 | 230 | 0 | 65536 | 0 |
| large_pad16_g8_trans_groupn_lpad2_pad_m_reg768 | 213 | 0 | 49088 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_g2_reg768 | 226 | 0 | 65536 | 0 |
| role64_double_wrap_cacheab_reg768 | 238 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_finalwait_reg768 | 221 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_binit_reg768 | 256 | 1 | 65536 | 8 |
| role64_double_wrap_cacheb_latewait_reg768 | 238 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bsoff_reg768 | 225 | 0 | 65536 | 0 |
| wide_raw_barrier | 125 | 0 | 24576 | 0 |
| large_pad16_relaxed | 128 | 467 | 49088 | 628 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_endwait_reg768 | 246 | 0 | 65536 | 0 |
| tall512_mr4_pad16_g8_reg768 | 207 | 0 | 61376 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mchunk8_atwob1_reg768 | 221 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_reg768 | 221 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_reg768 | 230 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_pow2b_reg768 | 250 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_reg768 | 221 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_lpad3_reg768 | 226 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid12_bufferepi_reg768 | 221 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_swave_lpad2_reg768 | 247 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_b64p0_reg768 | 256 | 2 | 65536 | 12 |
| role64_coop_double_plain_reg768 | 248 | 0 | 65536 | 0 |
| large_mr4_k64_kpack8_g8_reg768 | 224 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_np12_reg768 | 244 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid6_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_reg768 | 129 | 0 | 32768 | 0 |
| rolephase_m1_bank_asmb_lpad2_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_reg768 | 221 | 0 | 65536 | 0 |
| large_pad16_g8_trans_pad_m_reg768 | 211 | 0 | 49088 | 0 |
| role64_double_wrap_reg768 | 237 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_asoff_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair1_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid12_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768 | 230 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid4_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768 | 230 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_reg768 | 244 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_aburst_reg768 | 226 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_swave_lpad1_reg768 | 246 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_fixedn_reg768 | 225 | 0 | 65536 | 0 |
| large_pad8_g8_reg768 | 202 | 0 | 40928 | 0 |
| wide | 124 | 0 | 24576 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_aburst_abase_cc_reg768 | 226 | 0 | 65536 | 0 |
| role64_double_wrap_nosched_noprio_reg768 | 248 | 0 | 65536 | 0 |
| large_double_g8_reg768 | 256 | 124 | 65536 | 180 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_gp8x12_reg768 | 225 | 0 | 65536 | 0 |
| wide512_mr4_pad16_g8_reg768 | 207 | 0 | 61376 | 0 |
| large_mr4_pad16_g8_reg768 | 202 | 0 | 49088 | 0 |
| rolephase_m1_bank_asmb_dsprogress_reg768 | 247 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pv3_reg768 | 225 | 0 | 65536 | 0 |
| mmac_micro_512 | 202 | 0 | 0 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_g1_reg768 | 226 | 0 | 65536 | 0 |
| rolephase_m1_readywait_bank_asmb_reg768 | 223 | 0 | 65536 | 0 |
| large_wk16_pad16_g8_pad_m_reg768 | 168 | 0 | 32704 | 0 |
| mmac_micro_768 | 202 | 0 | 0 | 0 |
| specialized_async_kpack8_reg768 | 218 | 0 | 16384 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_g16_reg768 | 226 | 0 | 65536 | 0 |
| specialized_kpack8_prio_reg768 | 216 | 0 | 16384 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mchunk16_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mchunk4_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid6_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768 | 230 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid4_bufferepi_reg768 | 221 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_dsasm_reg768 | 247 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_g1_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reserve2_reg768 | 227 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_burst_postprio_reg768 | 221 | 0 | 65536 | 0 |
| role64_double_wrap_trans_groupn_nosched_noprio_reg768 | 248 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_pow2b_np4_reg768 | 250 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pv2_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_aburst_cc_reg768 | 226 | 0 | 65536 | 0 |
| rolephase_m1_bank_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_endwait_pow2b_reg768 | 250 | 0 | 65536 | 0 |
| wide_pad16 | 125 | 0 | 36800 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_np4_reg768 | 244 | 0 | 65536 | 0 |
| large_pad16_g8_trans_groupn_pad_m_reg768 | 211 | 0 | 49088 | 0 |
| large_mr4_xor_g8_reg768 | 201 | 0 | 32768 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_postprio_reg768 | 221 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack1_aw32_reg768 | 256 | 111 | 65536 | 448 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_endwait_base64_reg768 | 244 | 0 | 65536 | 0 |
| role64_double_wrap_zero_reg768 | 244 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pairout1_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid4_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_reg768 | 129 | 0 | 32768 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_aburst_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reserve3_reg768 | 228 | 0 | 65536 | 0 |
| base | 121 | 0 | 16384 | 0 |
| role64_double_wrap_cacheb_reg768 | 238 | 0 | 65536 | 0 |
| specialized | 84 | 527 | 16384 | 800 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_gp4x8_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_g4_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_fpclassrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_mid_reg768 | 223 | 0 | 65536 | 0 |
| large_kpack8_g8_trans_groupn_pad_m_reg768 | 209 | 0 | 32768 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_sf2_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m2_reg768 | 215 | 0 | 65536 | 0 |
| wide_double | 128 | 141 | 49152 | 280 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_selectrne_reg768 | 221 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_sf6_reg768 | 225 | 0 | 65536 | 0 |
| role64_double_wrap_nosched_reg768 | 237 | 0 | 65536 | 0 |
| rolephase_m4_reg768 | 247 | 0 | 65536 | 0 |
| rolephase_m1_weight32_bank_asmb_reg768 | 227 | 0 | 32768 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid6_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_grid2d_reg768 | 226 | 0 | 65536 | 0 |
| role64_coop_single_wrap_reg768 | 243 | 0 | 32768 | 0 |
| role64_single_wrap_reg768 | 233 | 0 | 32768 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack3_aw32_reg768 | 256 | 334 | 65536 | 956 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_abase_reg768 | 225 | 0 | 65536 | 0 |
| large_wk16_pad8_g8_pad_m_reg768 | 168 | 0 | 24544 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_reg768 | 221 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reserve1_reg768 | 226 | 0 | 65536 | 0 |
| b_soffset_probe | 5 | 0 | 0 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_reg768 | 226 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_grid2d_g1_reg768 | 226 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_reg768 | 223 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_burst_reg768 | 221 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob1_reg768 | 221 | 0 | 65536 | 0 |
| large | 128 | 326 | 32768 | 508 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair2_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_reg768 | 246 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_midsplit_bufferepi_reg768 | 222 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bsoff_ppair1_asoff_reg768 | 225 | 0 | 65536 | 0 |
| base_double | 211 | 0 | 32768 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_stagger_reg768 | 223 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pairout4_reg768 | 229 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair1_asoff_reg768 | 225 | 0 | 65536 | 0 |
| specialized_kpack8_reg768 | 218 | 0 | 16384 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p0_late_base64_reg768 | 244 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_aalways1_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair1_asoff_cw8_pw2_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pow2b_reg768 | 227 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_aalways2_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_dsprogress_reg768 | 256 | 6 | 65536 | 20 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_bhoist_reg768 | 256 | 1 | 65536 | 8 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_binit_reg768 | 256 | 2 | 65536 | 12 |
| specialized_kpack8_buffer_prio_reg768 | 214 | 0 | 16384 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mchunk8_reg768 | 225 | 0 | 65536 | 0 |
| role64_double_wrap_postprio_reg768 | 237 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_cachea_reg768 | 225 | 0 | 65536 | 0 |
| role64_double_wrap_trans_groupn_reg768 | 237 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_mathfence_reg768 | 221 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_pow2b_reg768 | 234 | 0 | 65536 | 0 |
| large_raw_barrier_g8_reg768 | 202 | 0 | 32768 | 0 |
| wide_pad8 | 125 | 0 | 30688 | 0 |
| rolephase_m1_swap_bank_asmb_reg768 | 226 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_fixedk_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_abase_reg768 | 221 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_priority_reg768 | 223 | 0 | 65536 | 0 |
| producer_soffset_probe | 39 | 0 | 32768 | 0 |
| role64_double_wrap_cacheb_llvm_reg768 | 238 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p0_late_pow2b_reg768 | 250 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_abase_swave_reg768 | 245 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_dsasm_postprio_reg768 | 247 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_builtinlgkm_reg768 | 230 | 0 | 65536 | 0 |
| large_pad16_g8_trans_groupn_aligned_pad_m_reg768 | 202 | 0 | 49088 | 0 |
| role64_double_wrap_trans_reg768 | 237 | 0 | 65536 | 0 |
| role64_single_plain_reg768 | 233 | 0 | 32768 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_swave_reg768 | 245 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid6_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_cw4_pw2_sf2_reg768 | 210 | 0 | 32768 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_pwait_reg768 | 244 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_cw8_pw2_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob2_reg768 | 223 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_gp12x8_reg768 | 225 | 0 | 65536 | 0 |
| rolephase_m1_readywait_bank_asmb_priority_reg768 | 223 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p0_late_reg768 | 246 | 0 | 65536 | 0 |
| rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_np4_reg768 | 246 | 0 | 65536 | 0 |
| large_pad16_g8_reg768 | 202 | 0 | 49088 | 0 |

## 硬件计数器：剩余差距的方向

已先核实本机rocprof --list-basic，再对同一个FC2采集两组counter；每个实现3次，输出metrics.csv。不同counter组来自不同进程运行，工具按dispatch合并，不作为同一个时刻的联合测量。

| 实现 | MMOP指令计数 | LDS指令计数 | LDS bank冲突计数 |
|---|---:|---:|---:|
| base | 178,225,152 | 77,973,504 | 445,562,880 |
| wide_pad16 | 178,225,152 | 72,403,968 | 133,668,864 |
| rocblas | 178,225,152 | 44,556,288 | 0 |

三者MMOP指令量一致，说明计算量比较一致；CK仍有更多LDS指令和冲突，而rocBLAS该计数为0。优化后冲突计数下降，但尚不能把计数下降比例直接当成时间收益。

本机目录中MMOP、LDS指令和bank冲突相关条目均标为emulated，以上保留为DTK工具报告量，不能直接称为独立测得的bank停顿时间。SQ_ACTIVE_INST_MMOP固定约为指令计数2倍，不能当作真实MMAC忙碌率或峰值利用率。需结合ISA、实际非emulated等待计数与未插桩性能验证。全部原始counter、时间戳及目录保存在results/stage_profile；性能排序始终使用未插桩bench。

## 文件与复现

- gemm.cpp：隔离BF16入口；policy.hpp是03所用policy的私有副本。
- padded_policy.hpp、xor_policy.hpp、grouped_partitioner.hpp：布局/调度候选。
- double_buffer*.hpp、warp_variants.hpp：流水/软件warp片段候选。
- build.py：候选配置、编译命令、退出码和二进制SHA256；后续轮次冻结源码在build/attempts。
- bench.py：正确性门槛和HIP event计时；results/stage_*/保存完整轮次数据。
- inspect_device.py：从实际ELF提取资源；build/isa_*/保存元数据。早期临时inspect.py曾遮蔽Python标准库，已改名；B轮编译成功后的第一次bench因此未执行，修复后使用同一批二进制完成测试。
- profile_once.py、run_profile.sh：小GEMM硬件counter调查，不启动模型。
- run_stage_*.sh：每轮复现脚本；顺序运行，不并发占用GPU。早期run_stage_c.sh只重复了本来默认开启的编译flag，两份二进制与base/wide哈希一致，未运行重复GPU测试；有效C轮为run_stage_c_raw.sh。

例如宿主机在HCU4空闲时执行：

```bash
docker exec ck_yyc bash /workspace/composable_kernel/example_hcu/ck_tile/04_gemm/run_stage_h.sh
docker exec ck_yyc python3 /workspace/composable_kernel/example_hcu/ck_tile/04_gemm/summarize.py
```

重跑会覆盖相同tag的results，需保留历史时复制新目录或更换tag。大tile/双缓冲等回退方案保留作实验，不作为默认最终方案。目标达标还需要最终候选的独立复测、更广正确性和真实M验收，并回到H3容器确认。

完整阶段：stage_a_layout, stage_aa_k16_residency, stage_ab_scheduling_controls, stage_ac_blas_operand_orientation, stage_ac_native_m18880, stage_ad_generic_column_output, stage_ae_vgpr_allocation_shift, stage_af_contiguous_vector_restore, stage_af_native_m18880, stage_ag_transposed_cache_swizzle, stage_ah_vector_group_tile, stage_ai_phase_register_prefetch, stage_aj_phase_cache_swizzle, stage_ak_banked_wait_schedule, stage_al_weight_k32_buffer, stage_am_ready_wait_and_phase_stagger, stage_an_ordered_lds_reads, stage_ao_phase_vgpr_alignment, stage_ap_mixed_prefetch_and_structured_buffer, stage_aq_branchfree_prefetch_and_unroll, stage_ar_mixed_prefetch_rebuild_verify, stage_as_combined_mixed_branchfree, stage_at_combined_prefetch_independent, stage_au_bounded_buffer_output, stage_av_xprof_guided_prefetch_points, stage_aw_mmac_burst_boundaries, stage_ax_k64_activation_banks, stage_ay_branchless_bf16_output, stage_az_branchless_rne_independent, stage_b_tiles, stage_ba_producer_consumer_cadence, stage_bb_cadence_independent, stage_bc_native_pow2_b, stage_bd_scalar_addressing, stage_be_scalar_wave_livepads, stage_bf_wave_group_barrier_stagger, stage_bg_explicit_pair_tiles, stage_bh_pair_tiles_independent, stage_bi_scalar_m0_burst, stage_bj_aburst_declared_clobbers, stage_bk_grid_and_token_groups, stage_bl_static_wave_groups, stage_bm_static_wave_independent, stage_bn_static_wave_lds_schedule, stage_bo_static_wave_b_prefetch, stage_bp_scalar_b_base_independent, stage_bq_fpclass_rne_and_b_prefetch, stage_bs_accumulator_subbanks, stage_bt_producer_priority_balance, stage_bu_low_register_constraints, stage_bv_native_k64_activation_banks, stage_bw_native_k64_late_wait, stage_bx_fixed_nk_specialization, stage_by_final_bbase_cta_traversal, stage_bz_native_b64_addressing_and_mmac_window, stage_c_raw_barrier, stage_ca_wave_group_prefetch_balance, stage_cb_native_prefetch_between_math_phases, stage_cc_two_lds_bases_and_immediate_reads, stage_cd_b_wait_and_barrier_decoupling, stage_ce_independent_mmac_tile_order, stage_cf_producer_terminal_branch, stage_cg_producer_fixed_slot_pairs, stage_ch_producer_scalar_offset, stage_ci_producer_scalar_offset_independent, stage_cj_two_projection_producer_dispatch, stage_ck_shape_dispatch_independent, stage_cl_native_b64_single_rebase, stage_cm_native_b64_rebase_schedule, stage_cn_native_b64_shared_startup, stage_co_native_b64_immutable_initial, stage_cp_native_b64_phase_waits, stage_cq_paired_feature_output_builtin, stage_cr_paired_output_alignment_hoist, stage_cs_half_feature_tile, stage_ct_half_tile_six_waves, stage_cu_original_tile_two_producers, stage_cv_activation_scalar_offset, stage_cw_phase1_a_streaming, stage_d_double_buffer, stage_e_lds_padding, stage_f_dynamic_double, stage_g_warp_k16, stage_h_tile_order, stage_i_pack_xor, stage_j_launch_bounds, stage_k_n_interleave, stage_l_balanced_operands, stage_m_kpack_major, stage_n_wave_specialized, stage_o_register_pool, stage_p_large_pipeline, stage_q_specialized_layout, stage_r_buffer_priority, stage_s_aspect_k64, stage_u_12_math_waves, stage_u_native_m18880, stage_v_k64_wrapped_pipeline, stage_w_mmac_order, stage_x_cooperative_prefetch, stage_y_structured_cache_swizzle, stage_z_structured_wait_tracking

部分阶段：无

更新日期：2026-10-13T22:37:45.357261+08:00
