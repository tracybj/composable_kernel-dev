	.text
	.amdgcn_target "amdgcn-amd-amdhsa--gfx936:sramecc+"
	.protected	_Z10h3_ck_bf16N7ck_tile10GemmKernelINS_19GemmTilePartitionerINS_13TileGemmShapeINS_8sequenceIJLi128ELi128ELi32EEEENS3_IJLi4ELi1ELi1EEEENS3_IJLi32ELi64ELi32EEEELb0ELb0EEEEENS_32MmacGemmPipelineAGmemBGmemCRegV1INS_32MmacUniversalGemmPipelineProblemIttfS7_NS_18MmacTileGemmTraitsILb0ELb0ELb0ENS_13tensor_layout4gemm8RowMajorENSD_11ColumnMajorESE_Li2ELi4ELi1ELi1EEELNS_21GemmPipelineSchedulerE1ELb1ELNS_10TailNumberE10ENS_9null_typeESJ_EENS_12H3GemmPolicyEEENS_17Default2DEpilogueINS_24Default2DEpilogueProblemIftLb0ELb0ELb1ELNS_21memory_operation_enumE0EEEvEEE15GemmCommonKargsE ; -- Begin function _Z10h3_ck_bf16N7ck_tile10GemmKernelINS_19GemmTilePartitionerINS_13TileGemmShapeINS_8sequenceIJLi128ELi128ELi32EEEENS3_IJLi4ELi1ELi1EEEENS3_IJLi32ELi64ELi32EEEELb0ELb0EEEEENS_32MmacGemmPipelineAGmemBGmemCRegV1INS_32MmacUniversalGemmPipelineProblemIttfS7_NS_18MmacTileGemmTraitsILb0ELb0ELb0ENS_13tensor_layout4gemm8RowMajorENSD_11ColumnMajorESE_Li2ELi4ELi1ELi1EEELNS_21GemmPipelineSchedulerE1ELb1ELNS_10TailNumberE10ENS_9null_typeESJ_EENS_12H3GemmPolicyEEENS_17Default2DEpilogueINS_24Default2DEpilogueProblemIftLb0ELb0ELb1ELNS_21memory_operation_enumE0EEEvEEE15GemmCommonKargsE
	.globl	_Z10h3_ck_bf16N7ck_tile10GemmKernelINS_19GemmTilePartitionerINS_13TileGemmShapeINS_8sequenceIJLi128ELi128ELi32EEEENS3_IJLi4ELi1ELi1EEEENS3_IJLi32ELi64ELi32EEEELb0ELb0EEEEENS_32MmacGemmPipelineAGmemBGmemCRegV1INS_32MmacUniversalGemmPipelineProblemIttfS7_NS_18MmacTileGemmTraitsILb0ELb0ELb0ENS_13tensor_layout4gemm8RowMajorENSD_11ColumnMajorESE_Li2ELi4ELi1ELi1EEELNS_21GemmPipelineSchedulerE1ELb1ELNS_10TailNumberE10ENS_9null_typeESJ_EENS_12H3GemmPolicyEEENS_17Default2DEpilogueINS_24Default2DEpilogueProblemIftLb0ELb0ELb1ELNS_21memory_operation_enumE0EEEvEEE15GemmCommonKargsE
	.p2align	8
	.type	_Z10h3_ck_bf16N7ck_tile10GemmKernelINS_19GemmTilePartitionerINS_13TileGemmShapeINS_8sequenceIJLi128ELi128ELi32EEEENS3_IJLi4ELi1ELi1EEEENS3_IJLi32ELi64ELi32EEEELb0ELb0EEEEENS_32MmacGemmPipelineAGmemBGmemCRegV1INS_32MmacUniversalGemmPipelineProblemIttfS7_NS_18MmacTileGemmTraitsILb0ELb0ELb0ENS_13tensor_layout4gemm8RowMajorENSD_11ColumnMajorESE_Li2ELi4ELi1ELi1EEELNS_21GemmPipelineSchedulerE1ELb1ELNS_10TailNumberE10ENS_9null_typeESJ_EENS_12H3GemmPolicyEEENS_17Default2DEpilogueINS_24Default2DEpilogueProblemIftLb0ELb0ELb1ELNS_21memory_operation_enumE0EEEvEEE15GemmCommonKargsE,@function
_Z10h3_ck_bf16N7ck_tile10GemmKernelINS_19GemmTilePartitionerINS_13TileGemmShapeINS_8sequenceIJLi128ELi128ELi32EEEENS3_IJLi4ELi1ELi1EEEENS3_IJLi32ELi64ELi32EEEELb0ELb0EEEEENS_32MmacGemmPipelineAGmemBGmemCRegV1INS_32MmacUniversalGemmPipelineProblemIttfS7_NS_18MmacTileGemmTraitsILb0ELb0ELb0ENS_13tensor_layout4gemm8RowMajorENSD_11ColumnMajorESE_Li2ELi4ELi1ELi1EEELNS_21GemmPipelineSchedulerE1ELb1ELNS_10TailNumberE10ENS_9null_typeESJ_EENS_12H3GemmPolicyEEENS_17Default2DEpilogueINS_24Default2DEpilogueProblemIftLb0ELb0ELb1ELNS_21memory_operation_enumE0EEEvEEE15GemmCommonKargsE: ; @_Z10h3_ck_bf16N7ck_tile10GemmKernelINS_19GemmTilePartitionerINS_13TileGemmShapeINS_8sequenceIJLi128ELi128ELi32EEEENS3_IJLi4ELi1ELi1EEEENS3_IJLi32ELi64ELi32EEEELb0ELb0EEEEENS_32MmacGemmPipelineAGmemBGmemCRegV1INS_32MmacUniversalGemmPipelineProblemIttfS7_NS_18MmacTileGemmTraitsILb0ELb0ELb0ENS_13tensor_layout4gemm8RowMajorENSD_11ColumnMajorESE_Li2ELi4ELi1ELi1EEELNS_21GemmPipelineSchedulerE1ELb1ELNS_10TailNumberE10ENS_9null_typeESJ_EENS_12H3GemmPolicyEEENS_17Default2DEpilogueINS_24Default2DEpilogueProblemIftLb0ELb0ELb1ELNS_21memory_operation_enumE0EEEvEEE15GemmCommonKargsE
; %bb.0:
	s_load_dwordx4 s[12:15], s[4:5], 0x0
	s_load_dwordx2 s[0:1], s[4:5], 0x10
	s_load_dwordx4 s[8:11], s[4:5], 0x18
	s_load_dwordx2 s[16:17], s[4:5], 0x28
	s_lshl_b32 s19, s6, 7
	s_lshl_b32 s18, s7, 7
	s_add_u32 s20, 0, 0
	s_waitcnt lgkmcnt(0)
	s_addc_u32 s8, s8, 0x7fffffff
	s_add_u32 s3, 0, 0
	s_addc_u32 s3, s10, -1
	v_lshrrev_b32_e32 v69, 6, v0
	v_mbcnt_lo_u32_b32 v0, -1, 0
	s_add_i32 s3, s3, 1
	v_mbcnt_hi_u32_b32 v68, -1, v0
	s_add_u32 s4, 0, 0
	v_readfirstlane_b32 s22, v69
	v_lshlrev_b32_e32 v0, 3, v68
	v_lshrrev_b32_e32 v16, 2, v68
	s_addc_u32 s4, s9, 0x7fffffff
	v_and_b32_e32 v64, 24, v0
	v_lshl_or_b32 v0, s22, 5, v16
	s_mul_i32 s2, s8, s11
	s_mul_i32 s4, s4, s16
	v_add_u32_e32 v80, s19, v0
	s_add_i32 s5, s3, s2
	s_add_i32 s21, s4, s3
	v_mad_u64_u32 v[65:66], s[2:3], v80, s11, v[64:65]
	v_add_u32_e32 v79, s18, v0
	s_lshl_b32 s6, s5, 1
	v_mad_u64_u32 v[66:67], s[2:3], v79, s16, v[64:65]
	s_mov_b32 s7, 0x20000
	s_mov_b32 s4, s12
	s_mov_b32 s5, s13
	v_lshlrev_b32_e32 v8, 1, v65
	v_lshl_add_u32 v9, s11, 5, v8
	buffer_load_dwordx4 v[0:3], v8, s[4:7], 0 offen
	buffer_load_dwordx4 v[4:7], v9, s[4:7], 0 offen
	s_lshl_b32 s21, s21, 1
	s_mov_b32 s4, s14
	s_mov_b32 s5, s15
	s_mov_b64 s[26:27], s[6:7]
	s_mov_b64 s[24:25], s[4:5]
	s_mov_b32 s26, s21
	v_lshlrev_b32_e32 v17, 1, v66
	v_lshl_add_u32 v18, s16, 5, v17
	buffer_load_dwordx4 v[8:11], v17, s[24:27], 0 offen
	buffer_load_dwordx4 v[12:15], v18, s[24:27], 0 offen
	v_lshlrev_b32_e32 v77, 5, v16
	s_lshl_b32 s2, s22, 10
	v_or3_b32 v16, v77, s2, v64
	v_lshrrev_b32_e32 v17, 1, v68
	v_lshlrev_b32_e32 v16, 1, v16
	s_mov_b32 s4, 0
	v_and_b32_e32 v78, 56, v17
	s_cmp_gt_i32 s10, 32
	s_waitcnt vmcnt(3)
	ds_write_b128 v16, v[0:3]
	s_waitcnt vmcnt(2)
	ds_write_b128 v16, v[4:7] offset:1024
	s_waitcnt vmcnt(1)
	ds_write_b128 v16, v[8:11] offset:8192
	s_waitcnt vmcnt(0)
	ds_write_b128 v16, v[12:15] offset:9216
	v_lshlrev_b32_e32 v0, 5, v68
	s_cbranch_scc1 .LBB0_2
; %bb.1:
	v_and_b32_e32 v0, 0x1e0, v0
	v_add_u32_e32 v76, v0, v78
	v_or_b32_e32 v67, 0x800, v76
	v_add_u32_e32 v73, 0x200, v76
	v_or_b32_e32 v74, 0x400, v76
	v_add_u32_e32 v75, 0x600, v76
	v_add_u32_e32 v70, 0xa00, v76
	v_or_b32_e32 v71, 0xc00, v76
	v_add_u32_e32 v72, 0xe00, v76
	s_mov_b64 s[2:3], 0
	s_branch .LBB0_3
.LBB0_2:
	s_mov_b64 s[2:3], -1
                                        ; implicit-def: $sgpr4
                                        ; implicit-def: $vgpr73
                                        ; implicit-def: $vgpr74
                                        ; implicit-def: $vgpr75
                                        ; implicit-def: $vgpr67
                                        ; implicit-def: $vgpr70
                                        ; implicit-def: $vgpr71
                                        ; implicit-def: $vgpr72
                                        ; implicit-def: $vgpr76
.LBB0_3:
	s_andn2_b64 vcc, exec, s[2:3]
	v_mov_b32_e32 v3, s4
	v_mov_b32_e32 v2, s4
	v_mov_b32_e32 v1, s4
	v_mov_b32_e32 v0, s4
	v_mov_b32_e32 v7, s4
	v_mov_b32_e32 v6, s4
	v_mov_b32_e32 v5, s4
	v_mov_b32_e32 v4, s4
	v_mov_b32_e32 v11, s4
	v_mov_b32_e32 v10, s4
	v_mov_b32_e32 v9, s4
	v_mov_b32_e32 v8, s4
	v_mov_b32_e32 v15, s4
	v_mov_b32_e32 v14, s4
	v_mov_b32_e32 v13, s4
	v_mov_b32_e32 v12, s4
	v_mov_b32_e32 v19, s4
	v_mov_b32_e32 v18, s4
	v_mov_b32_e32 v17, s4
	v_mov_b32_e32 v16, s4
	v_mov_b32_e32 v23, s4
	v_mov_b32_e32 v22, s4
	v_mov_b32_e32 v21, s4
	v_mov_b32_e32 v20, s4
	v_mov_b32_e32 v27, s4
	v_mov_b32_e32 v26, s4
	v_mov_b32_e32 v25, s4
	v_mov_b32_e32 v24, s4
	v_mov_b32_e32 v31, s4
	v_mov_b32_e32 v30, s4
	v_mov_b32_e32 v29, s4
	v_mov_b32_e32 v28, s4
	v_mov_b32_e32 v35, s4
	v_mov_b32_e32 v34, s4
	v_mov_b32_e32 v33, s4
	v_mov_b32_e32 v32, s4
	v_mov_b32_e32 v39, s4
	v_mov_b32_e32 v38, s4
	v_mov_b32_e32 v37, s4
	v_mov_b32_e32 v36, s4
	v_mov_b32_e32 v43, s4
	v_mov_b32_e32 v42, s4
	v_mov_b32_e32 v41, s4
	v_mov_b32_e32 v40, s4
	v_mov_b32_e32 v47, s4
	v_mov_b32_e32 v46, s4
	v_mov_b32_e32 v45, s4
	v_mov_b32_e32 v44, s4
	v_mov_b32_e32 v51, s4
	v_mov_b32_e32 v50, s4
	v_mov_b32_e32 v49, s4
	v_mov_b32_e32 v48, s4
	v_mov_b32_e32 v55, s4
	v_mov_b32_e32 v54, s4
	v_mov_b32_e32 v53, s4
	v_mov_b32_e32 v52, s4
	v_mov_b32_e32 v59, s4
	v_mov_b32_e32 v58, s4
	v_mov_b32_e32 v57, s4
	v_mov_b32_e32 v56, s4
	v_mov_b32_e32 v60, s4
	v_mov_b32_e32 v61, s4
	v_mov_b32_e32 v62, s4
	v_mov_b32_e32 v63, s4
	s_cbranch_vccnz .LBB0_6
; %bb.4:
	v_lshlrev_b32_e32 v0, 5, v68
	v_and_b32_e32 v0, 0x1e0, v0
	v_add_u32_e32 v76, v0, v78
	v_mov_b32_e32 v0, 0x2000
	v_lshl_or_b32 v78, v76, 1, v0
	v_lshl_add_u32 v0, v80, 1, 32
	v_lshl_add_u32 v2, v79, 1, 32
	v_mul_lo_u32 v0, s11, v0
	v_mul_lo_u32 v2, s16, v2
	s_add_i32 s2, s10, 31
	v_and_b32_e32 v1, 3, v68
	s_lshr_b32 s2, s2, 5
	v_lshlrev_b32_e32 v1, 4, v1
	v_mov_b32_e32 v60, 0
	v_or_b32_e32 v67, 0x800, v76
	v_add_u32_e32 v73, 0x200, v76
	v_or_b32_e32 v74, 0x400, v76
	v_add_u32_e32 v75, 0x600, v76
	v_add_u32_e32 v70, 0xa00, v76
	v_or_b32_e32 v71, 0xc00, v76
	v_add_u32_e32 v72, 0xe00, v76
	s_add_i32 s2, s2, 1
	v_add3_u32 v79, v0, v1, 64
	v_lshl_add_u32 v65, v65, 1, 64
	v_add3_u32 v80, v2, v1, 64
	v_lshl_add_u32 v66, v66, 1, 64
	v_mov_b32_e32 v61, v60
	v_mov_b32_e32 v62, v60
	v_mov_b32_e32 v63, v60
	v_mov_b32_e32 v0, v60
	v_mov_b32_e32 v1, v60
	v_mov_b32_e32 v2, v60
	v_mov_b32_e32 v3, v60
	v_mov_b32_e32 v4, v60
	v_mov_b32_e32 v5, v60
	v_mov_b32_e32 v6, v60
	v_mov_b32_e32 v7, v60
	v_mov_b32_e32 v8, v60
	v_mov_b32_e32 v9, v60
	v_mov_b32_e32 v10, v60
	v_mov_b32_e32 v11, v60
	v_mov_b32_e32 v12, v60
	v_mov_b32_e32 v13, v60
	v_mov_b32_e32 v14, v60
	v_mov_b32_e32 v15, v60
	v_mov_b32_e32 v16, v60
	v_mov_b32_e32 v17, v60
	v_mov_b32_e32 v18, v60
	v_mov_b32_e32 v19, v60
	v_mov_b32_e32 v20, v60
	v_mov_b32_e32 v21, v60
	v_mov_b32_e32 v22, v60
	v_mov_b32_e32 v23, v60
	v_mov_b32_e32 v24, v60
	v_mov_b32_e32 v25, v60
	v_mov_b32_e32 v26, v60
	v_mov_b32_e32 v27, v60
	v_mov_b32_e32 v28, v60
	v_mov_b32_e32 v29, v60
	v_mov_b32_e32 v30, v60
	v_mov_b32_e32 v31, v60
	v_mov_b32_e32 v32, v60
	v_mov_b32_e32 v33, v60
	v_mov_b32_e32 v34, v60
	v_mov_b32_e32 v35, v60
	v_mov_b32_e32 v36, v60
	v_mov_b32_e32 v37, v60
	v_mov_b32_e32 v38, v60
	v_mov_b32_e32 v39, v60
	v_mov_b32_e32 v40, v60
	v_mov_b32_e32 v41, v60
	v_mov_b32_e32 v42, v60
	v_mov_b32_e32 v43, v60
	v_mov_b32_e32 v44, v60
	v_mov_b32_e32 v45, v60
	v_mov_b32_e32 v46, v60
	v_mov_b32_e32 v47, v60
	v_mov_b32_e32 v48, v60
	v_mov_b32_e32 v49, v60
	v_mov_b32_e32 v50, v60
	v_mov_b32_e32 v51, v60
	v_mov_b32_e32 v52, v60
	v_mov_b32_e32 v53, v60
	v_mov_b32_e32 v54, v60
	v_mov_b32_e32 v55, v60
	v_mov_b32_e32 v56, v60
	v_mov_b32_e32 v57, v60
	v_mov_b32_e32 v58, v60
	v_mov_b32_e32 v59, v60
.LBB0_5:                                ; =>This Inner Loop Header: Depth=1
	s_mov_b32 s4, s12
	s_mov_b32 s5, s13
	v_readfirstlane_b32 s3, v69
	buffer_load_dwordx4 v[81:84], v65, s[4:7], 0 offen
	buffer_load_dwordx4 v[85:88], v79, s[4:7], 0 offen
	s_mov_b32 s4, s14
	s_mov_b32 s5, s15
	s_lshl_b32 s3, s3, 10
	s_mov_b64 s[26:27], s[6:7]
	s_mov_b64 s[24:25], s[4:5]
	s_mov_b32 s26, s21
	v_or_b32_e32 v89, s3, v76
	v_lshlrev_b32_e32 v105, 1, v89
	buffer_load_dwordx4 v[89:92], v66, s[24:27], 0 offen
	buffer_load_dwordx4 v[93:96], v80, s[24:27], 0 offen
	s_waitcnt vmcnt(0) lgkmcnt(0)
	s_barrier
	ds_read_b128 v[97:100], v105
	ds_read_b128 v[101:104], v78
	ds_read_b128 v[105:108], v105 offset:1024
	ds_read_b128 v[109:112], v78 offset:5120
	ds_read_b128 v[113:116], v78 offset:6144
	s_add_i32 s2, s2, -1
	s_waitcnt lgkmcnt(3)
	v_mmac_f32_16x16x16_bf16 v[60:63], v[97:98], v[101:102], v[60:63]
	s_waitcnt lgkmcnt(2)
	v_mmac_f32_16x16x16_bf16 v[44:47], v[105:106], v[101:102], v[44:47]
	v_mmac_f32_16x16x16_bf16 v[60:63], v[99:100], v[103:104], v[60:63]
	v_mmac_f32_16x16x16_bf16 v[44:47], v[107:108], v[103:104], v[44:47]
	ds_read_b128 v[101:104], v78 offset:1024
	ds_read_b128 v[117:120], v78 offset:7168
	s_waitcnt lgkmcnt(3)
	v_mmac_f32_16x16x16_bf16 v[24:27], v[97:98], v[109:110], v[24:27]
	s_waitcnt lgkmcnt(2)
	v_mmac_f32_16x16x16_bf16 v[20:23], v[97:98], v[113:114], v[20:23]
	v_mmac_f32_16x16x16_bf16 v[8:11], v[105:106], v[109:110], v[8:11]
	s_waitcnt lgkmcnt(1)
	v_mmac_f32_16x16x16_bf16 v[56:59], v[97:98], v[101:102], v[56:59]
	v_mmac_f32_16x16x16_bf16 v[40:43], v[105:106], v[101:102], v[40:43]
	v_mmac_f32_16x16x16_bf16 v[56:59], v[99:100], v[103:104], v[56:59]
	v_mmac_f32_16x16x16_bf16 v[40:43], v[107:108], v[103:104], v[40:43]
	ds_read_b128 v[101:104], v78 offset:2048
	s_waitcnt lgkmcnt(1)
	v_mmac_f32_16x16x16_bf16 v[16:19], v[97:98], v[117:118], v[16:19]
	v_mmac_f32_16x16x16_bf16 v[4:7], v[105:106], v[113:114], v[4:7]
	v_mmac_f32_16x16x16_bf16 v[0:3], v[105:106], v[117:118], v[0:3]
	v_mmac_f32_16x16x16_bf16 v[24:27], v[99:100], v[111:112], v[24:27]
	s_waitcnt lgkmcnt(0)
	v_mmac_f32_16x16x16_bf16 v[52:55], v[97:98], v[101:102], v[52:55]
	v_mmac_f32_16x16x16_bf16 v[36:39], v[105:106], v[101:102], v[36:39]
	v_mmac_f32_16x16x16_bf16 v[52:55], v[99:100], v[103:104], v[52:55]
	v_mmac_f32_16x16x16_bf16 v[36:39], v[107:108], v[103:104], v[36:39]
	ds_read_b128 v[101:104], v78 offset:3072
	v_mmac_f32_16x16x16_bf16 v[20:23], v[99:100], v[115:116], v[20:23]
	v_mmac_f32_16x16x16_bf16 v[16:19], v[99:100], v[119:120], v[16:19]
	v_mmac_f32_16x16x16_bf16 v[8:11], v[107:108], v[111:112], v[8:11]
	v_mmac_f32_16x16x16_bf16 v[4:7], v[107:108], v[115:116], v[4:7]
	s_waitcnt lgkmcnt(0)
	v_mmac_f32_16x16x16_bf16 v[48:51], v[97:98], v[101:102], v[48:51]
	v_mmac_f32_16x16x16_bf16 v[32:35], v[105:106], v[101:102], v[32:35]
	v_mmac_f32_16x16x16_bf16 v[48:51], v[99:100], v[103:104], v[48:51]
	v_mmac_f32_16x16x16_bf16 v[32:35], v[107:108], v[103:104], v[32:35]
	ds_read_b128 v[101:104], v78 offset:4096
	v_mmac_f32_16x16x16_bf16 v[0:3], v[107:108], v[119:120], v[0:3]
	v_add_u32_e32 v79, 64, v79
	v_add_u32_e32 v65, 64, v65
	s_cmp_gt_u32 s2, 2
	s_waitcnt lgkmcnt(0)
	v_mmac_f32_16x16x16_bf16 v[28:31], v[97:98], v[101:102], v[28:31]
	v_mmac_f32_16x16x16_bf16 v[12:15], v[105:106], v[101:102], v[12:15]
	v_or_b32_e32 v97, s3, v77
	v_mmac_f32_16x16x16_bf16 v[28:31], v[99:100], v[103:104], v[28:31]
	v_mmac_f32_16x16x16_bf16 v[12:15], v[107:108], v[103:104], v[12:15]
	v_add_lshl_u32 v97, v97, v64, 1
	v_add_u32_e32 v80, 64, v80
	v_add_u32_e32 v66, 64, v66
	s_waitcnt lgkmcnt(0)
	s_barrier
	ds_write_b128 v97, v[81:84]
	ds_write_b128 v97, v[85:88] offset:1024
	ds_write_b128 v97, v[89:92] offset:8192
	ds_write_b128 v97, v[93:96] offset:9216
	s_cbranch_scc1 .LBB0_5
.LBB0_6:
	v_readfirstlane_b32 s2, v69
	s_lshl_b32 s2, s2, 10
	v_add_lshl_u32 v64, s2, v76, 1
	v_lshlrev_b32_e32 v65, 1, v76
	s_waitcnt lgkmcnt(0)
	s_barrier
	ds_read_b128 v[77:80], v64
	v_lshlrev_b32_e32 v66, 1, v73
	ds_read_b128 v[81:84], v65 offset:8192
	ds_read_b128 v[85:88], v66 offset:8192
	v_lshlrev_b32_e32 v65, 1, v74
	v_lshlrev_b32_e32 v66, 1, v75
	ds_read_b128 v[73:76], v65 offset:8192
	ds_read_b128 v[89:92], v66 offset:8192
	ds_read_b128 v[93:96], v64 offset:1024
	v_lshlrev_b32_e32 v64, 1, v67
	v_lshlrev_b32_e32 v70, 1, v70
	ds_read_b128 v[64:67], v64 offset:8192
	ds_read_b128 v[97:100], v70 offset:8192
	v_lshlrev_b32_e32 v70, 1, v71
	s_waitcnt lgkmcnt(4)
	v_mmac_f32_16x16x16_bf16 v[52:55], v[77:78], v[73:74], v[52:55]
	s_waitcnt lgkmcnt(2)
	v_mmac_f32_16x16x16_bf16 v[36:39], v[93:94], v[73:74], v[36:39]
	v_lshlrev_b32_e32 v74, 1, v72
	ds_read_b128 v[70:73], v70 offset:8192
	ds_read_b128 v[101:104], v74 offset:8192
	v_mmac_f32_16x16x16_bf16 v[60:63], v[77:78], v[81:82], v[60:63]
	v_mmac_f32_16x16x16_bf16 v[60:63], v[79:80], v[83:84], v[60:63]
	v_mmac_f32_16x16x16_bf16 v[56:59], v[77:78], v[85:86], v[56:59]
	v_mmac_f32_16x16x16_bf16 v[48:51], v[77:78], v[89:90], v[48:51]
	v_mmac_f32_16x16x16_bf16 v[44:47], v[93:94], v[81:82], v[44:47]
	v_mmac_f32_16x16x16_bf16 v[40:43], v[93:94], v[85:86], v[40:43]
	v_mmac_f32_16x16x16_bf16 v[32:35], v[93:94], v[89:90], v[32:35]
	s_waitcnt lgkmcnt(3)
	v_mmac_f32_16x16x16_bf16 v[28:31], v[77:78], v[64:65], v[28:31]
	s_waitcnt lgkmcnt(2)
	v_mmac_f32_16x16x16_bf16 v[24:27], v[77:78], v[97:98], v[24:27]
	s_waitcnt lgkmcnt(1)
	v_mmac_f32_16x16x16_bf16 v[20:23], v[77:78], v[70:71], v[20:23]
	s_waitcnt lgkmcnt(0)
	v_mmac_f32_16x16x16_bf16 v[16:19], v[77:78], v[101:102], v[16:19]
	v_mmac_f32_16x16x16_bf16 v[12:15], v[93:94], v[64:65], v[12:15]
	v_mmac_f32_16x16x16_bf16 v[8:11], v[93:94], v[97:98], v[8:11]
	v_mmac_f32_16x16x16_bf16 v[4:7], v[93:94], v[70:71], v[4:7]
	v_mmac_f32_16x16x16_bf16 v[0:3], v[93:94], v[101:102], v[0:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v64, 0x7f800000, v60
	v_mmac_f32_16x16x16_bf16 v[56:59], v[79:80], v[87:88], v[56:59]
	v_mmac_f32_16x16x16_bf16 v[52:55], v[79:80], v[75:76], v[52:55]
	v_mmac_f32_16x16x16_bf16 v[48:51], v[79:80], v[91:92], v[48:51]
	v_mmac_f32_16x16x16_bf16 v[44:47], v[95:96], v[83:84], v[44:47]
	v_mmac_f32_16x16x16_bf16 v[40:43], v[95:96], v[87:88], v[40:43]
	v_mmac_f32_16x16x16_bf16 v[36:39], v[95:96], v[75:76], v[36:39]
	v_mmac_f32_16x16x16_bf16 v[32:35], v[95:96], v[91:92], v[32:35]
	v_mmac_f32_16x16x16_bf16 v[28:31], v[79:80], v[66:67], v[28:31]
	v_mmac_f32_16x16x16_bf16 v[24:27], v[79:80], v[99:100], v[24:27]
	v_mmac_f32_16x16x16_bf16 v[20:23], v[79:80], v[72:73], v[20:23]
	v_mmac_f32_16x16x16_bf16 v[16:19], v[79:80], v[103:104], v[16:19]
	v_mmac_f32_16x16x16_bf16 v[12:15], v[95:96], v[66:67], v[12:15]
	v_mmac_f32_16x16x16_bf16 v[8:11], v[95:96], v[99:100], v[8:11]
	v_mmac_f32_16x16x16_bf16 v[4:7], v[95:96], v[72:73], v[4:7]
	v_mmac_f32_16x16x16_bf16 v[0:3], v[95:96], v[103:104], v[0:3]
	v_cmp_ne_u32_e32 vcc, s2, v64
                                        ; implicit-def: $vgpr64
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.7:
	v_bfe_u32 v64, v60, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v64, v60, v64, s4
; %bb.8:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.9:
	v_mov_b32_e32 v64, 0
	v_or_b32_e32 v65, 0x10000, v60
	v_cmp_eq_u32_sdwa vcc, v60, v64 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v64, v65, v60, vcc
; %bb.10:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v60, 0x7f800000, v61
	v_cmp_ne_u32_e32 vcc, s2, v60
                                        ; implicit-def: $vgpr65
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.11:
	v_bfe_u32 v60, v61, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v65, v61, v60, s4
; %bb.12:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.13:
	v_mov_b32_e32 v60, 0
	v_or_b32_e32 v65, 0x10000, v61
	v_cmp_eq_u32_sdwa vcc, v61, v60 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v65, v65, v61, vcc
; %bb.14:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v60, 0x7f800000, v62
	v_cmp_ne_u32_e32 vcc, s2, v60
                                        ; implicit-def: $vgpr66
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.15:
	v_bfe_u32 v60, v62, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v66, v62, v60, s4
; %bb.16:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.17:
	v_mov_b32_e32 v60, 0
	v_or_b32_e32 v61, 0x10000, v62
	v_cmp_eq_u32_sdwa vcc, v62, v60 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v66, v61, v62, vcc
; %bb.18:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v60, 0x7f800000, v63
	v_cmp_ne_u32_e32 vcc, s2, v60
                                        ; implicit-def: $vgpr67
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.19:
	v_bfe_u32 v60, v63, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v67, v63, v60, s4
                                        ; implicit-def: $vgpr60_vgpr61_vgpr62_vgpr63
; %bb.20:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.21:
	v_mov_b32_e32 v60, 0
	v_or_b32_e32 v61, 0x10000, v63
	v_cmp_eq_u32_sdwa vcc, v63, v60 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v67, v61, v63, vcc
; %bb.22:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v60, 0x7f800000, v56
	v_cmp_ne_u32_e32 vcc, s2, v60
                                        ; implicit-def: $vgpr60
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.23:
	v_bfe_u32 v60, v56, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v60, v56, v60, s4
; %bb.24:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.25:
	v_mov_b32_e32 v60, 0
	v_or_b32_e32 v61, 0x10000, v56
	v_cmp_eq_u32_sdwa vcc, v56, v60 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v60, v61, v56, vcc
; %bb.26:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v56, 0x7f800000, v57
	v_cmp_ne_u32_e32 vcc, s2, v56
                                        ; implicit-def: $vgpr61
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.27:
	v_bfe_u32 v56, v57, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v61, v57, v56, s4
; %bb.28:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.29:
	v_mov_b32_e32 v56, 0
	v_or_b32_e32 v61, 0x10000, v57
	v_cmp_eq_u32_sdwa vcc, v57, v56 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v61, v61, v57, vcc
; %bb.30:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v56, 0x7f800000, v58
	v_cmp_ne_u32_e32 vcc, s2, v56
                                        ; implicit-def: $vgpr62
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.31:
	v_bfe_u32 v56, v58, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v62, v58, v56, s4
; %bb.32:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.33:
	v_mov_b32_e32 v56, 0
	v_or_b32_e32 v57, 0x10000, v58
	v_cmp_eq_u32_sdwa vcc, v58, v56 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v62, v57, v58, vcc
; %bb.34:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v56, 0x7f800000, v59
	v_cmp_ne_u32_e32 vcc, s2, v56
                                        ; implicit-def: $vgpr63
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.35:
	v_bfe_u32 v56, v59, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v63, v59, v56, s4
                                        ; implicit-def: $vgpr56_vgpr57_vgpr58_vgpr59
; %bb.36:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.37:
	v_mov_b32_e32 v56, 0
	v_or_b32_e32 v57, 0x10000, v59
	v_cmp_eq_u32_sdwa vcc, v59, v56 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v63, v57, v59, vcc
; %bb.38:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v56, 0x7f800000, v52
	v_cmp_ne_u32_e32 vcc, s2, v56
                                        ; implicit-def: $vgpr56
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.39:
	v_bfe_u32 v56, v52, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v56, v52, v56, s4
; %bb.40:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.41:
	v_mov_b32_e32 v56, 0
	v_or_b32_e32 v57, 0x10000, v52
	v_cmp_eq_u32_sdwa vcc, v52, v56 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v56, v57, v52, vcc
; %bb.42:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v52, 0x7f800000, v53
	v_cmp_ne_u32_e32 vcc, s2, v52
                                        ; implicit-def: $vgpr57
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.43:
	v_bfe_u32 v52, v53, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v57, v53, v52, s4
; %bb.44:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.45:
	v_mov_b32_e32 v52, 0
	v_or_b32_e32 v57, 0x10000, v53
	v_cmp_eq_u32_sdwa vcc, v53, v52 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v57, v57, v53, vcc
; %bb.46:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v52, 0x7f800000, v54
	v_cmp_ne_u32_e32 vcc, s2, v52
                                        ; implicit-def: $vgpr58
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.47:
	v_bfe_u32 v52, v54, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v58, v54, v52, s4
; %bb.48:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.49:
	v_mov_b32_e32 v52, 0
	v_or_b32_e32 v53, 0x10000, v54
	v_cmp_eq_u32_sdwa vcc, v54, v52 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v58, v53, v54, vcc
; %bb.50:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v52, 0x7f800000, v55
	v_cmp_ne_u32_e32 vcc, s2, v52
                                        ; implicit-def: $vgpr59
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.51:
	v_bfe_u32 v52, v55, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v59, v55, v52, s4
                                        ; implicit-def: $vgpr52_vgpr53_vgpr54_vgpr55
; %bb.52:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.53:
	v_mov_b32_e32 v52, 0
	v_or_b32_e32 v53, 0x10000, v55
	v_cmp_eq_u32_sdwa vcc, v55, v52 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v59, v53, v55, vcc
; %bb.54:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v52, 0x7f800000, v48
	v_cmp_ne_u32_e32 vcc, s2, v52
                                        ; implicit-def: $vgpr52
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.55:
	v_bfe_u32 v52, v48, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v52, v48, v52, s4
; %bb.56:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.57:
	v_mov_b32_e32 v52, 0
	v_or_b32_e32 v53, 0x10000, v48
	v_cmp_eq_u32_sdwa vcc, v48, v52 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v52, v53, v48, vcc
; %bb.58:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v48, 0x7f800000, v49
	v_cmp_ne_u32_e32 vcc, s2, v48
                                        ; implicit-def: $vgpr53
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.59:
	v_bfe_u32 v48, v49, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v53, v49, v48, s4
; %bb.60:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.61:
	v_mov_b32_e32 v48, 0
	v_or_b32_e32 v53, 0x10000, v49
	v_cmp_eq_u32_sdwa vcc, v49, v48 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v53, v53, v49, vcc
; %bb.62:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v48, 0x7f800000, v50
	v_cmp_ne_u32_e32 vcc, s2, v48
                                        ; implicit-def: $vgpr54
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.63:
	v_bfe_u32 v48, v50, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v54, v50, v48, s4
; %bb.64:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.65:
	v_mov_b32_e32 v48, 0
	v_or_b32_e32 v49, 0x10000, v50
	v_cmp_eq_u32_sdwa vcc, v50, v48 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v54, v49, v50, vcc
; %bb.66:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v48, 0x7f800000, v51
	v_cmp_ne_u32_e32 vcc, s2, v48
                                        ; implicit-def: $vgpr55
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.67:
	v_bfe_u32 v48, v51, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v55, v51, v48, s4
                                        ; implicit-def: $vgpr48_vgpr49_vgpr50_vgpr51
; %bb.68:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.69:
	v_mov_b32_e32 v48, 0
	v_or_b32_e32 v49, 0x10000, v51
	v_cmp_eq_u32_sdwa vcc, v51, v48 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v55, v49, v51, vcc
; %bb.70:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v48, 0x7f800000, v44
	v_cmp_ne_u32_e32 vcc, s2, v48
                                        ; implicit-def: $vgpr48
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.71:
	v_bfe_u32 v48, v44, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v48, v44, v48, s4
; %bb.72:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.73:
	v_mov_b32_e32 v48, 0
	v_or_b32_e32 v49, 0x10000, v44
	v_cmp_eq_u32_sdwa vcc, v44, v48 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v48, v49, v44, vcc
; %bb.74:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v44, 0x7f800000, v45
	v_cmp_ne_u32_e32 vcc, s2, v44
                                        ; implicit-def: $vgpr49
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.75:
	v_bfe_u32 v44, v45, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v49, v45, v44, s4
; %bb.76:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.77:
	v_mov_b32_e32 v44, 0
	v_or_b32_e32 v49, 0x10000, v45
	v_cmp_eq_u32_sdwa vcc, v45, v44 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v49, v49, v45, vcc
; %bb.78:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v44, 0x7f800000, v46
	v_cmp_ne_u32_e32 vcc, s2, v44
                                        ; implicit-def: $vgpr50
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.79:
	v_bfe_u32 v44, v46, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v50, v46, v44, s4
; %bb.80:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.81:
	v_mov_b32_e32 v44, 0
	v_or_b32_e32 v45, 0x10000, v46
	v_cmp_eq_u32_sdwa vcc, v46, v44 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v50, v45, v46, vcc
; %bb.82:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v44, 0x7f800000, v47
	v_cmp_ne_u32_e32 vcc, s2, v44
                                        ; implicit-def: $vgpr51
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.83:
	v_bfe_u32 v44, v47, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v51, v47, v44, s4
                                        ; implicit-def: $vgpr44_vgpr45_vgpr46_vgpr47
; %bb.84:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.85:
	v_mov_b32_e32 v44, 0
	v_or_b32_e32 v45, 0x10000, v47
	v_cmp_eq_u32_sdwa vcc, v47, v44 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v51, v45, v47, vcc
; %bb.86:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v44, 0x7f800000, v40
	v_cmp_ne_u32_e32 vcc, s2, v44
                                        ; implicit-def: $vgpr44
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.87:
	v_bfe_u32 v44, v40, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v44, v40, v44, s4
; %bb.88:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.89:
	v_mov_b32_e32 v44, 0
	v_or_b32_e32 v45, 0x10000, v40
	v_cmp_eq_u32_sdwa vcc, v40, v44 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v44, v45, v40, vcc
; %bb.90:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v40, 0x7f800000, v41
	v_cmp_ne_u32_e32 vcc, s2, v40
                                        ; implicit-def: $vgpr45
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.91:
	v_bfe_u32 v40, v41, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v45, v41, v40, s4
; %bb.92:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.93:
	v_mov_b32_e32 v40, 0
	v_or_b32_e32 v45, 0x10000, v41
	v_cmp_eq_u32_sdwa vcc, v41, v40 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v45, v45, v41, vcc
; %bb.94:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v40, 0x7f800000, v42
	v_cmp_ne_u32_e32 vcc, s2, v40
                                        ; implicit-def: $vgpr46
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.95:
	v_bfe_u32 v40, v42, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v46, v42, v40, s4
; %bb.96:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.97:
	v_mov_b32_e32 v40, 0
	v_or_b32_e32 v41, 0x10000, v42
	v_cmp_eq_u32_sdwa vcc, v42, v40 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v46, v41, v42, vcc
; %bb.98:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v40, 0x7f800000, v43
	v_cmp_ne_u32_e32 vcc, s2, v40
                                        ; implicit-def: $vgpr47
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.99:
	v_bfe_u32 v40, v43, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v47, v43, v40, s4
                                        ; implicit-def: $vgpr40_vgpr41_vgpr42_vgpr43
; %bb.100:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.101:
	v_mov_b32_e32 v40, 0
	v_or_b32_e32 v41, 0x10000, v43
	v_cmp_eq_u32_sdwa vcc, v43, v40 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v47, v41, v43, vcc
; %bb.102:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v40, 0x7f800000, v36
	v_cmp_ne_u32_e32 vcc, s2, v40
                                        ; implicit-def: $vgpr40
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.103:
	v_bfe_u32 v40, v36, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v40, v36, v40, s4
; %bb.104:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.105:
	v_mov_b32_e32 v40, 0
	v_or_b32_e32 v41, 0x10000, v36
	v_cmp_eq_u32_sdwa vcc, v36, v40 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v40, v41, v36, vcc
; %bb.106:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v36, 0x7f800000, v37
	v_cmp_ne_u32_e32 vcc, s2, v36
                                        ; implicit-def: $vgpr41
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.107:
	v_bfe_u32 v36, v37, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v41, v37, v36, s4
; %bb.108:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.109:
	v_mov_b32_e32 v36, 0
	v_or_b32_e32 v41, 0x10000, v37
	v_cmp_eq_u32_sdwa vcc, v37, v36 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v41, v41, v37, vcc
; %bb.110:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v36, 0x7f800000, v38
	v_cmp_ne_u32_e32 vcc, s2, v36
                                        ; implicit-def: $vgpr42
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.111:
	v_bfe_u32 v36, v38, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v42, v38, v36, s4
; %bb.112:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.113:
	v_mov_b32_e32 v36, 0
	v_or_b32_e32 v37, 0x10000, v38
	v_cmp_eq_u32_sdwa vcc, v38, v36 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v42, v37, v38, vcc
; %bb.114:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v36, 0x7f800000, v39
	v_cmp_ne_u32_e32 vcc, s2, v36
                                        ; implicit-def: $vgpr43
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.115:
	v_bfe_u32 v36, v39, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v43, v39, v36, s4
                                        ; implicit-def: $vgpr36_vgpr37_vgpr38_vgpr39
; %bb.116:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.117:
	v_mov_b32_e32 v36, 0
	v_or_b32_e32 v37, 0x10000, v39
	v_cmp_eq_u32_sdwa vcc, v39, v36 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v43, v37, v39, vcc
; %bb.118:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v36, 0x7f800000, v32
	v_cmp_ne_u32_e32 vcc, s2, v36
                                        ; implicit-def: $vgpr36
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.119:
	v_bfe_u32 v36, v32, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v36, v32, v36, s4
; %bb.120:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.121:
	v_mov_b32_e32 v36, 0
	v_or_b32_e32 v37, 0x10000, v32
	v_cmp_eq_u32_sdwa vcc, v32, v36 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v36, v37, v32, vcc
; %bb.122:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v32, 0x7f800000, v33
	v_cmp_ne_u32_e32 vcc, s2, v32
                                        ; implicit-def: $vgpr37
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.123:
	v_bfe_u32 v32, v33, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v37, v33, v32, s4
; %bb.124:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.125:
	v_mov_b32_e32 v32, 0
	v_or_b32_e32 v37, 0x10000, v33
	v_cmp_eq_u32_sdwa vcc, v33, v32 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v37, v37, v33, vcc
; %bb.126:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v32, 0x7f800000, v34
	v_cmp_ne_u32_e32 vcc, s2, v32
                                        ; implicit-def: $vgpr38
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.127:
	v_bfe_u32 v32, v34, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v38, v34, v32, s4
; %bb.128:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.129:
	v_mov_b32_e32 v32, 0
	v_or_b32_e32 v33, 0x10000, v34
	v_cmp_eq_u32_sdwa vcc, v34, v32 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v38, v33, v34, vcc
; %bb.130:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v32, 0x7f800000, v35
	v_cmp_ne_u32_e32 vcc, s2, v32
                                        ; implicit-def: $vgpr39
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.131:
	v_bfe_u32 v32, v35, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v39, v35, v32, s4
                                        ; implicit-def: $vgpr32_vgpr33_vgpr34_vgpr35
; %bb.132:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.133:
	v_mov_b32_e32 v32, 0
	v_or_b32_e32 v33, 0x10000, v35
	v_cmp_eq_u32_sdwa vcc, v35, v32 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v39, v33, v35, vcc
; %bb.134:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v32, 0x7f800000, v28
	v_cmp_ne_u32_e32 vcc, s2, v32
                                        ; implicit-def: $vgpr32
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.135:
	v_bfe_u32 v32, v28, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v32, v28, v32, s4
; %bb.136:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.137:
	v_mov_b32_e32 v32, 0
	v_or_b32_e32 v33, 0x10000, v28
	v_cmp_eq_u32_sdwa vcc, v28, v32 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v32, v33, v28, vcc
; %bb.138:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v28, 0x7f800000, v29
	v_cmp_ne_u32_e32 vcc, s2, v28
                                        ; implicit-def: $vgpr33
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.139:
	v_bfe_u32 v28, v29, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v33, v29, v28, s4
; %bb.140:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.141:
	v_mov_b32_e32 v28, 0
	v_or_b32_e32 v33, 0x10000, v29
	v_cmp_eq_u32_sdwa vcc, v29, v28 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v33, v33, v29, vcc
; %bb.142:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v28, 0x7f800000, v30
	v_cmp_ne_u32_e32 vcc, s2, v28
                                        ; implicit-def: $vgpr34
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.143:
	v_bfe_u32 v28, v30, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v34, v30, v28, s4
; %bb.144:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.145:
	v_mov_b32_e32 v28, 0
	v_or_b32_e32 v29, 0x10000, v30
	v_cmp_eq_u32_sdwa vcc, v30, v28 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v34, v29, v30, vcc
; %bb.146:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v28, 0x7f800000, v31
	v_cmp_ne_u32_e32 vcc, s2, v28
                                        ; implicit-def: $vgpr35
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.147:
	v_bfe_u32 v28, v31, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v35, v31, v28, s4
                                        ; implicit-def: $vgpr28_vgpr29_vgpr30_vgpr31
; %bb.148:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.149:
	v_mov_b32_e32 v28, 0
	v_or_b32_e32 v29, 0x10000, v31
	v_cmp_eq_u32_sdwa vcc, v31, v28 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v35, v29, v31, vcc
; %bb.150:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v28, 0x7f800000, v24
	v_cmp_ne_u32_e32 vcc, s2, v28
                                        ; implicit-def: $vgpr28
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.151:
	v_bfe_u32 v28, v24, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v28, v24, v28, s4
; %bb.152:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.153:
	v_mov_b32_e32 v28, 0
	v_or_b32_e32 v29, 0x10000, v24
	v_cmp_eq_u32_sdwa vcc, v24, v28 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v28, v29, v24, vcc
; %bb.154:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v24, 0x7f800000, v25
	v_cmp_ne_u32_e32 vcc, s2, v24
                                        ; implicit-def: $vgpr29
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.155:
	v_bfe_u32 v24, v25, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v29, v25, v24, s4
; %bb.156:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.157:
	v_mov_b32_e32 v24, 0
	v_or_b32_e32 v29, 0x10000, v25
	v_cmp_eq_u32_sdwa vcc, v25, v24 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v29, v29, v25, vcc
; %bb.158:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v24, 0x7f800000, v26
	v_cmp_ne_u32_e32 vcc, s2, v24
                                        ; implicit-def: $vgpr30
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.159:
	v_bfe_u32 v24, v26, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v30, v26, v24, s4
; %bb.160:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.161:
	v_mov_b32_e32 v24, 0
	v_or_b32_e32 v25, 0x10000, v26
	v_cmp_eq_u32_sdwa vcc, v26, v24 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v30, v25, v26, vcc
; %bb.162:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v24, 0x7f800000, v27
	v_cmp_ne_u32_e32 vcc, s2, v24
                                        ; implicit-def: $vgpr31
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.163:
	v_bfe_u32 v24, v27, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v31, v27, v24, s4
                                        ; implicit-def: $vgpr24_vgpr25_vgpr26_vgpr27
; %bb.164:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.165:
	v_mov_b32_e32 v24, 0
	v_or_b32_e32 v25, 0x10000, v27
	v_cmp_eq_u32_sdwa vcc, v27, v24 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v31, v25, v27, vcc
; %bb.166:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v24, 0x7f800000, v20
	v_cmp_ne_u32_e32 vcc, s2, v24
                                        ; implicit-def: $vgpr24
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.167:
	v_bfe_u32 v24, v20, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v24, v20, v24, s4
; %bb.168:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.169:
	v_mov_b32_e32 v24, 0
	v_or_b32_e32 v25, 0x10000, v20
	v_cmp_eq_u32_sdwa vcc, v20, v24 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v24, v25, v20, vcc
; %bb.170:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v20, 0x7f800000, v21
	v_cmp_ne_u32_e32 vcc, s2, v20
                                        ; implicit-def: $vgpr25
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.171:
	v_bfe_u32 v20, v21, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v25, v21, v20, s4
; %bb.172:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.173:
	v_mov_b32_e32 v20, 0
	v_or_b32_e32 v25, 0x10000, v21
	v_cmp_eq_u32_sdwa vcc, v21, v20 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v25, v25, v21, vcc
; %bb.174:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v20, 0x7f800000, v22
	v_cmp_ne_u32_e32 vcc, s2, v20
                                        ; implicit-def: $vgpr26
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.175:
	v_bfe_u32 v20, v22, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v26, v22, v20, s4
; %bb.176:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.177:
	v_mov_b32_e32 v20, 0
	v_or_b32_e32 v21, 0x10000, v22
	v_cmp_eq_u32_sdwa vcc, v22, v20 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v26, v21, v22, vcc
; %bb.178:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v20, 0x7f800000, v23
	v_cmp_ne_u32_e32 vcc, s2, v20
                                        ; implicit-def: $vgpr27
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.179:
	v_bfe_u32 v20, v23, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v27, v23, v20, s4
                                        ; implicit-def: $vgpr20_vgpr21_vgpr22_vgpr23
; %bb.180:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.181:
	v_mov_b32_e32 v20, 0
	v_or_b32_e32 v21, 0x10000, v23
	v_cmp_eq_u32_sdwa vcc, v23, v20 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v27, v21, v23, vcc
; %bb.182:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v20, 0x7f800000, v16
	v_cmp_ne_u32_e32 vcc, s2, v20
                                        ; implicit-def: $vgpr20
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.183:
	v_bfe_u32 v20, v16, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v20, v16, v20, s4
; %bb.184:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.185:
	v_mov_b32_e32 v20, 0
	v_or_b32_e32 v21, 0x10000, v16
	v_cmp_eq_u32_sdwa vcc, v16, v20 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v20, v21, v16, vcc
; %bb.186:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v16, 0x7f800000, v17
	v_cmp_ne_u32_e32 vcc, s2, v16
                                        ; implicit-def: $vgpr21
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.187:
	v_bfe_u32 v16, v17, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v21, v17, v16, s4
; %bb.188:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.189:
	v_mov_b32_e32 v16, 0
	v_or_b32_e32 v21, 0x10000, v17
	v_cmp_eq_u32_sdwa vcc, v17, v16 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v21, v21, v17, vcc
; %bb.190:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v16, 0x7f800000, v18
	v_cmp_ne_u32_e32 vcc, s2, v16
                                        ; implicit-def: $vgpr22
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.191:
	v_bfe_u32 v16, v18, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v22, v18, v16, s4
; %bb.192:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.193:
	v_mov_b32_e32 v16, 0
	v_or_b32_e32 v17, 0x10000, v18
	v_cmp_eq_u32_sdwa vcc, v18, v16 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v22, v17, v18, vcc
; %bb.194:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v16, 0x7f800000, v19
	v_cmp_ne_u32_e32 vcc, s2, v16
                                        ; implicit-def: $vgpr23
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.195:
	v_bfe_u32 v16, v19, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v23, v19, v16, s4
                                        ; implicit-def: $vgpr16_vgpr17_vgpr18_vgpr19
; %bb.196:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.197:
	v_mov_b32_e32 v16, 0
	v_or_b32_e32 v17, 0x10000, v19
	v_cmp_eq_u32_sdwa vcc, v19, v16 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v23, v17, v19, vcc
; %bb.198:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v16, 0x7f800000, v12
	v_cmp_ne_u32_e32 vcc, s2, v16
                                        ; implicit-def: $vgpr16
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.199:
	v_bfe_u32 v16, v12, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v16, v12, v16, s4
; %bb.200:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.201:
	v_mov_b32_e32 v16, 0
	v_or_b32_e32 v17, 0x10000, v12
	v_cmp_eq_u32_sdwa vcc, v12, v16 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v16, v17, v12, vcc
; %bb.202:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v12, 0x7f800000, v13
	v_cmp_ne_u32_e32 vcc, s2, v12
                                        ; implicit-def: $vgpr17
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.203:
	v_bfe_u32 v12, v13, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v17, v13, v12, s4
; %bb.204:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.205:
	v_mov_b32_e32 v12, 0
	v_or_b32_e32 v17, 0x10000, v13
	v_cmp_eq_u32_sdwa vcc, v13, v12 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v17, v17, v13, vcc
; %bb.206:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v12, 0x7f800000, v14
	v_cmp_ne_u32_e32 vcc, s2, v12
                                        ; implicit-def: $vgpr18
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.207:
	v_bfe_u32 v12, v14, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v18, v14, v12, s4
; %bb.208:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.209:
	v_mov_b32_e32 v12, 0
	v_or_b32_e32 v13, 0x10000, v14
	v_cmp_eq_u32_sdwa vcc, v14, v12 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v18, v13, v14, vcc
; %bb.210:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v12, 0x7f800000, v15
	v_cmp_ne_u32_e32 vcc, s2, v12
                                        ; implicit-def: $vgpr19
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.211:
	v_bfe_u32 v12, v15, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v19, v15, v12, s4
                                        ; implicit-def: $vgpr12_vgpr13_vgpr14_vgpr15
; %bb.212:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.213:
	v_mov_b32_e32 v12, 0
	v_or_b32_e32 v13, 0x10000, v15
	v_cmp_eq_u32_sdwa vcc, v15, v12 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v19, v13, v15, vcc
; %bb.214:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v12, 0x7f800000, v8
	v_cmp_ne_u32_e32 vcc, s2, v12
                                        ; implicit-def: $vgpr12
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.215:
	v_bfe_u32 v12, v8, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v12, v8, v12, s4
; %bb.216:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.217:
	v_mov_b32_e32 v12, 0
	v_or_b32_e32 v13, 0x10000, v8
	v_cmp_eq_u32_sdwa vcc, v8, v12 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v12, v13, v8, vcc
; %bb.218:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v8, 0x7f800000, v9
	v_cmp_ne_u32_e32 vcc, s2, v8
                                        ; implicit-def: $vgpr13
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.219:
	v_bfe_u32 v8, v9, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v13, v9, v8, s4
; %bb.220:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.221:
	v_mov_b32_e32 v8, 0
	v_or_b32_e32 v13, 0x10000, v9
	v_cmp_eq_u32_sdwa vcc, v9, v8 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v13, v13, v9, vcc
; %bb.222:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v8, 0x7f800000, v10
	v_cmp_ne_u32_e32 vcc, s2, v8
                                        ; implicit-def: $vgpr14
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.223:
	v_bfe_u32 v8, v10, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v14, v10, v8, s4
; %bb.224:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.225:
	v_mov_b32_e32 v8, 0
	v_or_b32_e32 v9, 0x10000, v10
	v_cmp_eq_u32_sdwa vcc, v10, v8 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v14, v9, v10, vcc
; %bb.226:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v8, 0x7f800000, v11
	v_cmp_ne_u32_e32 vcc, s2, v8
                                        ; implicit-def: $vgpr15
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.227:
	v_bfe_u32 v8, v11, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v15, v11, v8, s4
                                        ; implicit-def: $vgpr8_vgpr9_vgpr10_vgpr11
; %bb.228:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.229:
	v_mov_b32_e32 v8, 0
	v_or_b32_e32 v9, 0x10000, v11
	v_cmp_eq_u32_sdwa vcc, v11, v8 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v15, v9, v11, vcc
; %bb.230:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v8, 0x7f800000, v4
	v_cmp_ne_u32_e32 vcc, s2, v8
                                        ; implicit-def: $vgpr8
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.231:
	v_bfe_u32 v8, v4, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v8, v4, v8, s4
; %bb.232:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.233:
	v_mov_b32_e32 v8, 0
	v_or_b32_e32 v9, 0x10000, v4
	v_cmp_eq_u32_sdwa vcc, v4, v8 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v8, v9, v4, vcc
; %bb.234:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v4, 0x7f800000, v5
	v_cmp_ne_u32_e32 vcc, s2, v4
                                        ; implicit-def: $vgpr9
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.235:
	v_bfe_u32 v4, v5, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v9, v5, v4, s4
; %bb.236:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.237:
	v_mov_b32_e32 v4, 0
	v_or_b32_e32 v9, 0x10000, v5
	v_cmp_eq_u32_sdwa vcc, v5, v4 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v9, v9, v5, vcc
; %bb.238:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v4, 0x7f800000, v6
	v_cmp_ne_u32_e32 vcc, s2, v4
                                        ; implicit-def: $vgpr10
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.239:
	v_bfe_u32 v4, v6, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v10, v6, v4, s4
; %bb.240:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.241:
	v_mov_b32_e32 v4, 0
	v_or_b32_e32 v5, 0x10000, v6
	v_cmp_eq_u32_sdwa vcc, v6, v4 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v10, v5, v6, vcc
; %bb.242:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v4, 0x7f800000, v7
	v_cmp_ne_u32_e32 vcc, s2, v4
                                        ; implicit-def: $vgpr11
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.243:
	v_bfe_u32 v4, v7, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v11, v7, v4, s4
                                        ; implicit-def: $vgpr4_vgpr5_vgpr6_vgpr7
; %bb.244:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.245:
	v_mov_b32_e32 v4, 0
	v_or_b32_e32 v5, 0x10000, v7
	v_cmp_eq_u32_sdwa vcc, v7, v4 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v11, v5, v7, vcc
; %bb.246:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v4, 0x7f800000, v0
	v_cmp_ne_u32_e32 vcc, s2, v4
                                        ; implicit-def: $vgpr4
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.247:
	v_bfe_u32 v4, v0, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v4, v0, v4, s4
; %bb.248:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.249:
	v_mov_b32_e32 v4, 0
	v_or_b32_e32 v5, 0x10000, v0
	v_cmp_eq_u32_sdwa vcc, v0, v4 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v4, v5, v0, vcc
; %bb.250:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v0, 0x7f800000, v1
	v_cmp_ne_u32_e32 vcc, s2, v0
                                        ; implicit-def: $vgpr6
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.251:
	v_bfe_u32 v0, v1, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v6, v1, v0, s4
; %bb.252:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.253:
	v_mov_b32_e32 v0, 0
	v_or_b32_e32 v5, 0x10000, v1
	v_cmp_eq_u32_sdwa vcc, v1, v0 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v6, v5, v1, vcc
; %bb.254:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v0, 0x7f800000, v2
	v_cmp_ne_u32_e32 vcc, s2, v0
                                        ; implicit-def: $vgpr7
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.255:
	v_bfe_u32 v0, v2, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v7, v2, v0, s4
; %bb.256:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.257:
	v_mov_b32_e32 v0, 0
	v_or_b32_e32 v1, 0x10000, v2
	v_cmp_eq_u32_sdwa vcc, v2, v0 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v7, v1, v2, vcc
; %bb.258:
	s_or_b64 exec, exec, s[2:3]
	s_mov_b32 s2, 0x7f800000
	v_and_b32_e32 v0, 0x7f800000, v3
	v_cmp_ne_u32_e32 vcc, s2, v0
                                        ; implicit-def: $vgpr5
	s_and_saveexec_b64 s[2:3], vcc
	s_xor_b64 s[2:3], exec, s[2:3]
; %bb.259:
	v_bfe_u32 v0, v3, 16, 1
	s_movk_i32 s4, 0x7fff
	v_add3_u32 v5, v3, v0, s4
                                        ; implicit-def: $vgpr0_vgpr1_vgpr2_vgpr3
; %bb.260:
	s_andn2_saveexec_b64 s[2:3], s[2:3]
; %bb.261:
	v_mov_b32_e32 v0, 0
	v_or_b32_e32 v1, 0x10000, v3
	v_cmp_eq_u32_sdwa vcc, v3, v0 src0_sel:WORD_0 src1_sel:DWORD
	v_cndmask_b32_e32 v5, v1, v3, vcc
; %bb.262:
	s_or_b64 exec, exec, s[2:3]
	v_readfirstlane_b32 s3, v69
	s_lshl_b32 s3, s3, 5
	v_lshrrev_b32_e32 v0, 16, v7
	v_lshrrev_b32_e32 v7, 16, v8
	v_lshrrev_b32_e32 v8, 16, v15
	v_lshrrev_b32_e32 v15, 16, v16
	v_lshrrev_b32_e32 v16, 16, v23
	v_lshrrev_b32_e32 v23, 16, v24
	v_lshrrev_b32_e32 v24, 16, v31
	v_lshrrev_b32_e32 v31, 16, v32
	v_lshrrev_b32_e32 v32, 16, v39
	v_lshrrev_b32_e32 v39, 16, v40
	v_lshrrev_b32_e32 v40, 16, v47
	v_lshrrev_b32_e32 v47, 16, v48
	v_lshrrev_b32_e32 v48, 16, v55
	v_lshrrev_b32_e32 v55, 16, v56
	v_lshrrev_b32_e32 v56, 16, v63
	v_and_b32_e32 v63, 15, v68
	s_add_i32 s3, s3, s19
	v_add_u32_e32 v63, s3, v63
	v_mul_lo_u32 v63, v63, s17
	s_mul_i32 s8, s8, s17
	s_mul_hi_u32 s2, s20, s17
	s_add_i32 s2, s2, s8
	v_lshrrev_b32_e32 v1, 16, v6
	v_lshrrev_b32_e32 v6, 16, v9
	v_lshrrev_b32_e32 v9, 16, v14
	v_lshrrev_b32_e32 v14, 16, v17
	v_lshrrev_b32_e32 v17, 16, v22
	v_lshrrev_b32_e32 v22, 16, v25
	v_lshrrev_b32_e32 v25, 16, v30
	v_lshrrev_b32_e32 v30, 16, v33
	v_lshrrev_b32_e32 v33, 16, v38
	v_lshrrev_b32_e32 v38, 16, v41
	v_lshrrev_b32_e32 v41, 16, v46
	v_lshrrev_b32_e32 v46, 16, v49
	v_lshrrev_b32_e32 v49, 16, v54
	v_lshrrev_b32_e32 v54, 16, v57
	v_lshrrev_b32_e32 v57, 16, v62
	v_lshrrev_b32_e32 v62, 16, v65
	v_lshrrev_b32_e32 v65, 4, v68
	v_add3_u32 v63, v65, s18, v63
	s_add_i32 s2, s9, s2
	v_lshrrev_b32_e32 v3, 16, v11
	v_lshrrev_b32_e32 v11, 16, v12
	v_lshrrev_b32_e32 v12, 16, v19
	v_lshrrev_b32_e32 v19, 16, v20
	v_lshrrev_b32_e32 v20, 16, v27
	v_lshrrev_b32_e32 v27, 16, v28
	v_lshrrev_b32_e32 v28, 16, v35
	v_lshrrev_b32_e32 v35, 16, v36
	v_lshrrev_b32_e32 v36, 16, v43
	v_lshrrev_b32_e32 v43, 16, v44
	v_lshrrev_b32_e32 v44, 16, v51
	v_lshrrev_b32_e32 v51, 16, v52
	v_lshrrev_b32_e32 v52, 16, v59
	v_lshrrev_b32_e32 v64, 16, v64
	s_lshl_b32 s2, s2, 1
	s_mov_b32 s3, 0x20000
	v_lshlrev_b32_e32 v65, 1, v63
	v_lshrrev_b32_e32 v2, 16, v4
	v_lshrrev_b32_e32 v4, 16, v10
	v_lshrrev_b32_e32 v10, 16, v13
	v_lshrrev_b32_e32 v13, 16, v18
	v_lshrrev_b32_e32 v18, 16, v21
	v_lshrrev_b32_e32 v21, 16, v26
	v_lshrrev_b32_e32 v26, 16, v29
	v_lshrrev_b32_e32 v29, 16, v34
	v_lshrrev_b32_e32 v34, 16, v37
	v_lshrrev_b32_e32 v37, 16, v42
	v_lshrrev_b32_e32 v42, 16, v45
	v_lshrrev_b32_e32 v45, 16, v50
	v_lshrrev_b32_e32 v50, 16, v53
	v_lshrrev_b32_e32 v53, 16, v58
	v_lshrrev_b32_e32 v58, 16, v61
	v_lshrrev_b32_e32 v59, 16, v60
	v_lshrrev_b32_e32 v60, 16, v67
	v_lshrrev_b32_e32 v61, 16, v66
	buffer_store_short v64, v65, s[0:3], 0 offen
	buffer_store_short v62, v65, s[0:3], 0 offen offset:8
	buffer_store_short v61, v65, s[0:3], 0 offen offset:16
	buffer_store_short v60, v65, s[0:3], 0 offen offset:24
	buffer_store_short v56, v65, s[0:3], 0 offen offset:56
	buffer_store_short v57, v65, s[0:3], 0 offen offset:48
	buffer_store_short v58, v65, s[0:3], 0 offen offset:40
	buffer_store_short v59, v65, s[0:3], 0 offen offset:32
	buffer_store_short v55, v65, s[0:3], 0 offen offset:64
	buffer_store_short v54, v65, s[0:3], 0 offen offset:72
	buffer_store_short v53, v65, s[0:3], 0 offen offset:80
	buffer_store_short v52, v65, s[0:3], 0 offen offset:88
	v_add_u32_e32 v52, 60, v63
	v_lshlrev_b32_e32 v53, 1, v52
	buffer_store_short v48, v53, s[0:3], 0 offen
	v_add_u32_e32 v48, 56, v63
	v_lshlrev_b32_e32 v53, 1, v48
	buffer_store_short v49, v53, s[0:3], 0 offen
	v_add_u32_e32 v49, 52, v63
	v_lshlrev_b32_e32 v53, 1, v49
	buffer_store_short v50, v53, s[0:3], 0 offen
	v_add_u32_e32 v50, 48, v63
	s_lshl_b32 s4, s17, 4
	v_lshlrev_b32_e32 v53, 1, v50
	v_add_lshl_u32 v50, v50, s4, 1
	buffer_store_short v51, v53, s[0:3], 0 offen
	buffer_store_short v35, v50, s[0:3], 0 offen
	v_add_lshl_u32 v35, v49, s4, 1
	buffer_store_short v34, v35, s[0:3], 0 offen
	v_add_lshl_u32 v34, v48, s4, 1
	buffer_store_short v33, v34, s[0:3], 0 offen
	v_add_u32_e32 v33, s4, v52
	v_lshlrev_b32_e32 v34, 1, v33
	buffer_store_short v32, v34, s[0:3], 0 offen
	v_subrev_u32_e32 v32, 32, v34
	buffer_store_short v36, v32, s[0:3], 0 offen
	v_subrev_u32_e32 v32, 40, v34
	buffer_store_short v37, v32, s[0:3], 0 offen
	v_subrev_u32_e32 v32, 48, v34
	buffer_store_short v38, v32, s[0:3], 0 offen
	v_subrev_u32_e32 v32, 56, v34
	buffer_store_short v39, v32, s[0:3], 0 offen
	v_add_u32_e32 v32, 0xffffffa8, v34
	buffer_store_short v43, v32, s[0:3], 0 offen
	v_add_u32_e32 v32, 0xffffffb0, v34
	buffer_store_short v42, v32, s[0:3], 0 offen
	v_add_u32_e32 v32, 0xffffffb8, v34
	buffer_store_short v41, v32, s[0:3], 0 offen
	v_subrev_u32_e32 v32, 64, v34
	buffer_store_short v40, v32, s[0:3], 0 offen
	v_add_u32_e32 v32, 0xffffffa0, v34
	buffer_store_short v44, v32, s[0:3], 0 offen
	v_add_u32_e32 v32, 0xffffff98, v34
	buffer_store_short v45, v32, s[0:3], 0 offen
	v_add_u32_e32 v32, 0xffffff90, v34
	buffer_store_short v46, v32, s[0:3], 0 offen
	v_add_lshl_u32 v32, v63, s4, 1
	v_lshrrev_b32_e32 v5, 16, v5
	buffer_store_short v47, v32, s[0:3], 0 offen
	buffer_store_short v15, v32, s[0:3], 0 offen offset:128
	buffer_store_short v14, v34, s[0:3], 0 offen offset:16
	buffer_store_short v13, v34, s[0:3], 0 offen offset:24
	buffer_store_short v12, v34, s[0:3], 0 offen offset:32
	buffer_store_short v8, v34, s[0:3], 0 offen offset:64
	buffer_store_short v9, v34, s[0:3], 0 offen offset:56
	buffer_store_short v10, v34, s[0:3], 0 offen offset:48
	buffer_store_short v11, v34, s[0:3], 0 offen offset:40
	buffer_store_short v7, v34, s[0:3], 0 offen offset:72
	buffer_store_short v6, v34, s[0:3], 0 offen offset:80
	buffer_store_short v4, v34, s[0:3], 0 offen offset:88
	buffer_store_short v3, v34, s[0:3], 0 offen offset:96
	buffer_store_short v5, v34, s[0:3], 0 offen offset:128
	buffer_store_short v0, v34, s[0:3], 0 offen offset:120
	buffer_store_short v1, v34, s[0:3], 0 offen offset:112
	v_add_u32_e32 v0, 52, v33
	v_lshlrev_b32_e32 v1, 1, v0
	v_subrev_u32_e32 v0, s4, v0
	v_lshlrev_b32_e32 v0, 1, v0
	buffer_store_short v2, v1, s[0:3], 0 offen
	v_add_u32_e32 v1, -8, v0
	buffer_store_short v19, v0, s[0:3], 0 offen
	buffer_store_short v18, v0, s[0:3], 0 offen offset:8
	buffer_store_short v17, v0, s[0:3], 0 offen offset:16
	buffer_store_short v16, v0, s[0:3], 0 offen offset:24
	buffer_store_short v20, v1, s[0:3], 0 offen
	v_add_u32_e32 v1, -16, v0
	buffer_store_short v21, v1, s[0:3], 0 offen
	v_subrev_u32_e32 v1, 24, v0
	buffer_store_short v22, v1, s[0:3], 0 offen
	v_subrev_u32_e32 v1, 32, v0
	buffer_store_short v23, v1, s[0:3], 0 offen
	v_subrev_u32_e32 v1, 64, v0
	buffer_store_short v27, v1, s[0:3], 0 offen
	v_subrev_u32_e32 v1, 56, v0
	buffer_store_short v26, v1, s[0:3], 0 offen
	v_subrev_u32_e32 v1, 48, v0
	buffer_store_short v25, v1, s[0:3], 0 offen
	v_subrev_u32_e32 v1, 40, v0
	buffer_store_short v24, v1, s[0:3], 0 offen
	v_add_u32_e32 v1, 0xffffffb8, v0
	buffer_store_short v28, v1, s[0:3], 0 offen
	v_add_u32_e32 v1, 0xffffffb0, v0
	buffer_store_short v29, v1, s[0:3], 0 offen
	v_add_u32_e32 v1, 0xffffffa8, v0
	v_add_u32_e32 v0, 0xffffffa0, v0
	buffer_store_short v30, v1, s[0:3], 0 offen
	buffer_store_short v31, v0, s[0:3], 0 offen
	s_endpgm
	.section	.rodata,#alloc
	.p2align	6, 0x0
	.amdhsa_kernel _Z10h3_ck_bf16N7ck_tile10GemmKernelINS_19GemmTilePartitionerINS_13TileGemmShapeINS_8sequenceIJLi128ELi128ELi32EEEENS3_IJLi4ELi1ELi1EEEENS3_IJLi32ELi64ELi32EEEELb0ELb0EEEEENS_32MmacGemmPipelineAGmemBGmemCRegV1INS_32MmacUniversalGemmPipelineProblemIttfS7_NS_18MmacTileGemmTraitsILb0ELb0ELb0ENS_13tensor_layout4gemm8RowMajorENSD_11ColumnMajorESE_Li2ELi4ELi1ELi1EEELNS_21GemmPipelineSchedulerE1ELb1ELNS_10TailNumberE10ENS_9null_typeESJ_EENS_12H3GemmPolicyEEENS_17Default2DEpilogueINS_24Default2DEpilogueProblemIftLb0ELb0ELb1ELNS_21memory_operation_enumE0EEEvEEE15GemmCommonKargsE
		.amdhsa_group_segment_fixed_size 16384
		.amdhsa_private_segment_fixed_size 0
		.amdhsa_kernarg_size 48
		.amdhsa_user_sgpr_count 6
		.amdhsa_user_sgpr_private_segment_buffer 1
		.amdhsa_user_sgpr_dispatch_ptr 0
		.amdhsa_user_sgpr_queue_ptr 0
		.amdhsa_user_sgpr_kernarg_segment_ptr 1
		.amdhsa_user_sgpr_dispatch_id 0
		.amdhsa_user_sgpr_flat_scratch_init 0
		.amdhsa_user_sgpr_private_segment_size 0
		.amdhsa_uses_dynamic_stack 0
		.amdhsa_system_sgpr_private_segment_wavefront_offset 0
		.amdhsa_system_sgpr_workgroup_id_x 1
		.amdhsa_system_sgpr_workgroup_id_y 1
		.amdhsa_system_sgpr_workgroup_id_z 0
		.amdhsa_system_sgpr_workgroup_info 0
		.amdhsa_system_vgpr_workitem_id 0
		.amdhsa_next_free_vgpr 121
		.amdhsa_next_free_sgpr 28
		.amdhsa_reserve_flat_scratch 0
		.amdhsa_reserve_xnack_mask 1
		.amdhsa_float_round_mode_32 0
		.amdhsa_float_round_mode_16_64 0
		.amdhsa_float_denorm_mode_32 3
		.amdhsa_float_denorm_mode_16_64 3
		.amdhsa_dx10_clamp 1
		.amdhsa_ieee_mode 1
		.amdhsa_fp16_overflow 0
		.amdhsa_exception_fp_ieee_invalid_op 0
		.amdhsa_exception_fp_denorm_src 0
		.amdhsa_exception_fp_ieee_div_zero 0
		.amdhsa_exception_fp_ieee_overflow 0
		.amdhsa_exception_fp_ieee_underflow 0
		.amdhsa_exception_fp_ieee_inexact 0
		.amdhsa_exception_int_div_zero 0
	.end_amdhsa_kernel
	.text
.Lfunc_end0:
	.size	_Z10h3_ck_bf16N7ck_tile10GemmKernelINS_19GemmTilePartitionerINS_13TileGemmShapeINS_8sequenceIJLi128ELi128ELi32EEEENS3_IJLi4ELi1ELi1EEEENS3_IJLi32ELi64ELi32EEEELb0ELb0EEEEENS_32MmacGemmPipelineAGmemBGmemCRegV1INS_32MmacUniversalGemmPipelineProblemIttfS7_NS_18MmacTileGemmTraitsILb0ELb0ELb0ENS_13tensor_layout4gemm8RowMajorENSD_11ColumnMajorESE_Li2ELi4ELi1ELi1EEELNS_21GemmPipelineSchedulerE1ELb1ELNS_10TailNumberE10ENS_9null_typeESJ_EENS_12H3GemmPolicyEEENS_17Default2DEpilogueINS_24Default2DEpilogueProblemIftLb0ELb0ELb1ELNS_21memory_operation_enumE0EEEvEEE15GemmCommonKargsE, .Lfunc_end0-_Z10h3_ck_bf16N7ck_tile10GemmKernelINS_19GemmTilePartitionerINS_13TileGemmShapeINS_8sequenceIJLi128ELi128ELi32EEEENS3_IJLi4ELi1ELi1EEEENS3_IJLi32ELi64ELi32EEEELb0ELb0EEEEENS_32MmacGemmPipelineAGmemBGmemCRegV1INS_32MmacUniversalGemmPipelineProblemIttfS7_NS_18MmacTileGemmTraitsILb0ELb0ELb0ENS_13tensor_layout4gemm8RowMajorENSD_11ColumnMajorESE_Li2ELi4ELi1ELi1EEELNS_21GemmPipelineSchedulerE1ELb1ELNS_10TailNumberE10ENS_9null_typeESJ_EENS_12H3GemmPolicyEEENS_17Default2DEpilogueINS_24Default2DEpilogueProblemIftLb0ELb0ELb1ELNS_21memory_operation_enumE0EEEvEEE15GemmCommonKargsE
                                        ; -- End function
	.section	.AMDGPU.csdata
; Kernel info:
; codeLenInByte = 8532
; NumSgprs: 32
; NumVgprs: 121
; ScratchSize: 0
; MemoryBound: 0
; FloatMode: 240
; IeeeMode: 1
; LDSByteSize: 16384 bytes/workgroup (compile time only)
; SGPRBlocks: 3
; VGPRBlocks: 30
; NumSGPRsForWavesPerEU: 32
; NumVGPRsForWavesPerEU: 121
; Occupancy: 2
; WaveLimiterHint : 0
; COMPUTE_PGM_RSRC2:SCRATCH_EN: 0
; COMPUTE_PGM_RSRC2:USER_SGPR: 6
; COMPUTE_PGM_RSRC2:TRAP_HANDLER: 0
; COMPUTE_PGM_RSRC2:TGID_X_EN: 1
; COMPUTE_PGM_RSRC2:TGID_Y_EN: 1
; COMPUTE_PGM_RSRC2:TGID_Z_EN: 0
; COMPUTE_PGM_RSRC2:TIDIG_COMP_CNT: 0
	.ident	"clang version 17.0.0 (http://10.4.0.1/dcutoolkit/driverruntime/llvm-project.git ff5bf917a679c83afe927af051a20b86fef7b07d)"
	.section	".note.GNU-stack"
	.addrsig
	.amdgpu_metadata
---
amdhsa.kernels:
  - .args:
      - .offset:         0
        .size:           48
        .value_kind:     by_value
    .fp64_status:    0
    .group_segment_fixed_size: 16384
    .kernarg_segment_align: 8
    .kernarg_segment_size: 48
    .language:       OpenCL C
    .language_version:
      - 2
      - 0
    .max_flat_workgroup_size: 256
    .name:           _Z10h3_ck_bf16N7ck_tile10GemmKernelINS_19GemmTilePartitionerINS_13TileGemmShapeINS_8sequenceIJLi128ELi128ELi32EEEENS3_IJLi4ELi1ELi1EEEENS3_IJLi32ELi64ELi32EEEELb0ELb0EEEEENS_32MmacGemmPipelineAGmemBGmemCRegV1INS_32MmacUniversalGemmPipelineProblemIttfS7_NS_18MmacTileGemmTraitsILb0ELb0ELb0ENS_13tensor_layout4gemm8RowMajorENSD_11ColumnMajorESE_Li2ELi4ELi1ELi1EEELNS_21GemmPipelineSchedulerE1ELb1ELNS_10TailNumberE10ENS_9null_typeESJ_EENS_12H3GemmPolicyEEENS_17Default2DEpilogueINS_24Default2DEpilogueProblemIftLb0ELb0ELb1ELNS_21memory_operation_enumE0EEEvEEE15GemmCommonKargsE
    .private_segment_fixed_size: 0
    .sgpr_count:     32
    .sgpr_spill_count: 0
    .symbol:         _Z10h3_ck_bf16N7ck_tile10GemmKernelINS_19GemmTilePartitionerINS_13TileGemmShapeINS_8sequenceIJLi128ELi128ELi32EEEENS3_IJLi4ELi1ELi1EEEENS3_IJLi32ELi64ELi32EEEELb0ELb0EEEEENS_32MmacGemmPipelineAGmemBGmemCRegV1INS_32MmacUniversalGemmPipelineProblemIttfS7_NS_18MmacTileGemmTraitsILb0ELb0ELb0ENS_13tensor_layout4gemm8RowMajorENSD_11ColumnMajorESE_Li2ELi4ELi1ELi1EEELNS_21GemmPipelineSchedulerE1ELb1ELNS_10TailNumberE10ENS_9null_typeESJ_EENS_12H3GemmPolicyEEENS_17Default2DEpilogueINS_24Default2DEpilogueProblemIftLb0ELb0ELb1ELNS_21memory_operation_enumE0EEEvEEE15GemmCommonKargsE.kd
    .uniform_work_group_size: 1
    .uses_dynamic_stack: false
    .vgpr_count:     121
    .vgpr_spill_count: 0
    .wavefront_size: 64
amdhsa.target:   'amdgcn-amd-amdhsa--gfx936:sramecc+'
amdhsa.version:
  - 1
  - 2
...

	.end_amdgpu_metadata
