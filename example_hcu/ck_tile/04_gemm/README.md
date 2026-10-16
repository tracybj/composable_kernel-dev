# 04_gemm：H3 BF16 GEMM 迭代归档

四类 H3 单 rank GEMM 在 gfx936 上从约 65–86T 迭代到约 329–333 TFLOPS；这里只是 BF16 单算子，不是 INT8 或 H3 端到端收益。

## 先看哪里

- **当前最快实现**：[v07_struct32_330t](versions/v07_struct32_330t/README.md)。入口是该目录的 `src/struct32_dispatch.cpp`，主要流水是 `src/rolephase.cpp`。
- **关键版本与每次改动**：下面的版本表；每个版本有独立源码、精确编译宏、源文件 SHA 和历史性能。
- **全部实验经过**：[优化总览](records/experiment_review_20261014/GEMM_实验迭代总览_20261012-14.md)、[112 阶段记录](records/experiment_review_20261014/GEMM_112阶段逐项记录.md)。包含失败、负收益及 profiler 分析。
- **最终四 shape 验收**：[最终报告](records/FINAL_RESULTS_330T_20261014.md)；**4096³**：[约 287T 的补充测试](records/RESULTS_4096_20261016.md)。
- **整理与恢复说明**：[ARCHIVE.md](ARCHIVE.md)。

## 关键代码版本

下表为历史实测，均 M18880；单位 TFLOPS。早期约 70T 的构建没有独立冻结源码，因此保留完整报告/构建记录，不编造一个精确原版。7 个精确源码快照分别提交，Git commit 时间为 2026-10-16 归档时间。

| 版本 | 主要改动 | FC2 | QKV | out_proj | FC1 |
|---|---|---:|---:|---:|---:|
| [v01_vector_213t](versions/v01_vector_213t/README.md) | 恢复连续 K 向量读取 | 206.14 | 212.61 | 203.08 | 212.49 |
| [v02_banked_240t](versions/v02_banked_240t/README.md) | K32 双寄存器 bank 与完成等待 | 239.08 | 239.81 | 222.53 | 239.82 |
| [v03_prefetch_258t](versions/v03_prefetch_258t/README.md) | 交错预取与有界输出 | 255.54 | 258.33 | 243.97 | 258.37 |
| [v04_cadence_274t](versions/v04_cadence_274t/README.md) | RNE 与 producer/consumer 同步节奏 | 271.48 | 274.17 | 258.80 | 273.16 |
| [v05_static_301t](versions/v05_static_301t/README.md) | 双 tile 展开与静态 wave 分组 | 296.79 | 301.05 | 282.98 | 299.78 |
| [v06_rebase_308t](versions/v06_rebase_308t/README.md) | B 公共 K 偏移的标量 rebase | 298.73 | 307.74 | 285.51 | 306.52 |
| [v07_struct32_330t](versions/v07_struct32_330t/README.md) | K32 Cache Swizzle 与精确 shape 分发 | 329.14 | 333.18 | 329.74 | 331.63 |

## 构建和测试

在原实验的 `ck_yyc` 环境运行。命令不安装包、不切换工具链。

```bash
cd example_hcu/ck_tile/04_gemm
python3 verify_archive.py
python3 build.py --version v07_struct32_330t
HIP_VISIBLE_DEVICES=4 OMP_NUM_THREADS=4 python3 bench.py --tag my_h3_run
```

默认使用当前 checkout 的 CK include。原实验依赖 CK `5c8ea51ed7ac03bdcc24aeb811d51acc57233b60`；目标仓库主线与它不同。重建历史库时显式加 `--ck-root /workspace/composable_kernel` 指向原实验 CK，避免把不同依赖产生的二进制冒充原库。归档的编译/数值验证结果见 [VALIDATION.md](VALIDATION.md)。

基准默认 5 次预热、6 轮轮换顺序、每轮 10 次，HIP event 计时，取轮均值中位数；同一 buffer 重复运行，属于 hot-cache。输入/输出 BF16，FP32 累加，`Y[M,N] = X[M,K] @ W[N,K].T`，无 bias。吞吐 `2MNK/(ms×10^9)`。

构建产物和新测试输出进入忽略的 `build/`。原来的各阶段脚本、数百份重复/失败候选代码及二进制不再散放在例子目录中；所有报告保留，全部原文件另有完整备份。
