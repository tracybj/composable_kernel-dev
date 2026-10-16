# Producer 按投影选择：已验证的小收益

基于04_gemm当前有效22ef版本，不调用rocBLAS代替自研计算，不量化。BF16输入/输出、FP32累加；物理HCU4，ck_yyc，gfx936、DTK25.10.0-0。**约330TFLOPS目标仍未完成。**

## 实现

- `shape_dispatch.cpp`复用同一`rolephase.cpp`，分别生成原始和producer双tile+scalar-offset两套内核。N=5376且K=7168/3584选择后者，其余合法尺寸走原始路径。没有让回退的QKV/FC1使用新producer。
- 新producer按两K64 tile展开，LDS槽固定1/0；公共K byte偏移进入硬件scalar offset，行/列偏移保留在voffset。必要startup/每tile barrier、VM完成等待与末尾drain未删除。
- 现有CK接口最后一项是immediate offset，不是scalar offset；新私有`async_a_soffset.hpp`显式发出该硬件指令，并声明m0 clobber。
- scalar offset不参与voffset的硬件边界检查，所以只用于已调度合法K tile、完整N feature tile的权重搬运。h3_gemm验证N%256==0/K%64==0；禁止与终末unused A预取、cache A、A base rebase等未验证路径组合。不要复用到M-tail激活/输出搬运。

## 正确性与真实实现证据

- 独立搬运probe：8种K、首/中/末tile、3seed，63组/126次任意int16位模式复制，mapping/guard/输入不变全部通过。使用int16仅为了逐位验证BF16大小的数据搬运，不是INT8量化。
- ppair1/asoff/ppair1_asoff各102组/306次GEMM审计通过，覆盖奇数K tile数1/3/5/7/11/13和实际四shape。
- 本选择库额外126组/378次，增加四组实际N/K的小M尾部，全样本有限/guard/输入不变/重复确定性通过，且与原库输出逐位一致；普通随机仍另验FP32参考误差。不能把这表述为H3全网络/E2E画质验证。
- 实际ISA确认A load的scalar offset为s寄存器，不再总是0；两个内核各VGPR225、SGPR31、LDS65536、scratch/spill0。完整元数据在JSON。

## 两轮完整GEMM计时

M18880，5次预热，6轮轮换顺序×每轮10次HIP event，取轮均值中位数，profiler关闭。以下收益都相对本轮22ef控制；每轮内核缓存为热缓存。小收益仍应注意不同形状、缓存状态和实际H3调度下的变化。

### stage_cj_two_projection_producer_dispatch

| shape | 原库 ms | 选择库 ms | 耗时降低 | 选择库 T | 同轮 F.linear T | 配对6轮更快次数 |
|---|---:|---:|---:|---:|---:|---:|
| fc2 | 4.866149 | 4.842388 | +0.488% | 300.490 | 328.358 | 6/6 |
| qkv | 7.090627 | 7.090403 | +0.003% | 307.829 | 328.420 | 4/6 |
| out_proj | 2.546390 | 2.536286 | +0.397% | 286.854 | 331.347 | 6/6 |
| fc1 | 9.492042 | 9.490873 | +0.012% | 306.629 | 326.997 | 3/6 |

### stage_ck_shape_dispatch_independent

| shape | 原库 ms | 选择库 ms | 耗时降低 | 选择库 T | 同轮 F.linear T | 配对6轮更快次数 |
|---|---:|---:|---:|---:|---:|---:|
| fc2 | 4.870780 | 4.847484 | +0.478% | 300.174 | 327.225 | 6/6 |
| qkv | 7.091219 | 7.087307 | +0.055% | 307.964 | 327.781 | 5/6 |
| out_proj | 2.547854 | 2.538166 | +0.380% | 286.642 | 332.573 | 6/6 |
| fc1 | 9.491817 | 9.491657 | +0.002% | 306.604 | 328.742 | 3/6 |

## 状态和限制

保留为经过独立复测/广审计的可选实验库，原有效22ef库及默认rolephase配置不替换。FC2/out_proj收益约0.4%–0.5%，另外两投影基本持平；不按跨轮最低值、单次最快值或未插桩counter计算收益。没有证明整网E2E提速，也没有达到约330T。

候选：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768`。SHA `d2e34b972e3961fb99b2ea05802fc458561788dbb9587dc74a52fb96b3a5361f`；源码和构建命令/哈希在build/attempts冻结目录及build候选同名JSON。当前shape_dispatch.cpp是隔离实验包装，不是正式CK pipeline/policy重构；代码整理仍按用户要求推迟。未commit/push。

