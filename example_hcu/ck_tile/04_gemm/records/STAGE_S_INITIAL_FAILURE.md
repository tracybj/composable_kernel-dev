# S轮第一次脚本退出记录

候选均编译成功，第一次 bench 在首个 FC2 正确性阶段退出：`RuntimeError: launch error 1`。原因是 `wide512_mr4_pad16_g8_reg768` 要求 N%512=0，而 FC2/out_proj 的 N=5376，只能整除256。包装函数主动返回 hipErrorInvalidValue，未构成 GPU kernel 崩溃，也没有正式 S轮性能结果。

原环境文件保存在 `results/stage_s_incompatible_bn512_initial`。修复的是实验 bench：在调用前读取各候选构建 manifest 的 tile 对齐要求；无 tail 支持且不满足对齐者记录 `unsupported_alignment`，不计时、不当作正确。其他异常仍立即失败，不能吞掉真实 kernel 错误。

S轮重新运行仍保留四个 shape：512列候选只在 QKV/FC1 可运行；不能用其这些投影的收益外推到FC2/out_proj。此修复不改 GEMM 源码或正式库。
