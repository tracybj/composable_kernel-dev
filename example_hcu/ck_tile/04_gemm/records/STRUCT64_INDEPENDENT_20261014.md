# CacheSwizzle独立复测（DA，2026-10-14）

两开启bit62候选各补126组/378次C偏移2-byte原版逐位审计，全部通过；初轮正常对齐审计另计。此处复测四shape，不能用初轮最快单次替代。

| round | shape | candidate | ms | TFLOPS | time reduction vs BASE | vs validated dispatch | wins/6 vs BASE |
|---|---|---|---:|---:|---:|---:|
| CZ | fc2 | cache1 / wait0 | 5.004804 | 290.738 | -2.713% | -3.305% | 0 |
| CZ | fc2 | cache1 / wait1/4 | 4.897420 | 297.113 | -0.509% | -1.089% | 1 |
| CZ | qkv | cache1 / wait0 | 7.399978 | 294.951 | -4.463% | -4.458% | 0 |
| CZ | qkv | cache1 / wait1/4 | 7.419330 | 294.182 | -4.736% | -4.731% | 0 |
| CZ | out_proj | cache1 / wait0 | 2.490614 | 292.114 | +2.330% | +1.870% | 6 |
| CZ | out_proj | cache1 / wait1/4 | 2.494270 | 291.686 | +2.187% | +1.726% | 6 |
| CZ | fc1 | cache1 / wait0 | 9.891577 | 294.207 | -4.141% | -4.138% | 0 |
| CZ | fc1 | cache1 / wait1/4 | 9.899112 | 293.984 | -4.220% | -4.217% | 0 |
| DA | fc2 | cache1 / wait0 | 4.988060 | 291.714 | -2.420% | -2.961% | 0 |
| DA | fc2 | cache1 / wait1/4 | 4.899620 | 296.980 | -0.604% | -1.136% | 1 |
| DA | qkv | cache1 / wait0 | 7.410538 | 294.531 | -4.533% | -4.571% | 0 |
| DA | qkv | cache1 / wait1/4 | 7.409626 | 294.567 | -4.520% | -4.558% | 0 |
| DA | out_proj | cache1 / wait0 | 2.491382 | 292.024 | +2.159% | +1.839% | 6 |
| DA | out_proj | cache1 / wait1/4 | 2.493918 | 291.727 | +2.060% | +1.739% | 6 |
| DA | fc1 | cache1 / wait0 | 9.901369 | 293.917 | -4.295% | -4.316% | 0 |
| DA | fc1 | cache1 / wait1/4 | 9.887056 | 294.342 | -4.144% | -4.166% | 0 |

## 复测判定

满足两轮均超过原库和可选分流库0.3%、每轮至少5/6轮更快的shape/candidate如下（不是全局默认变更）：

- out_proj：rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_brow2_sb64c1_reg768
- out_proj：rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_pwait_brow2_sb64c1_reg768

若采用仅做精确N/K分派，另需验证分派库本身；不能把其他shape回退的cache核全局启用。无H3/INT8/E2E收益结论，330目标未达成。

