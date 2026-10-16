# v08 GEMM 设备端 TP2 通算融合实验

结论与数据入口：[RESULTS.md](RESULTS.md)。CPU 汇总：[comparison.json](comparison.json)。

这是独立算子实验，不是 H3 正式优化提交；没有启动 H3、没有修改模型源码或环境库，也没有 commit/push。

- 比较 FC2 `[18880,5376,7168]` 与 out_proj `[18880,5376,3584]` 的 GEMM + TP2 Sum。
- HCU4/5、ck_yyc；`full_b.log` / `full_c.log` 是两个独立正式计时进程。
- `bench_timing.cpp` 是正式计时源快照。`bench.cpp` 保持相同计算/通信实现，增加 `H3_RANDOM_GATE` 编译期验证分支；该分支默认不编译进计时程序。
- `tile_fused` 用设备 put+signal 发 packed tile；`direct_fused` 用 dushmem_ptr 的对端映射和设备 signal 实现直接读取归约。两者每 CTA 内仍按 tile 顺序计算后通信，没有宣称同 CTA 内持续计算/通信重叠。
- `random_gate.log` 仅用于全输出逐 bit 校验，不使用其时间作结论。
- `current_backend.json` 是 sglang_dev_yyc 的已安装 LightOp W8A8 接口参照；`current_backend_timing.py` 是该次测量源码快照，`current_backend.py` 仅增加可选报告文件名。
- `environment.json` 保存源码、二进制、库哈希及测试后设备状态；失败 smoke 和编译诊断保留，未混入最终统计。

## 复现

确认 HCU4/5 空闲；在 ck_yyc 内进入本目录：

```bash
bash run.sh build
bash run.sh build_gate
bash run.sh new_smoke 80 1 smoke
bash run.sh new_random_gate 80 1 gate
bash run.sh new_full 80 3 full
```

依赖原有 `/opt/dtk`、MPI、dushmem、RCCL、`/workspace/composable_kernel/include`，以及 `/workspace/composable_kernel_publish_20261016/example_hcu/ck_tile/04_gemm` 的 v08 源码和已验证 `.so`。没有安装新依赖。编译和最终设备链接均需 `-mllvm --enable-num-vgprs-768=true`，后者遗漏会造成此次实验中观察到的错误输出。

在 sglang_dev_yyc 内进入对应目录可运行 `bash run_current.sh new_reference`，不加载 H3 模型。性能报告默认仍读取本次冻结的 `full_b/full_c` 和 `current_backend.json`，重测数据应另行汇总，不覆盖本轮证据。

`python3 summarize.py` 仅重新生成 CPU 报告。GPU 程序有 600 秒超时，不应并行运行这些命令。

## 限制

当前只实现节点内 TP2、BF16 连续输入/输出、无 bias 的两个真实 shape。需要具备 peer mapping；不涵盖 TP4/8、SP All-to-All、生产请求并发、异常恢复或 INT8 替换。所有候选均包含恢复行主序输出的成本。归约前后的 BF16 舍入语义保留；随机 gate 与原 v08+RCCL 参考逐 bit 一致，不等同视频画质或模型 E2E 验收。
