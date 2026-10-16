# v03：两个kernel，tile ready驱动的通算重叠

这是当前最新候选，不接入H3。`two_kernel.hpp`定义独立计算/通信kernel；`reference_operators.hpp`逐字节保留v02 vec8计算/通信基元；`bench.cpp`提供双stream事件依赖、六模式对照、独立参考、全输出/guard检查和时间戳。

| shape：M18880/N5376 | 计算CTA | 通信CTA×threads | BF16 v08+RCCL | two_overlap | 时间下降 |
|---|---:|---:|---:|---:|---:|
| FC2 K7168 | 72 | 32×256 | 6.8861ms | 5.4874ms | 20.31% |
| out_proj K3584 | 64 | 16×512 | 4.6682ms | 3.9263ms | 15.89% |

3个独立进程，每进程每模式15正式样本，先取双rank较慢时间，再取进程中位数，最后平均三个中位数。时间覆盖两stream最终join，非只取计算结束。原始全部对照/初筛/负收益及完整时间戳：[ck_two_kernel_20261016](https://github.com/tracybj/minimax_h3_test/tree/main/minimax_h3_bw151_repro_0518/benchmarks/ck_two_kernel_20261016)；报告快照`HISTORICAL_RESULTS.md`。

```text
compute stream: G0 → publish0 → G1 → publish1 → G2 → …
comm stream:       wait0 → R0 → wait1 → R1 → …
join stream: 等compute和comm都完成
```

two_overlap没有等待整个GEMM的gdone事件；辅助two_serial显式等待gdone，使用相同kernel隔离交叠效果。通信kernel无LDS，计算kernel768threads/64KiB LDS；CTA并非CU绑定。full partial buffer每tile独立，不循环覆盖；跨调用epoch递增，在双方完成后才覆盖输入。

在原ck_yyc环境、HCU4/5空闲且相邻04_gemm已构建v08后：

```bash
bash run.sh build
bash run.sh new_gate 72 32 256 0 gate all
bash run.sh new_fc2 72 32 256 3 full fc2
bash run.sh new_out 64 16 512 3 full out
```

tag不能复用。trace会生成固定配置CSV，使用新目录防止覆盖。依赖原CK/DTK/MPI/RCCL/dushmem，局限节点内TP2、BF16连续矩阵、无bias，不是通用通信库。历史报告中的“未提交”指测试时状态。归档只调整run.sh的路径，kernel内容保持原样；本次未重跑GPU实验。
