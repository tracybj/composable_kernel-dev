# 05_gemm_comm：H3形状上的BF16通算实验

本目录与`../04_gemm`纯GEMM优化分开维护，**不是H3正式接入，不替换LightOp INT8，不包含新增模型测试**。

## 从哪里看

| 版本 | 结构、关键源码 | 结果与限制 |
|---|---|---|
| [v01_direct_comm](versions/v01_direct_comm/README.md) | `bench.cpp`：v08 GEMM后，同CTA完成dushmem通知/对端读取/归约 | out_proj约快3.71%，FC2无收益；不声称同CTA内持续重叠 |
| [v02_single_kernel_pipeline](versions/v02_single_kernel_pipeline/README.md) | `operators.hpp`标量版；`vec8/operators.hpp`向量版；计算/通信CTA同kernel分工 | FC2约快11.47%，相同72/8配置out_proj慢16.46%；负结果保留 |
| [v03_two_kernel_tiles](versions/v03_two_kernel_tiles/README.md) | **最新候选**：`two_kernel.hpp`与`bench.cpp`；双kernel/双stream，tile ready驱动通信 | FC2约快20.31%，out_proj约快15.89%，均相对同轮BF16 v08+RCCL |

HCU4/5、gfx936，单节点TP2；M18880、N5376、K7168/3584。FP32累加、BF16输入/局部输出/归约输出。最终两节点是同轮算子段，不是裸GEMM，也不是INT8 E2E。当前INT8参照更快，不能据此直接替换模型。

完整原始日志、正式样本、筛选/失败记录、时间戳、环境/库哈希统一位于 [H3实验总索引](https://github.com/tracybj/minimax_h3_test/blob/main/minimax_h3_bw151_repro_0518/benchmarks/COMMUNICATION_EXPERIMENTS.md)。三个目录的`HISTORICAL_RESULTS.md`是当时报告快照，其中相对链接/“未commit”表述按原实验语境解释；当前入口为本文件及各版本`README.md`。

## 复现环境

在原`ck_yyc`环境中，CK依赖原实验提交`5c8ea51ed7ac03bdcc24aeb811d51acc57233b60`，不能把其它CK头文件重建的库冒充原二进制。

```bash
# 在本仓库的04_gemm先重建参考v08；--ck-root使用原依赖checkout
cd example_hcu/ck_tile/04_gemm
python3 build.py --version v08_ck_tile --ck-root /workspace/composable_kernel
cd ../05_gemm_comm
python3 verify_archive.py
```

各`run.sh`支持`CK_ROOT`（默认`/workspace/composable_kernel`）和`V08_ROOT`（默认相邻`04_gemm`）。GPU限定物理4/5，执行前检查空闲，进程600秒有界；不停止其它任务。DTK、MPI、RCCL、dushmem使用原有安装；无库文件/模型文件随Git分发。编译和最终设备链接均须扩展寄存器选项`--enable-num-vgprs-768=true`。

本次归档只调整run.sh的目录定位，没有修改任何`.cpp/.hpp`计算或通信代码；在本地与服务器历史快照逐文件核实一致。可重建不代表跨工具链二进制哈希相同，重跑必须记录自己的环境/哈希，不覆盖旧证据。

先阅读版本README，再手动执行单个bench。历史controller/capture/summarize脚本保留用于解释原日志；其中环境路径和结果位置保留实验语境，不自动用于新checkout，也不要在源码目录直接重跑汇总去覆盖历史报告。

## 范围

三代是独立实验，不是生产算子库。无SP、无多TP组并发、无跨节点、无真实模型/动态batch/异常恢复验证。CTA编号不是CU编号；未证明物理CU绑定或跨kernel同CU共驻。最终双kernel使用整张partial buffer，无循环覆盖；tile ready协议不能直接推广到任意GPU和调度条件。

`ARCHIVE_MANIFEST.json`和`verify_archive.py`提供CPU-only完整性检查。原始证据集中在H3仓库，避免两仓库重复提交大文件。build、缓存和后续新跑出的log/csv/json均忽略，不自动纳入Git。
