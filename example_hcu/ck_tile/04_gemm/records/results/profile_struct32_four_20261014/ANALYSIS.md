# 最终K32 CacheSwizzle：四shape counter对照（2026-10-14）

同一HCU4、M18880，每实现3次；比较原22ef、最终统一入口和rocBLAS。库SHA与DG未插桩正式计时一致；各shape命令、capture.log/results.txt、DTK waits.csv及原始工具metric定义保留。

| shape | wait metric | BASE mean | optimized mean | rocBLAS mean | optimized/BASE | optimized/rocBLAS |
|---|---|---:|---:|---:|---:|---:|
| fc2 | SQ_WAIT_CNT_VM | 194,007,330 | 215,182,996 | 109,450,139 | 1.109 | 1.966 |
| fc2 | SQ_WAIT_CNT_LGKM | 89,690,957 | 72,060,997 | 156,891,451 | 0.803 | 0.459 |
| fc2 | SQ_WAIT_BARRIER | 366,184,302 | 228,617,251 | 477,080,535 | 0.624 | 0.479 |
| fc2 | SQ_WAIT_INST_VMEM | 123,283,970 | 115,984,493 | 175,132,760 | 0.941 | 0.662 |
| fc2 | SQ_WAIT_INST_LDS | 82,075,758 | 47,212,647 | 53,121,367 | 0.575 | 0.889 |
| qkv | SQ_WAIT_CNT_VM | 281,049,367 | 303,442,596 | 106,791,536 | 1.080 | 2.841 |
| qkv | SQ_WAIT_CNT_LGKM | 137,086,598 | 111,015,880 | 235,314,612 | 0.810 | 0.472 |
| qkv | SQ_WAIT_BARRIER | 525,509,606 | 350,202,899 | 742,567,341 | 0.666 | 0.472 |
| qkv | SQ_WAIT_INST_VMEM | 160,366,759 | 137,771,584 | 262,008,939 | 0.859 | 0.526 |
| qkv | SQ_WAIT_INST_LDS | 124,036,384 | 71,670,834 | 105,979,267 | 0.578 | 0.676 |
| out_proj | SQ_WAIT_CNT_VM | 180,259,372 | 87,684,215 | 45,064,950 | 0.486 | 1.946 |
| out_proj | SQ_WAIT_CNT_LGKM | 56,449,226 | 39,095,398 | 81,440,432 | 0.693 | 0.480 |
| out_proj | SQ_WAIT_BARRIER | 187,259,139 | 102,742,852 | 210,561,026 | 0.549 | 0.488 |
| out_proj | SQ_WAIT_INST_VMEM | 121,185,238 | 50,002,394 | 90,269,488 | 0.413 | 0.554 |
| out_proj | SQ_WAIT_INST_LDS | 40,840,404 | 25,198,545 | 32,488,465 | 0.617 | 0.776 |
| fc1 | SQ_WAIT_CNT_VM | 382,789,830 | 384,621,516 | 152,763,032 | 1.005 | 2.518 |
| fc1 | SQ_WAIT_CNT_LGKM | 180,639,897 | 150,822,837 | 312,031,728 | 0.835 | 0.483 |
| fc1 | SQ_WAIT_BARRIER | 702,729,940 | 449,875,494 | 1,009,554,606 | 0.640 | 0.446 |
| fc1 | SQ_WAIT_INST_VMEM | 217,197,134 | 167,542,221 | 356,751,214 | 0.771 | 0.470 |
| fc1 | SQ_WAIT_INST_LDS | 164,817,119 | 95,422,862 | 134,050,326 | 0.579 | 0.712 |

## 解读边界

首次DTK配置沿用旧kernel过滤器，仅捕获原版和rocBLAS共6项，缺少命名空间内的新kernel；没有将缺失当作0。原始文件保留，补上h3_struct32过滤后仅重采DTK waits，每shape9项，来自profile_struct32_waits_corrected_20261014；xprof原有9项完整，不重跑正式计时。
等待counter为wave聚合，不能相加或当毫秒占比；SQ指令/bank-conflict等部分由安装工具emulated，L2CacheHit不是全部HBM流量。ROCprof gpu-id为HSA agent索引，物理卡由HIP_VISIBLE_DEVICES=4和xprof gpu-id(4)核对。
性能结论来自DF/DG未插桩HIP event，不能用profile时长排名，不能把单一等待比值解释为全部收益来源。详细其余metric、各样本范围和原生符号在analysis.json。

