# 两 kernel、CTA/tile 级计算通信重叠

结果入口：[RESULTS.md](RESULTS.md)。结构化数据：[comparison.json](comparison.json)。

这是独立 BF16 TP2 微基准，不是 H3 模型测试或正式 SGLang 修改。没有 commit/push，所有 GPU 作业已结束。

## 最终结果

三次全新独立进程，每模式每进程15个正式样本，先取进程中位数再平均：

| 节点 | compute CTA | communication CTA × threads | 同轮v08+RCCL | 两kernel重叠 | 时间减少 |
|---|---:|---:|---:|---:|---:|
| FC2 | 72 | 32×256 | 6.8861 ms | 5.4874 ms | 20.31% |
| out_proj | 64 | 16×512 | 4.6682 ms | 3.9263 ms | 15.89% |

这里只比较 BF16；不能当作 H3 E2E 收益或替换当前 LightOp INT8 的结论。

## 实现

- `reference_operators.hpp`：上一轮向量化单 kernel 实现的逐字节快照，提供冻结计算流水与通信方法，也作为本轮单 kernel 对照。
- `two_kernel.hpp`：独立 `compute_kernel` 和 `communication_kernel`。计算逐 tile 发布 ready 后继续推进，通信只等对应 tile。
- `bench.cpp`：两条非默认 stream 的启动/最终join；`two_overlap`没有等待整个GEMM，只有辅助 `two_serial` 使用 gdone event；六种对照、资源查询、完整输出/guard校验与独立时间戳。
- 通信kernel无LDS，threads及寄存器独立于768-thread/64KiB LDS计算kernel。全尺寸partial buffer不循环覆盖；通信CTA最多32，生产者不等消费者，不依赖循环缓冲释放。

## 记录

- `controller.py`、`selection.json`、`formal_*`：首轮7配置及负结果，保留不覆盖。
- `extended_controller.py`、`selection_extended.json`：补充4组通信并发，按每节点性能和双rank真实交叠门禁选参数。
- **`final_fc2_a/b/c.log`、`final_out_a/b/c.log`**：最终独立正式复测。screen/gate/trace不混入其均值。
- `trace_*.csv`：同GPU wall_clock64时间戳。选定two_overlap通信早于整个GEMM结束，且同一生产者G(next)/R(current)存在交叠；相同实现two_serial交集为0。
- `analysis.json` / `comparison.json`：原始样本、均值、中位数、范围、初筛、门禁和时间戳验证；controller镜像日志不重复计数。
- `environment.json`：源码/库/可执行文件SHA、Git revision、编译器、资源查询和测试后显存。所有结果均来自同一冻结实验可执行文件。

## 复现

在现有ck_yyc中进入本目录，确认HCU4/5空闲，不要并行运行：

```bash
bash run.sh build
# 随机完整输出门禁
bash run.sh fresh_gate 72 32 256 0 gate all
# FC2：M18880 N5376 K7168
bash run.sh fresh_fc2 72 32 256 3 full fc2
# out_proj：M18880 N5376 K3584
bash run.sh fresh_out 64 16 512 3 full out
```

参数依次为：tag、计算CTA数、通信CTA数、通信threads、正式轮数、kind、shape。kind可为smoke/gate/full/trace。相同配置已有trace文件时使用新的实验目录，不能覆盖本轮证据。

依赖 `/workspace/composable_kernel/include`、现有DTK/MPI/RCCL/dushmem，以及 `/workspace/composable_kernel_publish_20261016/example_hcu/ck_tile/04_gemm` 的冻结v08库/源码。编译和最终设备链接都保留扩展寄存器选项。过程环境仅对本次进程生效，不更换库。GPU程序限时600秒，仅管理自己的mpirun。

`analyze.py`、`summarize.py`、`capture.py` 只做CPU分析和只读设备信息采集；`build/`不归档到Git。

## 限制

单节点TP2、连续BF16、当前两个shape、无bias；没有SP通信、模型checkpoint/真实激活、INT8、生产并发、异常恢复或E2E验证。时间戳的通信阶段含signal/wait/归约，不是物理链路忙碌率。资源独立与已观察到的GPU级交叠，不等于已经证明两kernel在同一个CU共驻。
