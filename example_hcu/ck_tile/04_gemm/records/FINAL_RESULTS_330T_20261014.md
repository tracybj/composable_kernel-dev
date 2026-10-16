# CK Tile BF16 GEMM：约330T目标验收（2026-10-14）

## 结论

四个H3代表本地GEMM shape已达到约330TFLOPS，并在两个独立轮次中均接近同轮rocBLAS。统一入口自身通过正常/2-byte输出对齐、有限与特殊BF16数据、fallback/API及逐位审计。原库保留；未接H3，不是INT8或E2E成绩。

物理HCU4 / ck_yyc / gfx936 / 80CU / DTK25.10.0-0 / Torch2.9 / HIP6.3.26113 / CK `5c8ea51ed7ac03bdcc24aeb811d51acc57233b60`。输入X[M,K]、W[N,K]连续BF16，输出X@W.T为BF16、FP32累加；每次MAC按2 FLOPs计算。M=18880。

## 两轮最终统一入口计时

5次预热、6轮轮换×10 HIP events，取每轮均值的中位数，profiler关闭、重复同一buffer的hot-cache口径；对照相同输入、设备和运行环境。不是冷缓存、并发负载或完整H3时间。

| round | shape (N,K) | CK ms | CK TFLOPS | rocBLAS linear TFLOPS | rocBLAS mm_out TFLOPS | time reduction vs old BASE | vs previous optional | CK time vs fastest rocBLAS |
|---|---|---:|---:|---:|---:|---:|---:|---:|
| DF | fc2 (5376,7168) | 4.424885 | 328.842 | 328.821 | 328.553 | +9.041% | +8.610% | -0.007% |
| DF | qkv (10752,5376) | 6.546403 | 333.409 | 327.077 | 328.487 | +7.688% | +7.669% | -1.476% |
| DF | out_proj (5376,3584) | 2.207438 | 329.587 | 331.985 | 332.640 | +13.254% | +11.386% | +0.926% |
| DF | fc1 (14336,5376) | 8.736617 | 333.101 | 327.377 | 328.275 | +8.051% | +8.072% | -1.449% |
| DG | fc2 (5376,7168) | 4.420853 | 329.142 | 328.756 | 329.124 | +9.113% | +8.717% | -0.005% |
| DG | qkv (10752,5376) | 6.550875 | 333.182 | 328.280 | 329.095 | +7.562% | +7.544% | -1.227% |
| DG | out_proj (5376,3584) | 2.206406 | 329.742 | 332.359 | 332.703 | +13.304% | +11.429% | +0.898% |
| DG | fc1 (14336,5376) | 8.775322 | 331.632 | 327.607 | 327.532 | +7.565% | +7.551% | -1.214% |

## 新实现具体做了什么

1. 保留已有256×256 tile、K64权重LDS双槽、8 compute+4 producer waves、K32激活寄存器双bank、静态wave分组、双tile循环和CK BF16 RNE。没有换成外部GEMM。
2. 对内部激活操作数B启用结构化buffer：descriptor第62位CacheSwizzle=1；stride为不大于K*2的最大2次幂，row remainder留在voffset；K32公共偏移进入descriptor低48位pointer，保留高stride/cache位。这里内部B是公开接口的X，不是W。
3. logical row count保护M尾部；unused terminal phase置record count0，并在消费最后current bank前VM0；所有startup、current/future依赖、LDS/barrier、terminal pending register live/drain保留。
4. FC2/out_proj选择producer fixed pair+A scalar offset组合，QKV/FC1用普通CacheSwizzle；仅精确四组N/K分派，其他原支持尺寸走原raw基线，避免缩小原接口支持域。

| projection | K*2 byte stride | descriptor pow2 stride | row remainder | selected compute |
|---|---:|---:|---:|---|
| FC2 | 14336 | 8192 | 6144 | cache + producer pair/A soffset |
| QKV/FC1 | 10752 | 8192 | 2560 | cache |
| out_proj | 7168 | 4096 | 3072 | cache + producer pair/A soffset |

## 代码与构建

- 主入口：`struct32_dispatch.cpp`；共同kernel/流水：`rolephase.cpp`；私有结构化加载：`structured_b32.hpp`。A scalar offset实现：`async_a_soffset.hpp`。
- 构建目标：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32dispatch_reg768`，导出`h3_gemm(a,w,c,m,n,k,stream)`；共享include和03_gemm未修改。
- 动态库：`build/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32dispatch_reg768.so`，SHA256 `cbd4638fde724142b41314e7d2578e79fbbdc0a6bcd43d6e154716e768f5f6b9`。完整构建命令、compiler版本、源码哈希、冻结源码路径见同名JSON。
- 单独结构化K32原型要求K<8192以容纳14-bit stride；统一入口仅对已验证四组N/K使用它，其他尺寸保留原支持域回退。

| compiled entry | VGPR | SGPR | LDS | scratch | spill |
|---|---:|---:|---:|---:|---:|
| _ZN11h3_original18h3_original_kernelEPKtS1_Ptiii | 225 | 31 | 65536 | 0 | 0 |
| _ZN11h3_struct3218h3_struct32_kernelEPKtS1_Ptiii | 233 | 38 | 65536 | 0 | 0 |
| _ZN20h3_struct32_producer27h3_struct32_producer_kernelEPKtS1_Ptiii | 233 | 38 | 65536 | 0 | 0 |

## 正确性与验收覆盖

- DD四个直接候选各126组/378次、DE两个获益候选各126组/378次C偏移2-byte、DF统一入口正常/偏移各126组/378次：共1008组/3024次candidate launch通过。FP32参考和旧库额外launch不计入。
- 三seed、精确binary fraction/普通normal分布、odd K tile、四真实N/K小M尾部、输入不变、输出guard、finite、三次确定性及与原版逐位一致；普通分布另核对FP32参考误差。
- 原始位模式搬运1620组/3240次，另540次实际GPU descriptor回读，确认pointer/stride/bit62/bit63/row bounds；CPU地址模型138240项。
- 统一入口72种特殊BF16 case、144次候选调用：0/负0、Inf、正负NaN payload、subnormal、溢出大数，输出按int16位比较原版；均含2-byte输出与guard/输入保护。这只覆盖所测模式，不声明全部非有限输入有统一跨库NaN语义。
- K8128/8192/16384的三个非目标fallback case逐位正确；六类非法参数和原版均返回hipErrorInvalidValue。
- DF/DG四shape每轮均满足约330T区间且不慢于同轮最快rocBLAS超过3%；不能用平均吞吐、某一shape或最快单次替代。每类每轮至少5/6轮胜过上一个可选库。

## 如何理解CacheSwizzle的贡献

DD成对源码只有cache位配置不同，但编译后OFF/ON原型SGPR44/38并不相同，因此不要把完整收益全部等同为“只改一个硬件bit”的纯cache命中效果。最终成功来自结构化地址与当前K32流水的组合；旧K64方案只能使out_proj约292T，说明流水搭配同样关键。
四shape同轮xprof/DTK每实现3样本，最终库SHA与DG一致。完整counter表见`results/profile_struct32_four_20261014/ANALYSIS.md`；等待为wave聚合，不能当毫秒占比，SQ部分指标emulated，profile时长不用作排名。

## 重建和复测

在ck_yyc中使用原DTK环境（不切换编译器/驱动）。原始命令和日志在run_stage_dd/de/df/dg.sh；脚本拒绝覆盖已有tag。下面用新tag复测已编译的最终统一入口：

```bash
cd /workspace/composable_kernel/example_hcu/ck_tile/04_gemm
export HIP_VISIBLE_DEVICES=4 OMP_NUM_THREADS=4
python3 build.py rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32dispatch_reg768
python3 bench.py --tag verify_struct32_$(date +%Y%m%d_%H%M%S) --m 18880 \
  --candidates rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768,rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32dispatch_reg768
```

源码包`results/struct32_330t_sources_20261014.tar.gz`包含重建所需私有源码/头文件、build/bench/audit脚本、报告；仍依赖相同CK revision和容器工具链，不包含外部rocBLAS/驱动二进制。

## 完成范围和限制

完成的是本任务的四个代表BF16单GEMM接近rocBLAS约330T目标。没有量化、没有启动H3、没有宣称E2E提升；没有修改03/include、全局环境、驱动或时钟。无commit/push，无目录清理重排；原22ef、d2e、de1c库及全部负结果保留。

没有凭空引用理论硬件峰值/MFU，也不外推任意M/N/K、冷缓存、其他卡或并发场景；模型接入与真实E2E属于后续单独工作。

