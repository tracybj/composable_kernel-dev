# 关键版本 Git 提交索引

这些为 2026-10-16 按历史顺序创建的归档提交，原始实验发生在 10-12～14；4096³ 补充实验发生在 10-16。不伪造过去的 commit 时间。

| 提交 | 内容 |
|---|---|
| `efa38ec37829` | docs(gemm): archive all H3 gfx936 experiment reports and add reproducible harness |
| `d65c69bd6201` | feat(gemm): retain exact vectorized operand-loading milestone (~213T) |
| `7f14fdd837bd` | feat(gemm): retain K32 register-bank prefetch and wait scheduling (~240T) |
| `cd2ccbd609a4` | feat(gemm): retain interleaved prefetch and bounded output milestone (~258T) |
| `b5abb6cdfd58` | feat(gemm): retain BF16 RNE and producer-consumer cadence milestone (~274T) |
| `3a1ccb8a6ce3` | feat(gemm): retain paired tiles and static wave scheduling milestone (~301T) |
| `53006278ad70` | feat(gemm): retain scalar B-resource rebase milestone (~308T) |
| `635aee52876b` | feat(gemm): retain validated K32 cache-swizzle dispatch milestone (~330T) |

每个版本在 versions/ 下独立保留，不用切换 Git 历史即可阅读、构建和比较。恢复某次归档提交时使用隔离 worktree 或新目录，避免覆盖当前工作。
