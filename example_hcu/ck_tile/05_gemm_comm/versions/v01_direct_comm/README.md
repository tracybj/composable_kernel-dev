# v01：同CTA计算后通信

入口`bench.cpp`；原计时源码快照`bench_timing.cpp`。包括packed tile和peer direct两条路径，以及split/fused对照；M尾块、完整输出、guard与随机门禁。它把通信放进kernel，但同CTA仍先算当前tile再通信，不是同CTA内持续并行。

历史同轮BF16 v08+RCCL→direct_fused：FC2 6.8301→6.9249ms（慢1.39%），out_proj 4.6566→4.4838ms（快3.71%）。完整报告见`HISTORICAL_RESULTS.md`；原始证据见[ck_v08_comm_20261016](https://github.com/tracybj/minimax_h3_test/tree/main/minimax_h3_bw151_repro_0518/benchmarks/ck_v08_comm_20261016)。

在原ck_yyc环境、HCU4/5空闲且相邻04_gemm已构建v08后：

```bash
bash run.sh build
bash run.sh build_gate
bash run.sh new_gate 80 1 gate
bash run.sh new_formal 80 3 full
```

旧`current_backend*.py/run_current.sh`仅作当时sglang_dev_yyc中的INT8工程参照，保留原环境绝对路径，不能在ck_yyc直接执行。源码与证据哈希可对照历史environment.json；归档只调整run.sh定位，不改kernel。
