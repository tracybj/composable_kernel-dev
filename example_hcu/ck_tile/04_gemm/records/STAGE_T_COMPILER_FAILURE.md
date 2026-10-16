# T轮调度策略编译器失败

本机 DTK `--help-hidden` 列出了 `--misched=gcn-max-ilp` 与 `gcn-max-occupancy`。只在独立候选编译命令中尝试，没有切换系统编译器或运行库。

- `large_pad16_g8_ilp_reg768`：编译器在 Machine Instruction Scheduler 中崩溃，`malloc(): invalid size (unsorted)`，Aborted。
- `large_pad16_g8_occupancy_reg768`：编译器崩溃，Floating point exception。

两个候选没有成功生成可测试的新库，未运行 GPU 测试，没有性能或正确性结论。构建命令、冻结源码、SHA、日志和退出码在 `build/attempts` 与候选 `.json/.log`。不能将“帮助中有该选项”当成此 gfx936 工具链能够稳定支持。

继续使用已验证的默认调度器和寄存器池选项；本轮不升级 DTK、不修改正式环境。
