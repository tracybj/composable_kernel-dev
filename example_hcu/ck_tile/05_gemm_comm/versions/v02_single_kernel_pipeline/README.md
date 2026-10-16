# v02：同kernel独立计算/通信CTA

根目录`operators.hpp`为标量通信初版；`vec8/operators.hpp`是16字节/lane的BF16向量通信版。GEMM保留04_gemm的v08流水，计算CTA发布ready继续下一tile，通信CTA独立等待/读对端/归约。不是把原权重loader wave改成通信wave。

标量通信76/4配置约34ms，交叠不能弥补通信吞吐不足。vec8正式72计算/8通信CTA：FC2 6.8303→6.0467ms（快11.47%），out_proj 4.6499→5.4155ms（慢16.46%）。各CTA均768线程和64KiB LDS，包括通信角色，限制资源灵活性；这是下一版拆两个kernel的动机。

完整报告`HISTORICAL_RESULTS.md`；日志、筛选、时间戳和数值门禁：[ck_v08_pipeline_20261016](https://github.com/tracybj/minimax_h3_test/tree/main/minimax_h3_bw151_repro_0518/benchmarks/ck_v08_pipeline_20261016)。不能把FC2最优配比视作所有shape最优。

在原ck_yyc环境、HCU4/5空闲且相邻04_gemm已构建v08后：

```bash
cd vec8
bash run.sh build
bash run.sh new_gate 8 0 gate fc2
bash run.sh new_fc2 8 3 full fc2
bash run.sh new_out 8 3 full out
```

参数`8`是通信CTA数，总CTA保持80，其余72计算。根目录run.sh同接口，用于重现标量版。trace使用新目录避免覆盖相同配置CSV；历史controllers/analysis依赖原日志组织，不自动对新checkout运行。
