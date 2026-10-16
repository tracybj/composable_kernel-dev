# K32 CacheSwizzle独立复测（DD/DE，2026-10-14）

两开启bit62候选各补126组/378次C偏移2-byte逐位审计，全部通过；正常对齐审计见DD报告。没有采用INT8或调用rocBLAS冒充CK。

| round | shape | candidate | ms | TFLOPS | rocBLAS linear TFLOPS | vs BASE time reduction | vs latest optional |
|---|---|---|---:|---:|---:|---:|
| DD | fc2 | cache1 | 4.435877 | 328.027 | 329.414 | +8.848% | +8.390% |
| DD | fc2 | cache1+producer | 4.380205 | 332.196 | 329.414 | +9.992% | +9.540% |
| DD | qkv | cache1 | 6.535467 | 333.967 | 328.412 | +7.862% | +7.854% |
| DD | qkv | cache1+producer | 6.567971 | 332.314 | 328.412 | +7.404% | +7.395% |
| DD | out_proj | cache1 | 2.265374 | 321.158 | 332.200 | +10.968% | +9.015% |
| DD | out_proj | cache1+producer | 2.206350 | 329.750 | 332.200 | +13.288% | +11.386% |
| DD | fc1 | cache1 | 8.796034 | 330.851 | 327.301 | +7.355% | +7.353% |
| DD | fc1 | cache1+producer | 8.816530 | 330.082 | 327.301 | +7.139% | +7.137% |
| DE | fc2 | cache1 | 4.463965 | 325.963 | 326.405 | +8.383% | +7.975% |
| DE | fc2 | cache1+producer | 4.440516 | 327.684 | 326.405 | +8.865% | +8.458% |
| DE | qkv | cache1 | 6.555819 | 332.930 | 328.253 | +7.537% | +7.567% |
| DE | qkv | cache1+producer | 6.571684 | 332.127 | 328.253 | +7.314% | +7.343% |
| DE | out_proj | cache1 | 2.266726 | 320.967 | 332.886 | +10.998% | +8.969% |
| DE | out_proj | cache1+producer | 2.204766 | 329.987 | 332.886 | +13.431% | +11.457% |
| DE | fc1 | cache1 | 8.726017 | 333.506 | 327.898 | +8.102% | +8.083% |
| DE | fc1 | cache1+producer | 8.810521 | 330.307 | 327.898 | +7.212% | +7.193% |

两轮四shape超过旧可选库的候选：[('fc2', 'plain'), ('qkv', 'plain'), ('out_proj', 'plain'), ('fc1', 'plain'), ('fc2', 'producer'), ('qkv', 'producer'), ('out_proj', 'producer'), ('fc1', 'producer')]
这里只验证直接kernel；组合分派库本身需另外验证和独立计时。不得仅由最佳shape、平均吞吐或某次最快样本宣布四shape目标完成。

