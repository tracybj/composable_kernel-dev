# v08 CTA 级计算/通信流水

完整结论：[RESULTS.md](RESULTS.md)。结构化结果：[comparison.json](comparison.json)。

本目录是独立 TP2 BF16 微基准，不加载 H3、不修改共享 CK 或模型、不 commit/push。

- 根目录：标量通信初版，保留 2/4/8 通信 CTA 的负结果及 76+4 时间戳。
- `vec8/`：只把通信归约读写改为每 lane 连续 8 个 BF16，沿用同一个 v08 GEMM；扫描 2/4/8/16/24 通信 CTA，按 FC2 选择 72+8 后，另起三个独立进程复测两个 shape。
- `operators.hpp`：计算、发布、设备通信、独立计算/通信 CTA 角色。
- `bench.cpp`：v08+RCCL、serial80、相同角色关闭交叠、角色流水、compute-only 对照；周期/随机输入完整输出与 guard 校验；独立时间戳采集。
- `vec8/controller.py`：已经完成的有界测试批次，`COMPLETE` 表示结束；不要误当作后台 H3 服务。
- `formal_a/b/c.log`：最终性能，trace/gate/screen 不混入正式统计。
- `analysis.json` / `trace_*.csv`：逐样本统计及同 GPU 时间戳，通信区间包含等待/求和，不代表物理链路忙碌。
- `environment.json`：源码/库/二进制哈希、Git revision、工具链、测试后设备状态；`*_resources.txt` 为本地工具元数据原始输出。

复现需要现有 ck_yyc、gfx936、DTK/MPI/RCCL/dushmem、`/workspace/composable_kernel/include` 和 `/workspace/composable_kernel_publish_20261016/example_hcu/ck_tile/04_gemm` 的冻结 v08 库/源码。进入对应版本目录：

```bash
bash run.sh build
bash run.sh fresh_gate 8 0 gate all
bash run.sh fresh_formal 8 3 full all
bash run.sh fresh_trace 8 1 trace all
```

先确认 HCU4/5 空闲，不能并行启动上述命令。脚本检查占用、拒绝覆盖日志，并带 600 秒超时。已有相同配比 trace 时应复制到新实验目录再采集，不覆盖本轮证据。编译和最终设备链接都保留扩展寄存器选项。

`analyze.py`、`summarize.py`、`capture.py` 为 CPU/只读设备信息工具，不发起 GPU 工作负载。重新总结先分别运行根目录和 vec8 的 analyze.py，再运行根目录 summarize.py；controller 镜像日志不重复计数。

结论：72+8 单 kernel 流水对 FC2 相比同轮 v08+RCCL 时间下降约 11.47%；同配置 out_proj 反而慢约16.46%。out_proj 的向量化 serial80 对照约4.29ms，比4.65ms基线快约7.75%，但不是独立角色流水收益。尚无 H3 E2E 或 INT8 替换结论；两 kernel 流水仅是后续建议，尚未实现。
