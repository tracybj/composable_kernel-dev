# GEMM 实测库与源码位置索引（2026-10-14）

此表按112阶段首次出现的**库SHA256**编号。同SHA的不同名称合并为一个V编号，不把别名算独立优化。共293个实测阶段引用的不同哈希（含未计时/不支持候选）。

服务器根目录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm`；容器根目录：`/workspace/composable_kernel/example_hcu/ck_tile/04_gemm`。

当前源码行号用于阅读最新开关实现；历史精确重建应使用下面与实测库SHA一致的冻结构建记录。相同二进制可能由多次兼容重建产生，索引选取一个已校验的留存构建，不声称它是唯一一次历史构建。完整命令/源文件SHA及其余构建记录在 `../HISTORY_SOURCE_CATALOG_20261014.json`。

最早26个库没有独立源码快照，明确标注；一个旧 `rolephase_m1_bank_asmb_mid_reg768` 历史哈希未在当前build/*.so找到匹配库，快照和构建记录仍在。不能据此保证293个旧二进制均可直接启动。

<a id="v001"></a>
## V001 — gemm.cpp

候选名：`base`

实测库 SHA256：`3bcfb1ebe0b3c6ac3875476c7d436a8f8bfbe640d5674565d609bc4816201333`；首次出现：`stage_a_layout`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/base.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=128 -DH3_BK=32 -DH3_WM=4 -DH3_WN=1 -DH3_SWIZZLE=0
```

<a id="v002"></a>
## V002 — gemm.cpp

候选名：`swizzle`

实测库 SHA256：`041bd904c5b12fed60d5beb7f992237e29c0d7ae87bd6d00d19860f788d8b841`；首次出现：`stage_a_layout`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/swizzle.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=128 -DH3_BK=32 -DH3_WM=4 -DH3_WN=1 -DH3_SWIZZLE=1
```

<a id="v003"></a>
## V003 — gemm.cpp

候选名：`balanced`

实测库 SHA256：`6d6c6e7e7af90b2eb3aaa0b1dd0392a648a2d3ac5c275cf1674aa007038d3d59`；首次出现：`stage_b_tiles`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/balanced.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=128 -DH3_BK=32 -DH3_WM=2 -DH3_WN=2 -DH3_SWIZZLE=0
```

<a id="v004"></a>
## V004 — gemm.cpp

候选名：`wide`

实测库 SHA256：`51b3e3617afb7d0dc1a0a593296d55c3e153e32de055d23c7e4c4d42004523ad`；首次出现：`stage_b_tiles`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/wide.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0
```

<a id="v005"></a>
## V005 — gemm.cpp

候选名：`tall`

实测库 SHA256：`ecb7d718041a3fe053204db00ddf72a924f1161797cac7875cb776a7da8c9d14`；首次出现：`stage_b_tiles`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/tall.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=128 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0
```

<a id="v006"></a>
## V006 — gemm.cpp

候选名：`large`

实测库 SHA256：`4cabbd153734d426bb31d94e3ea0a7cc7c923a9536a75bf7877389a827e0f259`；首次出现：`stage_b_tiles`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/large.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0
```

<a id="v007"></a>
## V007 — gemm.cpp

候选名：`k64`

实测库 SHA256：`67f6e73436a2a8647fe18c1a3916e3f0fc420e74c643800d4d2b32d9157126e1`；首次出现：`stage_b_tiles`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/k64.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=128 -DH3_BK=64 -DH3_WM=4 -DH3_WN=1 -DH3_SWIZZLE=0
```

<a id="v008"></a>
## V008 — gemm.cpp

候选名：`wide_k64`

实测库 SHA256：`4c06a2e2719977a28e29388c242fa3eb3db97e4b7e37b6f6d47596ccc5bf137b`；首次出现：`stage_b_tiles`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/wide_k64.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=64 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0
```

<a id="v009"></a>
## V009 — gemm.cpp

候选名：`large_k64`

实测库 SHA256：`f11b19da768c0b656c008a4bbbf3a8b76259c9dc0af5fe857b67b3c7e162cd3c`；首次出现：`stage_b_tiles`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/large_k64.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0
```

<a id="v010"></a>
## V010 — gemm.cpp

候选名：`base_raw_barrier`

实测库 SHA256：`fd3b5162a177a68bf7873d6f7220cabf625d64ca59b31b8ce032cd8d1317b2ea`；首次出现：`stage_c_raw_barrier`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/base_raw_barrier.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=128 -DH3_BK=32 -DH3_WM=4 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1
```

<a id="v011"></a>
## V011 — gemm.cpp

候选名：`wide_raw_barrier`

实测库 SHA256：`716cd310a96703802f0a8b19ea3e9048f7e255d63621d11b5c4d11ef661e6063`；首次出现：`stage_c_raw_barrier`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/wide_raw_barrier.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1
```

<a id="v012"></a>
## V012 — gemm.cpp

候选名：`wide4_raw_barrier`

实测库 SHA256：`d53ee963ab0b6b7b084d142988d81c44f5fe136012a806f9f13fcb680027e7e5`；首次出现：`stage_d_double_buffer`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/wide4_raw_barrier.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1
```

<a id="v013"></a>
## V013 — gemm.cpp

候选名：`tall4_raw_barrier`

实测库 SHA256：`ed6eabad7fe9a065fa8f6ba8556f5169c97ca12601c47e5cf4e7e4b936845a8a`；首次出现：`stage_d_double_buffer`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/tall4_raw_barrier.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=128 -DH3_BK=32 -DH3_WM=4 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1
```

<a id="v014"></a>
## V014 — gemm.cpp

候选名：`base_double`

实测库 SHA256：`515c62adfcd476a480f08234980d5f354cc69194a43b5d47e9271792ca74fd92`；首次出现：`stage_d_double_buffer`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/base_double.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=128 -DH3_BK=32 -DH3_WM=4 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1
```

<a id="v015"></a>
## V015 — gemm.cpp

候选名：`wide_double`

实测库 SHA256：`1937667b3533333aee394778018f502d0328c9713ef8b65baba5cc6f091f4414`；首次出现：`stage_d_double_buffer`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/wide_double.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1
```

<a id="v016"></a>
## V016 — gemm.cpp

候选名：`wide4_double`

实测库 SHA256：`e6e564b226bf80bf0a7c9a32b1920c1e2c36a623faa74ad857fbf6cecd73644c`；首次出现：`stage_d_double_buffer`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/wide4_double.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1
```

<a id="v017"></a>
## V017 — gemm.cpp

候选名：`base_pad8`

实测库 SHA256：`e7f8432487818508900973ca53337349a457ec18f640dab43f345ca2a4f7f469`；首次出现：`stage_e_lds_padding`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/base_pad8.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=128 -DH3_BK=32 -DH3_WM=4 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=8
```

<a id="v018"></a>
## V018 — gemm.cpp

候选名：`wide_pad8`

实测库 SHA256：`c214e24e1025969f5607839cbd44272fa09edae7371c8fc1581d37b9fef2c7f3`；首次出现：`stage_e_lds_padding`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/wide_pad8.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=8
```

<a id="v019"></a>
## V019 — gemm.cpp

候选名：`wide_pad16`；`wide_pad16_relaxed`

实测库 SHA256：`6514e5cd018fac5cb1f069538a94f424e6a12454e62f545f4dba8eba98b5455a`；首次出现：`stage_e_lds_padding`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_pad16_relaxed/20261012-172349-208799/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_pad16_relaxed/20261012-172349-208799/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_RELAX_BOUNDS=1
```

<a id="v020"></a>
## V020 — gemm.cpp

候选名：`wide4_pad8`

实测库 SHA256：`cee6a4d84bcae089ad4752bb223a90f78145eb4cf0648ccd44178597d5e0c583`；首次出现：`stage_e_lds_padding`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/wide4_pad8.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=8
```

<a id="v021"></a>
## V021 — gemm.cpp

候选名：`wide4_k16`

实测库 SHA256：`05055bad1300c7a901946884d372801242841eb3a83da6d25759e3aa88d3bd8c`；首次出现：`stage_g_warp_k16`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/wide4_k16.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=16 -DH3_WM=4 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_WK=16
```

<a id="v022"></a>
## V022 — gemm.cpp

候选名：`wide4_wk16`

实测库 SHA256：`10ef1d10f2eea6efbfe960d5bfdb78447b3b4c728ea2b940ddd9f3850f6af6a7`；首次出现：`stage_g_warp_k16`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/wide4_wk16.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_WK=16
```

<a id="v023"></a>
## V023 — gemm.cpp

候选名：`wide4_wk16_pad16`

实测库 SHA256：`898a0557b4fff9376e2dc595d5e6471859f2ccf4119eca99d52146d9bbb01939`；首次出现：`stage_g_warp_k16`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/wide4_wk16_pad16.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_WK=16
```

<a id="v024"></a>
## V024 — gemm.cpp

候选名：`wide_wk16_pad16`

实测库 SHA256：`2e85851234cc27d45b2f9c5c60b3d54b699ced085454bd7547f0ae06fea9c0f3`；首次出现：`stage_g_warp_k16`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/wide_wk16_pad16.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_WK=16
```

<a id="v025"></a>
## V025 — gemm.cpp

候选名：`base_dynamic_double`

实测库 SHA256：`a1f6179216f0273140461b404132eebe8d34d52221604a03d93d5f5946edee8f`；首次出现：`stage_f_dynamic_double`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/base_dynamic_double.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=128 -DH3_BK=32 -DH3_WM=4 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1 -DH3_DYNAMIC_DOUBLE=1
```

<a id="v026"></a>
## V026 — gemm.cpp

候选名：`wide_dynamic_double`

实测库 SHA256：`ac698c07542892186d48a13107b104db4bc40f0fba95bb58c4bb0f7e48292aeb`；首次出现：`stage_f_dynamic_double`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/wide_dynamic_double.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1 -DH3_DYNAMIC_DOUBLE=1
```

<a id="v027"></a>
## V027 — gemm.cpp

候选名：`wide4_dynamic_double`

实测库 SHA256：`845a9d72514dad7799501f551f1c2d870fc6fe2e75dc9aca560e2b9a6f72c8f0`；首次出现：`stage_f_dynamic_double`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/wide4_dynamic_double.json`。

**没有独立冻结源码快照**；构建命令和库哈希保留，当前入口不能当作当时源码的逐字替身。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1 -DH3_DYNAMIC_DOUBLE=1
```

<a id="v028"></a>
## V028 — gemm.cpp

候选名：`wide_pad16_g1`

实测库 SHA256：`d73d02f0b40405d69094fbd22e906289b2b6f3ebc4c8218bf4deabdb517a1f92`；首次出现：`stage_h_tile_order`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_pad16_g1/20261012-170749-053917/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_pad16_g1/20261012-170749-053917/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_GROUP_M=1
```

<a id="v029"></a>
## V029 — gemm.cpp

候选名：`wide_pad16_g2`

实测库 SHA256：`f153efd4fd6055748b5365ea69b2221a3a1fa98feead77e00bbd50f52fbc2f1e`；首次出现：`stage_h_tile_order`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_pad16_g2/20261012-170749-054565/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_pad16_g2/20261012-170749-054565/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_GROUP_M=2
```

<a id="v030"></a>
## V030 — gemm.cpp

候选名：`wide_pad16_g4`

实测库 SHA256：`adf20de24a48ebf0dd6738470f26417d67b39a8858035fc426b8aac3e4e1b950`；首次出现：`stage_h_tile_order`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_pad16_g4/20261012-170841-851157/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_pad16_g4/20261012-170841-851157/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_GROUP_M=4
```

<a id="v031"></a>
## V031 — gemm.cpp

候选名：`wide_pad16_g8`

实测库 SHA256：`c7f73530e169fa14365bd57606102a18d1f2aea22afee22e40e498494360ca7c`；首次出现：`stage_h_tile_order`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_pad16_g8/20261012-170842-277624/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_pad16_g8/20261012-170842-277624/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_GROUP_M=8
```

<a id="v032"></a>
## V032 — gemm.cpp

候选名：`wide_pad16_g16`

实测库 SHA256：`044155af66ebf38c4fccb19e63cf0d9f3164b22d12467ab2e266ffb1ab68cd1f`；首次出现：`stage_h_tile_order`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_pad16_g16/20261012-170935-155182/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_pad16_g16/20261012-170935-155182/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_GROUP_M=16
```

<a id="v033"></a>
## V033 — gemm.cpp

候选名：`base_xor`

实测库 SHA256：`ea3871ba4fa78a706ab863feefd29863804f94e582a3aced816b2cce10fe275a`；首次出现：`stage_i_pack_xor`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/base_xor/20261012-171714-879215/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/base_xor/20261012-171714-879215/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=128 -DH3_BK=32 -DH3_WM=4 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_XOR=1
```

<a id="v034"></a>
## V034 — gemm.cpp

候选名：`wide_xor`

实测库 SHA256：`e91533bc37c05175d689e0e8c97704c3f2f0bc7f8084117c9e748faf88bda82d`；首次出现：`stage_i_pack_xor`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_xor/20261012-171714-879787/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_xor/20261012-171714-879787/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_XOR=1
```

<a id="v035"></a>
## V035 — gemm.cpp

候选名：`wide4_xor`

实测库 SHA256：`035edfacd2a169b74937d31ce25b72042e44ab6ec69350a725a6bad3192ac5b9`；首次出现：`stage_i_pack_xor`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide4_xor/20261012-171808-012536/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide4_xor/20261012-171808-012536/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_XOR=1
```

<a id="v036"></a>
## V036 — gemm.cpp

候选名：`wide_xor_g8`

实测库 SHA256：`9f94a9ce0dec086f03904340cc74264865d0ee546bb73728568f06f1dee76f6b`；首次出现：`stage_i_pack_xor`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_xor_g8/20261012-171809-938267/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_xor_g8/20261012-171809-938267/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_GROUP_M=8 -DH3_RAW_BARRIER=1 -DH3_LDS_XOR=1
```

<a id="v037"></a>
## V037 — gemm.cpp

候选名：`large_relaxed`

实测库 SHA256：`62e7e4361c63ab677e0ea3b2a8cf82c68db3ca9cc43e492780c0c4ee6917a6d4`；首次出现：`stage_j_launch_bounds`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_relaxed/20261012-172233-483560/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_relaxed/20261012-172233-483560/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_RELAX_BOUNDS=1
```

<a id="v038"></a>
## V038 — gemm.cpp

候选名：`large_pad16_relaxed`

实测库 SHA256：`8caac7c98cc15ef59e65d1e04a053a7a173d241ad61bcaa1d9199b3f5d8f8da3`；首次出现：`stage_j_launch_bounds`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_relaxed/20261012-172233-483968/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_relaxed/20261012-172233-483968/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_RELAX_BOUNDS=1
```

<a id="v039"></a>
## V039 — gemm.cpp

候选名：`large_pad16_g8_relaxed`

实测库 SHA256：`a7eacb59dd09083243f229625fdbab9e136c92d6bfa96294fa879bf9aa723f1d`；首次出现：`stage_j_launch_bounds`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_relaxed/20261012-172348-528276/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_relaxed/20261012-172348-528276/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_GROUP_M=8 -DH3_RELAX_BOUNDS=1
```

<a id="v040"></a>
## V040 — gemm.cpp

候选名：`base_ni4`

实测库 SHA256：`5c970e7a4e7f271d9007ee68a78099f62585c907a52258efeb9d422cea55012e`；首次出现：`stage_k_n_interleave`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/base_ni4/20261012-172712-263636/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/base_ni4/20261012-172712-263636/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=128 -DH3_BK=32 -DH3_WM=4 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_NI4=1
```

<a id="v041"></a>
## V041 — gemm.cpp

候选名：`wide_ni4`

实测库 SHA256：`40fb3793aea8be507dab57df4efa215cdf22869660ad207a0cfd5925c0419c71`；首次出现：`stage_k_n_interleave`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_ni4/20261012-172712-263975/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_ni4/20261012-172712-263975/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_NI4=1
```

<a id="v042"></a>
## V042 — gemm.cpp

候选名：`wide_pad16_ni4_g8`

实测库 SHA256：`2bbb5a9939a13895a8e420522576e501ecf5af0c1d1ccbe4dbb62c4822f33129`；首次出现：`stage_k_n_interleave`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_pad16_ni4_g8/20261012-172753-679568/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_pad16_ni4_g8/20261012-172753-679568/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_GROUP_M=8 -DH3_NI4=1
```

<a id="v043"></a>
## V043 — gemm.cpp

候选名：`wide4_ni4`

实测库 SHA256：`15787acdc094ce1ab7ebe727643b7c243f30eec05ec1202e7a63d9a7fcfbe0d1`；首次出现：`stage_k_n_interleave`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide4_ni4/20261012-172754-541360/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide4_ni4/20261012-172754-541360/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_NI4=1
```

<a id="v044"></a>
## V044 — gemm.cpp

候选名：`wide_mr4`

实测库 SHA256：`59f09aafed1a5512b570c0508709a1d1a926c604f8e4911a4c24b3c3fe5598e2`；首次出现：`stage_l_balanced_operands`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_mr4/20261012-173214-563133/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_mr4/20261012-173214-563133/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=2 -DH3_WN=4 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_MR4=1 -DH3_WARP_M=64
```

<a id="v045"></a>
## V045 — gemm.cpp

候选名：`wide_mr4_pad16_g8`；`wide_mr4_pad16_g8_reg768`

实测库 SHA256：`18f5b1902387d8fb8fca0d94251eeca42bee5a61ecce62ab46c1957f749e9b32`；首次出现：`stage_l_balanced_operands`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_mr4_pad16_g8/20261012-173214-563625/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_mr4_pad16_g8/20261012-173214-563625/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=2 -DH3_WN=4 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_GROUP_M=8 -DH3_MR4=1 -DH3_WARP_M=64
```

<a id="v046"></a>
## V046 — gemm.cpp

候选名：`wide_mr4_wk16_pad16_g8`

实测库 SHA256：`fe04dd7afa98ca1d0da61f737855408678bd0c476fdd0262aefd1538db81c69f`；首次出现：`stage_l_balanced_operands`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_mr4_wk16_pad16_g8/20261012-173311-829254/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_mr4_wk16_pad16_g8/20261012-173311-829254/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=2 -DH3_WN=4 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_WK=16 -DH3_GROUP_M=8 -DH3_MR4=1 -DH3_WARP_M=64
```

<a id="v047"></a>
## V047 — gemm.cpp

候选名：`wide_mr4_pad8_g8`

实测库 SHA256：`b71df84372ca9ffd32ab585a15f1384f93217009a22c546ca08c9706e1385dc5`；首次出现：`stage_l_balanced_operands`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_mr4_pad8_g8/20261012-173311-949035/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_mr4_pad8_g8/20261012-173311-949035/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=2 -DH3_WN=4 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=8 -DH3_GROUP_M=8 -DH3_MR4=1 -DH3_WARP_M=64
```

<a id="v048"></a>
## V048 — gemm.cpp

候选名：`wide_kpack4_g8`

实测库 SHA256：`c816182e49692ffc92fa310f6efd50f0593ac4d1cb16ea2e98a14e62abfb70bf`；首次出现：`stage_m_kpack_major`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_kpack4_g8/20261012-174546-066154/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_kpack4_g8/20261012-174546-066154/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_GROUP_M=8 -DH3_RAW_BARRIER=1 -DH3_LDS_KPACK=4
```

<a id="v049"></a>
## V049 — gemm.cpp

候选名：`wide_kpack8_g8`

实测库 SHA256：`e3f87b2046cfd52a0d9da2a771f7d4e38cebe0b669010d28b91b71c8a9eb1702`；首次出现：`stage_m_kpack_major`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_kpack8_g8/20261012-174546-066532/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide_kpack8_g8/20261012-174546-066532/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_GROUP_M=8 -DH3_RAW_BARRIER=1 -DH3_LDS_KPACK=8
```

<a id="v050"></a>
## V050 — gemm.cpp

候选名：`wide4_kpack4_g8`

实测库 SHA256：`d1cf9f767d59cfdea471727e88ddad441101b03edcabac26ead6905430bc9550`；首次出现：`stage_m_kpack_major`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide4_kpack4_g8/20261012-174640-406242/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide4_kpack4_g8/20261012-174640-406242/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_GROUP_M=8 -DH3_RAW_BARRIER=1 -DH3_LDS_KPACK=4
```

<a id="v051"></a>
## V051 — gemm.cpp

候选名：`wide4_kpack8_g8`

实测库 SHA256：`3bdb5488094f0637b8171208bc7fa24527f444e101927fc3292939923d71ea6a`；首次出现：`stage_m_kpack_major`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide4_kpack8_g8/20261012-174641-386460/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide4_kpack8_g8/20261012-174641-386460/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_GROUP_M=8 -DH3_RAW_BARRIER=1 -DH3_LDS_KPACK=8
```

<a id="v052"></a>
## V052 — specialized.cpp

候选名：`specialized`

实测库 SHA256：`67cba21614da09e3fba7533b4a20db0185dc894f3765f93bdb7897e0d86c68fa`；首次出现：`stage_n_wave_specialized`。

当前入口：[specialized.cpp](<../specialized.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/specialized/20261012-182420-086419/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/specialized/20261012-182420-086419/specialized.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0
```

<a id="v053"></a>
## V053 — specialized.cpp

候选名：`specialized_reg768`

实测库 SHA256：`f836d481a45719647adb386fb62b87428f2cbba589003fd25efc0e47cfe69fd0`；首次出现：`stage_o_register_pool`。

当前入口：[specialized.cpp](<../specialized.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/specialized_reg768/20261012-182912-573424/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/specialized_reg768/20261012-182912-573424/specialized.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v054"></a>
## V054 — gemm.cpp

候选名：`large_pad16_g8_reg768`

实测库 SHA256：`9bbf035b9ccbe0760610ed5a919e50421c3acfec30d90550335cadcbc167d695`；首次出现：`stage_o_register_pool`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_reg768/20261012-182917-554733/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_reg768/20261012-182917-554733/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_GROUP_M=8
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v055"></a>
## V055 — gemm.cpp

候选名：`large_mr4_pad16_g8_reg768`

实测库 SHA256：`1d41e02cad33179462ac1403c774ee3b23d2d7aadb3184fab606dd90514b0fc8`；首次出现：`stage_p_large_pipeline`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_mr4_pad16_g8_reg768/20261012-183344-585160/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_mr4_pad16_g8_reg768/20261012-183344-585160/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=2 -DH3_WN=4 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_GROUP_M=8 -DH3_MR4=1 -DH3_WARP_M=64
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v056"></a>
## V056 — gemm.cpp

候选名：`large_mr4_xor_g8_reg768`

实测库 SHA256：`f55f3ee825a2f28e889655ceb77540ed02c8310d6ec6641ce596999cbb57ea23`；首次出现：`stage_p_large_pipeline`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_mr4_xor_g8_reg768/20261012-183344-585416/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_mr4_xor_g8_reg768/20261012-183344-585416/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=2 -DH3_WN=4 -DH3_SWIZZLE=0 -DH3_GROUP_M=8 -DH3_RAW_BARRIER=1 -DH3_LDS_XOR=1 -DH3_MR4=1 -DH3_WARP_M=64
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v057"></a>
## V057 — gemm.cpp

候选名：`large_kpack8_g8_reg768`

实测库 SHA256：`07f61845bfd9d4c18303aea0b549e0db7b4fef3f7bd7b07f30cd18ed8909c7f3`；首次出现：`stage_p_large_pipeline`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_kpack8_g8_reg768/20261012-183504-396185/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_kpack8_g8_reg768/20261012-183504-396185/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_GROUP_M=8 -DH3_RAW_BARRIER=1 -DH3_LDS_KPACK=8
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v058"></a>
## V058 — gemm.cpp

候选名：`large_raw_barrier_g8_reg768`

实测库 SHA256：`41777ccf404694d42d2033eb0d97b5244c0929833519a7d0f5873a47a5510e75`；首次出现：`stage_p_large_pipeline`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_raw_barrier_g8_reg768/20261012-183505-457452/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_raw_barrier_g8_reg768/20261012-183505-457452/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_GROUP_M=8
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v059"></a>
## V059 — gemm.cpp

候选名：`large_double_g8_reg768`

实测库 SHA256：`f71d3db1a6878103da0df35778283e32d9d071db79d809c5d52ea020b4ff470a`；首次出现：`stage_p_large_pipeline`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_double_g8_reg768/20261012-183621-093493/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_double_g8_reg768/20261012-183621-093493/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1 -DH3_GROUP_M=8
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v060"></a>
## V060 — specialized.cpp

候选名：`specialized_pad16_reg768`

实测库 SHA256：`17529a0028e4a4a4b4efae0a4008e8b68fb2b10360f1393ca1de4ed8043074df`；首次出现：`stage_q_specialized_layout`。

当前入口：[specialized.cpp](<../specialized.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/specialized_pad16_reg768/20261012-183846-459546/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/specialized_pad16_reg768/20261012-183846-459546/specialized.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_SPEC_PAD=16
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v061"></a>
## V061 — specialized.cpp

候选名：`specialized_kpack8_reg768`

实测库 SHA256：`51f07145f3e70b9e197c00a792da4e2691441946d256bacf2a6a2e2b01bb898e`；首次出现：`stage_q_specialized_layout`。

当前入口：[specialized.cpp](<../specialized.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/specialized_kpack8_reg768/20261012-183846-459869/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/specialized_kpack8_reg768/20261012-183846-459869/specialized.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_KPACK=8 -DH3_SPEC_KPACK=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v062"></a>
## V062 — specialized.cpp

候选名：`specialized_async_kpack8_reg768`

实测库 SHA256：`9fd3d7a3af0e2af85f251a85d26e82e6f7167471dd7fd444f10470fc4b18497d`；首次出现：`stage_q_specialized_layout`。

当前入口：[specialized.cpp](<../specialized.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/specialized_async_kpack8_reg768/20261012-183851-492755/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/specialized_async_kpack8_reg768/20261012-183851-492755/specialized.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_KPACK=8 -DH3_SPEC_KPACK=1 -DH3_SPEC_ASYNC=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v063"></a>
## V063 — specialized.cpp

候选名：`specialized_kpack8_prio_reg768`

实测库 SHA256：`c2c599e0c2da682a52d211d7d1ddf4ea0a42e264cabc86fa8659e3a10d9dfb1b`；首次出现：`stage_r_buffer_priority`。

当前入口：[specialized.cpp](<../specialized.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/specialized_kpack8_prio_reg768/20261012-184311-828824/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/specialized_kpack8_prio_reg768/20261012-184311-828824/specialized.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_KPACK=8 -DH3_SPEC_KPACK=1 -DH3_SPEC_PRIO=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v064"></a>
## V064 — specialized.cpp

候选名：`specialized_kpack8_buffer_reg768`

实测库 SHA256：`2476db3c9502005e05af85aee1944b9a7b27d6e99acd6c842289568c5be4e1b6`；首次出现：`stage_r_buffer_priority`。

当前入口：[specialized.cpp](<../specialized.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/specialized_kpack8_buffer_reg768/20261012-184311-829072/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/specialized_kpack8_buffer_reg768/20261012-184311-829072/specialized.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_KPACK=8 -DH3_SPEC_KPACK=1 -DH3_SPEC_BUFFER=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v065"></a>
## V065 — specialized.cpp

候选名：`specialized_kpack8_buffer_prio_reg768`

实测库 SHA256：`33a6b5ecdced73959b0c2d4d6210f5b582d4e8d6bdd05bfc24658ad4152d504c`；首次出现：`stage_r_buffer_priority`。

当前入口：[specialized.cpp](<../specialized.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/specialized_kpack8_buffer_prio_reg768/20261012-184316-983093/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/specialized_kpack8_buffer_prio_reg768/20261012-184316-983093/specialized.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_KPACK=8 -DH3_SPEC_KPACK=1 -DH3_SPEC_PRIO=1 -DH3_SPEC_BUFFER=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v066"></a>
## V066 — specialized.cpp

候选名：`specialized_async_kpack8_buffer_prio_reg768`

实测库 SHA256：`c1a00595a13f05136c310f9d9d1c136d9b581bd29a87f65683473ac92339fd6d`；首次出现：`stage_r_buffer_priority`。

当前入口：[specialized.cpp](<../specialized.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/specialized_async_kpack8_buffer_prio_reg768/20261012-184317-009550/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/specialized_async_kpack8_buffer_prio_reg768/20261012-184317-009550/specialized.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_KPACK=8 -DH3_SPEC_KPACK=1 -DH3_SPEC_ASYNC=1 -DH3_SPEC_PRIO=1 -DH3_SPEC_BUFFER=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v067"></a>
## V067 — gemm.cpp

候选名：`large_pad8_g8_reg768`

实测库 SHA256：`9ec0f12357c329c21f3aea9136724dd320a199d5c38e3ccff30a29486aa0b390`；首次出现：`stage_s_aspect_k64`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad8_g8_reg768/20261012-184445-241993/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad8_g8_reg768/20261012-184445-241993/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=8 -DH3_GROUP_M=8
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v068"></a>
## V068 — gemm.cpp

候选名：`large_mr4_k64_kpack8_g8_reg768`

实测库 SHA256：`8d48f009894d2f265dbcefd3820768c6af3c83422d1ffbe32d306a8d9eec3c93`；首次出现：`stage_s_aspect_k64`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_mr4_k64_kpack8_g8_reg768/20261012-184445-242385/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_mr4_k64_kpack8_g8_reg768/20261012-184445-242385/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=2 -DH3_WN=4 -DH3_SWIZZLE=0 -DH3_GROUP_M=8 -DH3_RAW_BARRIER=1 -DH3_MR4=1 -DH3_WARP_M=64 -DH3_LDS_KPACK=8
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v069"></a>
## V069 — gemm.cpp

候选名：`tall512_mr4_pad16_g8_reg768`

实测库 SHA256：`6bb7ae29a22d7246ff08dbf2814aeb023c05e92a939731ee81465729bc014d23`；首次出现：`stage_s_aspect_k64`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/tall512_mr4_pad16_g8_reg768/20261012-184601-726142/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/tall512_mr4_pad16_g8_reg768/20261012-184601-726142/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=512 -DH3_BN=128 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_GROUP_M=8 -DH3_MR4=1 -DH3_WARP_M=64
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v070"></a>
## V070 — gemm.cpp

候选名：`wide512_mr4_pad16_g8_reg768`

实测库 SHA256：`3eeb62a8cb4b6b10cd6f8efb772e5dbf7820154e6a4bb4dd6e87558b9524ff3e`；首次出现：`stage_s_aspect_k64`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide512_mr4_pad16_g8_reg768/20261012-184605-013937/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/wide512_mr4_pad16_g8_reg768/20261012-184605-013937/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=512 -DH3_BK=32 -DH3_WM=2 -DH3_WN=4 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_GROUP_M=8 -DH3_MR4=1 -DH3_WARP_M=64
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v071"></a>
## V071 — gemm.cpp

候选名：`large_pad16_g8_pad_m_reg768`

实测库 SHA256：`8f7d5fcbdf9cd4bda64afec0865ae960e6b6f0000564bd90f18dde9a18792ef4`；首次出现：`stage_u_12_math_waves`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_pad_m_reg768/20261012-190222-623924/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_pad_m_reg768/20261012-190222-623924/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_GROUP_M=8 -DH3_PAD_M=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v072"></a>
## V072 — gemm.cpp

候选名：`taller384_pad16_g8_pad_m_reg768`

实测库 SHA256：`627c5b8258e48d52a7212054a5255656f153b0b94839249e23c1eaf314feb4cb`；首次出现：`stage_u_12_math_waves`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/taller384_pad16_g8_pad_m_reg768/20261012-190222-623605/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/taller384_pad16_g8_pad_m_reg768/20261012-190222-623605/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=384 -DH3_BN=256 -DH3_BK=32 -DH3_WM=6 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_GROUP_M=8 -DH3_PAD_M=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v073"></a>
## V073 — role64.cpp

候选名：`role64_single_plain_reg768`

实测库 SHA256：`25c01e4622b4b2dcbfa53f6af890c904e6f0bd79a0de62221a8401676d7b89f8`；首次出现：`stage_v_k64_wrapped_pipeline`。

当前入口：[role64.cpp](<../role64.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_single_plain_reg768/20261012-191619-004657/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_single_plain_reg768/20261012-191619-004657/role64.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_ROLE_STAGES=1 -DH3_ROLE_WRAP=0
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v074"></a>
## V074 — role64.cpp

候选名：`role64_single_wrap_reg768`

实测库 SHA256：`bcf097a8a4959c7b84f84227ba6846343ff6c5ba9b2d30964673c1e6006cba21`；首次出现：`stage_v_k64_wrapped_pipeline`。

当前入口：[role64.cpp](<../role64.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_single_wrap_reg768/20261012-191619-005115/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_single_wrap_reg768/20261012-191619-005115/role64.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_ROLE_STAGES=1 -DH3_ROLE_WRAP=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v075"></a>
## V075 — role64.cpp

候选名：`role64_double_wrap_reg768`

实测库 SHA256：`3f7b081145e578ed98a0cb025d289d02916c794a757f2ef4b5348a24b0082251`；首次出现：`stage_v_k64_wrapped_pipeline`。

当前入口：[role64.cpp](<../role64.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_asm_reg768/20261012-192249-802686/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_asm_reg768/20261012-192249-802686/role64.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1 -DH3_PAD_M=1 -DH3_ROLE_STAGES=2 -DH3_ROLE_WRAP=1 -DH3_ROLE_ASM=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v076"></a>
## V076 — role64.cpp

候选名：`role64_double_wrap_zero_reg768`

实测库 SHA256：`079ddb03ad38046dd6528563733683cf7587b5e0c207b479ca9a2d4af415a771`；首次出现：`stage_w_mmac_order`。

当前入口：[role64.cpp](<../role64.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_zero_reg768/20261012-192249-803200/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_zero_reg768/20261012-192249-803200/role64.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1 -DH3_PAD_M=1 -DH3_ROLE_STAGES=2 -DH3_ROLE_WRAP=1 -DH3_ROLE_ZERO=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v077"></a>
## V077 — role64.cpp

候选名：`role64_coop_double_wrap_reg768`

实测库 SHA256：`5e65fd60f4fd252f6bcafbbdd81ddd9f481ff5b42de39c50f7b307d7c89f0e58`；首次出现：`stage_x_cooperative_prefetch`。

当前入口：[role64.cpp](<../role64.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_coop_double_wrap_reg768/20261012-193200-903312/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_coop_double_wrap_reg768/20261012-193200-903312/role64.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=8 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1 -DH3_PAD_M=1 -DH3_ROLE_STAGES=2 -DH3_ROLE_WRAP=1 -DH3_ROLE_COOP=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v078"></a>
## V078 — role64.cpp

候选名：`role64_coop_single_wrap_reg768`

实测库 SHA256：`a5435f71619da3b1647b2bd1a02d0f63ed3d2fca90ec53b172c8d445901b2b94`；首次出现：`stage_x_cooperative_prefetch`。

当前入口：[role64.cpp](<../role64.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_coop_single_wrap_reg768/20261012-193200-903966/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_coop_single_wrap_reg768/20261012-193200-903966/role64.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=8 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_ROLE_STAGES=1 -DH3_ROLE_WRAP=1 -DH3_ROLE_COOP=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v079"></a>
## V079 — role64.cpp

候选名：`role64_coop_double_plain_reg768`

实测库 SHA256：`d83d39bf0d08822ffbd7dae55d2c00b880b30ae4a33357fcddd9160448941da4`；首次出现：`stage_x_cooperative_prefetch`。

当前入口：[role64.cpp](<../role64.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_coop_double_plain_reg768/20261012-193206-026887/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_coop_double_plain_reg768/20261012-193206-026887/role64.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=8 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1 -DH3_PAD_M=1 -DH3_ROLE_STAGES=2 -DH3_ROLE_WRAP=0 -DH3_ROLE_COOP=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v080"></a>
## V080 — role64.cpp

候选名：`role64_double_wrap_cachea_reg768`

实测库 SHA256：`0b176e8e87408e348e6758728c327c35ed77b49f9a4eac80fc37fa4dfcff65de`；首次出现：`stage_y_structured_cache_swizzle`。

当前入口：[role64.cpp](<../role64.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_cachea_reg768/20261012-193805-564485/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_cachea_reg768/20261012-193805-564485/role64.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1 -DH3_PAD_M=1 -DH3_ROLE_STAGES=2 -DH3_ROLE_WRAP=1 -DH3_ROLE_CACHE_A=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v081"></a>
## V081 — role64.cpp

候选名：`role64_double_wrap_cacheb_reg768`

实测库 SHA256：`57fc7b4beecb81b632672d18c3b4cb55bbecf694e2ee7ded5661851e5771510b`；首次出现：`stage_y_structured_cache_swizzle`。

当前入口：[role64.cpp](<../role64.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_cacheb_reg768/20261012-193805-564937/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_cacheb_reg768/20261012-193805-564937/role64.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1 -DH3_PAD_M=1 -DH3_ROLE_STAGES=2 -DH3_ROLE_WRAP=1 -DH3_ROLE_CACHE_B=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v082"></a>
## V082 — role64.cpp

候选名：`role64_double_wrap_cacheab_reg768`

实测库 SHA256：`532c47b68c5dbbe01cfb2b2228c4629b465702f1460047330e4721c4d4c3706c`；首次出现：`stage_y_structured_cache_swizzle`。

当前入口：[role64.cpp](<../role64.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_cacheab_reg768/20261012-193810-688517/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_cacheab_reg768/20261012-193810-688517/role64.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1 -DH3_PAD_M=1 -DH3_ROLE_STAGES=2 -DH3_ROLE_WRAP=1 -DH3_ROLE_CACHE_A=1 -DH3_ROLE_CACHE_B=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v083"></a>
## V083 — role64.cpp

候选名：`role64_double_wrap_cacheb_latewait_reg768`

实测库 SHA256：`928917308163a7ee0c9c55eba90f0042d5a0ac8754723515a6f7ebb9ef1c65f3`；首次出现：`stage_z_structured_wait_tracking`。

当前入口：[role64.cpp](<../role64.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_cacheb_latewait_reg768/20261012-194221-281957/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_cacheb_latewait_reg768/20261012-194221-281957/role64.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1 -DH3_PAD_M=1 -DH3_ROLE_STAGES=2 -DH3_ROLE_WRAP=1 -DH3_ROLE_CACHE_B=1 -DH3_ROLE_LATEWAIT=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v084"></a>
## V084 — role64.cpp

候选名：`role64_double_wrap_cacheb_llvm_reg768`

实测库 SHA256：`cc276154fb419168f58ff3b58b08295c3c1fb686bc0cbad7abea0d36b4b00dbf`；首次出现：`stage_z_structured_wait_tracking`。

当前入口：[role64.cpp](<../role64.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_cacheb_llvm_reg768/20261012-194221-282465/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_cacheb_llvm_reg768/20261012-194221-282465/role64.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1 -DH3_PAD_M=1 -DH3_ROLE_STAGES=2 -DH3_ROLE_WRAP=1 -DH3_ROLE_CACHE_B=1 -DH3_ROLE_CACHE_LLVM=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v085"></a>
## V085 — role64.cpp

候选名：`role64_double_wrap_cacheab_llvm_reg768`

实测库 SHA256：`c1d281c51da630ec9bbc8c4c0b5123ade72a5aab2059fec02bef1756e64b5661`；首次出现：`stage_z_structured_wait_tracking`。

当前入口：[role64.cpp](<../role64.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_cacheab_llvm_reg768/20261012-194226-402151/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_cacheab_llvm_reg768/20261012-194226-402151/role64.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1 -DH3_PAD_M=1 -DH3_ROLE_STAGES=2 -DH3_ROLE_WRAP=1 -DH3_ROLE_CACHE_A=1 -DH3_ROLE_CACHE_B=1 -DH3_ROLE_CACHE_LLVM=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v086"></a>
## V086 — gemm.cpp

候选名：`large_wk16_kpack4_g8_pad_m_reg768`

实测库 SHA256：`8d1c0e887b5090e0512c7ad3ecc6aca2e5b53fb10601eb933cfa75f222a0d4b2`；首次出现：`stage_aa_k16_residency`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_wk16_kpack4_g8_pad_m_reg768/20261012-195033-052067/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_wk16_kpack4_g8_pad_m_reg768/20261012-195033-052067/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=16 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_WK=16 -DH3_GROUP_M=8 -DH3_LDS_KPACK=4 -DH3_PAD_M=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v087"></a>
## V087 — gemm.cpp

候选名：`large_wk16_kpack8_g8_pad_m_reg768`

实测库 SHA256：`2918a8b108dbf3c17209f49d7abe7e86ba0d374ff60e48df3e5f63a546f791c1`；首次出现：`stage_aa_k16_residency`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_wk16_kpack8_g8_pad_m_reg768/20261012-195033-052688/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_wk16_kpack8_g8_pad_m_reg768/20261012-195033-052688/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=16 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_WK=16 -DH3_GROUP_M=8 -DH3_LDS_KPACK=8 -DH3_PAD_M=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v088"></a>
## V088 — gemm.cpp

候选名：`large_wk16_pad8_g8_pad_m_reg768`

实测库 SHA256：`3c0300bf24ca95e1a00f54874cbce8db05cfcf28894c3ded4f954be1b29fe8b9`；首次出现：`stage_aa_k16_residency`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_wk16_pad8_g8_pad_m_reg768/20261012-194656-986833/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_wk16_pad8_g8_pad_m_reg768/20261012-194656-986833/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=16 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=8 -DH3_WK=16 -DH3_GROUP_M=8 -DH3_PAD_M=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v089"></a>
## V089 — gemm.cpp

候选名：`large_wk16_pad16_g8_pad_m_reg768`

实测库 SHA256：`73fe53b4693fd4d24cf6be0e25f8a08a642fd55fec618c2ae8c992c863e3c934`；首次出现：`stage_aa_k16_residency`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_wk16_pad16_g8_pad_m_reg768/20261012-195149-165818/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_wk16_pad16_g8_pad_m_reg768/20261012-195149-165818/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=16 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_WK=16 -DH3_GROUP_M=8 -DH3_PAD_M=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v090"></a>
## V090 — role64.cpp

候选名：`role64_double_wrap_noprio_reg768`

实测库 SHA256：`76379f23cdaeeb3098ed5b75f893537bbc6bc2fbca98376f87959702084b596f`；首次出现：`stage_ab_scheduling_controls`。

当前入口：[role64.cpp](<../role64.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_noprio_reg768/20261012-200034-590750/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_noprio_reg768/20261012-200034-590750/role64.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1 -DH3_PAD_M=1 -DH3_ROLE_STAGES=2 -DH3_ROLE_WRAP=1 -DH3_ROLE_NOPRIO=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v091"></a>
## V091 — role64.cpp

候选名：`role64_double_wrap_nosched_reg768`

实测库 SHA256：`f5336e722bbb636fe7ccddc93d9d14084070592a1c32d22ddbfb95e6bcc15ffb`；首次出现：`stage_ab_scheduling_controls`。

当前入口：[role64.cpp](<../role64.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_nosched_reg768/20261012-200034-591278/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_nosched_reg768/20261012-200034-591278/role64.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1 -DH3_PAD_M=1 -DH3_ROLE_STAGES=2 -DH3_ROLE_WRAP=1 -DH3_ROLE_NOSCHED=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v092"></a>
## V092 — role64.cpp

候选名：`role64_double_wrap_nosched_noprio_reg768`

实测库 SHA256：`2dbd9ab51bf4769f0f56b758f81aa17c3737711149b25b44d82defc4adf30f5c`；首次出现：`stage_ab_scheduling_controls`。

当前入口：[role64.cpp](<../role64.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_nosched_noprio_reg768/20261012-200039-749758/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_nosched_noprio_reg768/20261012-200039-749758/role64.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1 -DH3_PAD_M=1 -DH3_ROLE_STAGES=2 -DH3_ROLE_WRAP=1 -DH3_ROLE_NOPRIO=1 -DH3_ROLE_NOSCHED=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v093"></a>
## V093 — role64.cpp

候选名：`role64_double_wrap_postprio_reg768`

实测库 SHA256：`0ea53cda69752b7c7d00582eea6d5570cac1741ce2f546c24a293170a93bbf0e`；首次出现：`stage_ab_scheduling_controls`。

当前入口：[role64.cpp](<../role64.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_postprio_reg768/20261012-200039-784613/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_postprio_reg768/20261012-200039-784613/role64.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1 -DH3_PAD_M=1 -DH3_ROLE_STAGES=2 -DH3_ROLE_WRAP=1 -DH3_ROLE_POSTPRIO=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v094"></a>
## V094 — role64.cpp

候选名：`role64_double_wrap_trans_reg768`

实测库 SHA256：`2f6106adb0681ef1fa431f8ea8c18730e5168626829d403c791c88eee954c548`；首次出现：`stage_ac_blas_operand_orientation`。

当前入口：[role64.cpp](<../role64.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_trans_reg768/20261012-200654-851715/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_trans_reg768/20261012-200654-851715/role64.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1 -DH3_PAD_M=1 -DH3_ROLE_STAGES=2 -DH3_ROLE_WRAP=1 -DH3_ROLE_TRANSPOSE=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v095"></a>
## V095 — role64.cpp

候选名：`role64_double_wrap_trans_groupn_reg768`

实测库 SHA256：`b63d376a41da4503eb39939c117fec71dd292e58f6661e936e57547ea949b0a5`；首次出现：`stage_ac_blas_operand_orientation`。

当前入口：[role64.cpp](<../role64.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_trans_groupn_reg768/20261012-200654-852228/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_trans_groupn_reg768/20261012-200654-852228/role64.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1 -DH3_PAD_M=1 -DH3_ROLE_STAGES=2 -DH3_ROLE_WRAP=1 -DH3_ROLE_TRANSPOSE=1 -DH3_ROLE_GROUP_N=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v096"></a>
## V096 — role64.cpp

候选名：`role64_double_wrap_trans_groupn_nosched_noprio_reg768`

实测库 SHA256：`5886594748c2f8844d1ce92755f19f621318aee5217893c2cc14b35e771829a8`；首次出现：`stage_ac_blas_operand_orientation`。

当前入口：[role64.cpp](<../role64.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_trans_groupn_nosched_noprio_reg768/20261012-200700-186763/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_trans_groupn_nosched_noprio_reg768/20261012-200700-186763/role64.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1 -DH3_PAD_M=1 -DH3_ROLE_STAGES=2 -DH3_ROLE_WRAP=1 -DH3_ROLE_NOPRIO=1 -DH3_ROLE_NOSCHED=1 -DH3_ROLE_TRANSPOSE=1 -DH3_ROLE_GROUP_N=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v097"></a>
## V097 — gemm.cpp

候选名：`large_pad16_g8_trans_pad_m_reg768`

实测库 SHA256：`68b486eeaee833998b7890fcb598cae24b787ff70e3f603f42d12c1e8153758b`；首次出现：`stage_ad_generic_column_output`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_trans_pad_m_reg768/20261012-201931-431533/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_trans_pad_m_reg768/20261012-201931-431533/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_GROUP_M=8 -DH3_PAD_M=1 -DH3_GEMM_TRANSPOSE=1 -DH3_PAD_N=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v098"></a>
## V098 — gemm.cpp

候选名：`large_pad16_g8_trans_groupn_pad_m_reg768`

实测库 SHA256：`242c8cc3ab0f923483efcd73589af5fd51dcf9d78d5ef5f828c033b4d61e2e39`；首次出现：`stage_ad_generic_column_output`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_trans_groupn_pad_m_reg768/20261012-201931-432032/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_trans_groupn_pad_m_reg768/20261012-201931-432032/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_GROUP_M=8 -DH3_PAD_M=1 -DH3_GEMM_TRANSPOSE=1 -DH3_PAD_N=1 -DH3_GROUP_N=8
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v099"></a>
## V099 — gemm.cpp

候选名：`large_kpack8_g8_trans_groupn_pad_m_reg768`

实测库 SHA256：`b8cd7750108fb417be08e0c33329c561711c0463b9a037140203beaec256b69d`；首次出现：`stage_ad_generic_column_output`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_kpack8_g8_trans_groupn_pad_m_reg768/20261012-202048-479995/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_kpack8_g8_trans_groupn_pad_m_reg768/20261012-202048-479995/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_GROUP_M=8 -DH3_RAW_BARRIER=1 -DH3_LDS_KPACK=8 -DH3_PAD_M=1 -DH3_GEMM_TRANSPOSE=1 -DH3_PAD_N=1 -DH3_GROUP_N=8
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v100"></a>
## V100 — gemm.cpp

候选名：`large_pad16_g8_trans_groupn_lpad1_pad_m_reg768`

实测库 SHA256：`4ee6c3092e76f71f1ca712dd65f4fc6cdda91ab0e62b4a2454ae66f827131b77`；首次出现：`stage_ae_vgpr_allocation_shift`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_trans_groupn_lpad1_pad_m_reg768/20261012-203430-467048/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_trans_groupn_lpad1_pad_m_reg768/20261012-203430-467048/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_GROUP_M=8 -DH3_PAD_M=1 -DH3_GEMM_TRANSPOSE=1 -DH3_PAD_N=1 -DH3_GROUP_N=8 -DH3_LIVE_PAD=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v101"></a>
## V101 — gemm.cpp

候选名：`large_pad16_g8_trans_groupn_lpad2_pad_m_reg768`

实测库 SHA256：`a103c2b11310d37d1aa9cc5dc561308221d51b229d56578f4d2abe60bee3fc59`；首次出现：`stage_ae_vgpr_allocation_shift`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_trans_groupn_lpad2_pad_m_reg768/20261012-203430-467722/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_trans_groupn_lpad2_pad_m_reg768/20261012-203430-467722/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_GROUP_M=8 -DH3_PAD_M=1 -DH3_GEMM_TRANSPOSE=1 -DH3_PAD_N=1 -DH3_GROUP_N=8 -DH3_LIVE_PAD=2
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v102"></a>
## V102 — gemm.cpp

候选名：`large_pad16_g8_trans_groupn_lpad3_pad_m_reg768`

实测库 SHA256：`a4c6809c5e8d69b1c9435f0135df2715b791d7d3b785a8af2eeb3cb89a108e45`；首次出现：`stage_ae_vgpr_allocation_shift`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_trans_groupn_lpad3_pad_m_reg768/20261012-203546-795897/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_trans_groupn_lpad3_pad_m_reg768/20261012-203546-795897/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_GROUP_M=8 -DH3_PAD_M=1 -DH3_GEMM_TRANSPOSE=1 -DH3_PAD_N=1 -DH3_GROUP_N=8 -DH3_LIVE_PAD=3
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v103"></a>
## V103 — gemm.cpp

候选名：`large_pad16_g8_trans_groupn_aligned_pad_m_reg768`

实测库 SHA256：`5695e4a4bb14a0cc26777602e2dc03d8f077d77700eee3ca6c5832fb813971bd`；首次出现：`stage_af_contiguous_vector_restore`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_trans_groupn_aligned_pad_m_reg768/20261013-101818-236282/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_trans_groupn_aligned_pad_m_reg768/20261013-101818-236282/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_GROUP_M=8 -DH3_PAD_M=1 -DH3_GEMM_TRANSPOSE=1 -DH3_PAD_N=0 -DH3_GROUP_N=8
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v104"></a>
## V104 — gemm.cpp

候选名：`large_pad16_g8_trans_groupn_vecb_pad_m_reg768`

实测库 SHA256：`cfddb1994814a1b26e7f203a379293f873b516a48718844aa53beb3d57d17d75`；首次出现：`stage_af_contiguous_vector_restore`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_trans_groupn_vecb_pad_m_reg768/20261013-102011-099731/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_trans_groupn_vecb_pad_m_reg768/20261013-102011-099731/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_GROUP_M=8 -DH3_PAD_M=1 -DH3_GEMM_TRANSPOSE=1 -DH3_PAD_N=1 -DH3_GROUP_N=8 -DH3_CONTIGUOUS_B_VECTOR=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v105"></a>
## V105 — role64.cpp

候选名：`role64_double_wrap_trans_groupn_nosched_noprio_cachea_reg768`

实测库 SHA256：`7ae7eb19430445da17015dae48992404478f8ee83ff5a98a95584a8b50c4c784`；首次出现：`stage_ag_transposed_cache_swizzle`。

当前入口：[role64.cpp](<../role64.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_trans_groupn_nosched_noprio_cachea_reg768/20261013-102338-137839/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_trans_groupn_nosched_noprio_cachea_reg768/20261013-102338-137839/role64.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1 -DH3_PAD_M=1 -DH3_ROLE_STAGES=2 -DH3_ROLE_WRAP=1 -DH3_ROLE_CACHE_A=1 -DH3_ROLE_NOPRIO=1 -DH3_ROLE_NOSCHED=1 -DH3_ROLE_TRANSPOSE=1 -DH3_ROLE_GROUP_N=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v106"></a>
## V106 — role64.cpp

候选名：`role64_double_wrap_trans_groupn_nosched_noprio_cacheb_llvm_reg768`

实测库 SHA256：`370450f15919bd10f9c936ac707d2023eb7ccc90cd8ced84074e83d35051cdbc`；首次出现：`stage_ag_transposed_cache_swizzle`。

当前入口：[role64.cpp](<../role64.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_trans_groupn_nosched_noprio_cacheb_llvm_reg768/20261013-102338-138342/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_trans_groupn_nosched_noprio_cacheb_llvm_reg768/20261013-102338-138342/role64.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1 -DH3_PAD_M=1 -DH3_ROLE_STAGES=2 -DH3_ROLE_WRAP=1 -DH3_ROLE_CACHE_B=1 -DH3_ROLE_CACHE_LLVM=1 -DH3_ROLE_NOPRIO=1 -DH3_ROLE_NOSCHED=1 -DH3_ROLE_TRANSPOSE=1 -DH3_ROLE_GROUP_N=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v107"></a>
## V107 — role64.cpp

候选名：`role64_double_wrap_trans_groupn_nosched_noprio_cacheab_llvm_reg768`

实测库 SHA256：`12c563cdb30ca47d3b878b00de0d56abff2a8625f36514fce8d4712ea32001f0`；首次出现：`stage_ag_transposed_cache_swizzle`。

当前入口：[role64.cpp](<../role64.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_trans_groupn_nosched_noprio_cacheab_llvm_reg768/20261013-102343-432572/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/role64_double_wrap_trans_groupn_nosched_noprio_cacheab_llvm_reg768/20261013-102343-432572/role64.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_DOUBLE_BUFFER=1 -DH3_PAD_M=1 -DH3_ROLE_STAGES=2 -DH3_ROLE_WRAP=1 -DH3_ROLE_CACHE_A=1 -DH3_ROLE_CACHE_B=1 -DH3_ROLE_CACHE_LLVM=1 -DH3_ROLE_NOPRIO=1 -DH3_ROLE_NOSCHED=1 -DH3_ROLE_TRANSPOSE=1 -DH3_ROLE_GROUP_N=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v108"></a>
## V108 — gemm.cpp

候选名：`large_pad16_g8_trans_groupn4_vecb_pad_m_reg768`

实测库 SHA256：`f19633290ab7c9ca51f370b51b55c43e58f8cd92dbb39c6db4da5bf72ef02e09`；首次出现：`stage_ah_vector_group_tile`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_trans_groupn4_vecb_pad_m_reg768/20261013-102719-380265/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_trans_groupn4_vecb_pad_m_reg768/20261013-102719-380265/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_GROUP_M=8 -DH3_PAD_M=1 -DH3_GEMM_TRANSPOSE=1 -DH3_PAD_N=1 -DH3_GROUP_N=4 -DH3_CONTIGUOUS_B_VECTOR=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v109"></a>
## V109 — gemm.cpp

候选名：`large_pad16_g8_trans_groupn16_vecb_pad_m_reg768`

实测库 SHA256：`b141bddd878b5cb998496473c7ab5477c9cd29d6355de831cb0954a4c0e19d73`；首次出现：`stage_ah_vector_group_tile`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_trans_groupn16_vecb_pad_m_reg768/20261013-102719-380665/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_trans_groupn16_vecb_pad_m_reg768/20261013-102719-380665/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_GROUP_M=8 -DH3_PAD_M=1 -DH3_GEMM_TRANSPOSE=1 -DH3_PAD_N=1 -DH3_GROUP_N=16 -DH3_CONTIGUOUS_B_VECTOR=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v110"></a>
## V110 — gemm.cpp

候选名：`large_pad16_g8_trans_groupn32_vecb_pad_m_reg768`

实测库 SHA256：`043532cb576be4a405b5c559ae6acad6f360674fb39a80f89ff310a0e26c8d94`；首次出现：`stage_ah_vector_group_tile`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_trans_groupn32_vecb_pad_m_reg768/20261013-102834-413617/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/large_pad16_g8_trans_groupn32_vecb_pad_m_reg768/20261013-102834-413617/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=32 -DH3_WM=4 -DH3_WN=2 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_GROUP_M=8 -DH3_PAD_M=1 -DH3_GEMM_TRANSPOSE=1 -DH3_PAD_N=1 -DH3_GROUP_N=32 -DH3_CONTIGUOUS_B_VECTOR=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v111"></a>
## V111 — gemm.cpp

候选名：`tokenwide_pad16_g8_trans_groupn_vecb_pad_m_reg768`

实测库 SHA256：`1aefaadd98896761d9528224bf6c7441ba4fbc8f1733c55cf4fd0e9c12bf98cb`；首次出现：`stage_ah_vector_group_tile`。

当前入口：[gemm.cpp](<../gemm.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/tokenwide_pad16_g8_trans_groupn_vecb_pad_m_reg768/20261013-102835-368723/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/tokenwide_pad16_g8_trans_groupn_vecb_pad_m_reg768/20261013-102835-368723/gemm.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=512 -DH3_BK=32 -DH3_WM=2 -DH3_WN=4 -DH3_SWIZZLE=0 -DH3_RAW_BARRIER=1 -DH3_LDS_PAD=16 -DH3_GROUP_M=8 -DH3_PAD_M=1 -DH3_GEMM_TRANSPOSE=1 -DH3_PAD_N=1 -DH3_GROUP_N=8 -DH3_CONTIGUOUS_B_VECTOR=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v112"></a>
## V112 — rolephase.cpp

候选名：`rolephase_m1_reg768`

实测库 SHA256：`3ef9c0533b24267a260d33fbf4e8c3db110d6e03a85e8e41b01ac464540fc64b`；首次出现：`stage_ai_phase_register_prefetch`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_reg768/20261013-103510-263776/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_reg768/20261013-103510-263776/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v113"></a>
## V113 — rolephase.cpp

候选名：`rolephase_m2_reg768`

实测库 SHA256：`30c98cb9ab059775add158c5c866f173fabf96f9ebec24678d1aa17bf0fba48c`；首次出现：`stage_ai_phase_register_prefetch`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m2_reg768/20261013-103510-264198/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m2_reg768/20261013-103510-264198/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=2
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v114"></a>
## V114 — rolephase.cpp

候选名：`rolephase_m4_reg768`

实测库 SHA256：`a557d2521630e7f5c95b2a073a978498774ea7cf8da2b14c050107fe12bb5b74`；首次出现：`stage_ai_phase_register_prefetch`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m4_reg768/20261013-103515-534987/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m4_reg768/20261013-103515-534987/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=4
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v115"></a>
## V115 — rolephase.cpp

候选名：`rolephase_m4_priority_reg768`

实测库 SHA256：`486b0936221e8c96e88b01fff264a40cdc1c8ad42bf606442f7f66f604cfa48c`；首次出现：`stage_ai_phase_register_prefetch`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m4_priority_reg768/20261013-103515-649207/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m4_priority_reg768/20261013-103515-649207/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=4 -DH3_PHASE_PRIORITY=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v116"></a>
## V116 — rolephase.cpp

候选名：`rolephase_m1_cachea_reg768`

实测库 SHA256：`594cfb370b4d0ddd52464b2aac8cb28c593bfb64a1b0c6f49d9e8e6e37e7e83c`；首次出现：`stage_aj_phase_cache_swizzle`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_cachea_reg768/20261013-104039-040982/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_cachea_reg768/20261013-104039-040982/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_CACHE_A=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v117"></a>
## V117 — rolephase.cpp

候选名：`rolephase_m1_cacheb_reg768`

实测库 SHA256：`95dd6ec2399f0eea3cba79799e0233c98a5161181ea35ec11f2b555e46857b0b`；首次出现：`stage_aj_phase_cache_swizzle`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_cacheb_reg768/20261013-104039-041404/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_cacheb_reg768/20261013-104039-041404/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_CACHE_B=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v118"></a>
## V118 — rolephase.cpp

候选名：`rolephase_m1_cacheab_reg768`

实测库 SHA256：`3b8d3d1c565a0415034fbf7749b3175e931eac9bdd13e78f54cee09b559a42f2`；首次出现：`stage_aj_phase_cache_swizzle`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_cacheab_reg768/20261013-104044-285101/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_cacheab_reg768/20261013-104044-285101/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_CACHE_A=1 -DH3_PHASE_CACHE_B=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v119"></a>
## V119 — rolephase.cpp

候选名：`rolephase_m1_priority_reg768`

实测库 SHA256：`ead63b34745fe0490de278e08370ad991aa3f7f5cd57428f6225df1b69d15b2e`；首次出现：`stage_ak_banked_wait_schedule`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_priority_reg768/20261013-104936-660136/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_priority_reg768/20261013-104936-660136/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_PRIORITY=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v120"></a>
## V120 — rolephase.cpp

候选名：`rolephase_m1_bank_reg768`

实测库 SHA256：`91c1ed295a244e3a98dc353ade5ddabb526e090de1d2a29eac82f62859541539`；首次出现：`stage_ak_banked_wait_schedule`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_reg768/20261013-104936-660702/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_reg768/20261013-104936-660702/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v121"></a>
## V121 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_reg768`

实测库 SHA256：`d89f4906cd47339c8ccb9d920f3d22c5d05c1bf8479e7d26bd8cffa59cb207fc`；首次出现：`stage_ak_banked_wait_schedule`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_reg768/20261013-104941-946933/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_reg768/20261013-104941-946933/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v122"></a>
## V122 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_priority_reg768`

实测库 SHA256：`73c9e91c6ca962de810a3d19cb3131cde6e831bd058784445871f18b36ae3233`；首次出现：`stage_ak_banked_wait_schedule`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_priority_reg768/20261013-104942-012285/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_priority_reg768/20261013-104942-012285/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_PRIORITY=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v123"></a>
## V123 — rolephase.cpp

候选名：`rolephase_m1_weight32_reg768`

实测库 SHA256：`fbf30f49b94c05ff1a24f5250936e4836b86077721a442f5dcf270900cbce416`；首次出现：`stage_al_weight_k32_buffer`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_weight32_reg768/20261013-105845-506492/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_weight32_reg768/20261013-105845-506492/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_WEIGHT_K=32
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v124"></a>
## V124 — rolephase.cpp

候选名：`rolephase_m1_weight32_bank_asmb_reg768`

实测库 SHA256：`10b18e992e1c8a16321ee94960aeffed14f23fe292d75042d72ec5cf61f3d576`；首次出现：`stage_al_weight_k32_buffer`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_weight32_bank_asmb_reg768/20261013-105845-506818/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_weight32_bank_asmb_reg768/20261013-105845-506818/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_WEIGHT_K=32
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v125"></a>
## V125 — rolephase.cpp

候选名：`rolephase_m1_weight32_bank_asmb_priority_reg768`

实测库 SHA256：`94d12c0d866d993c790bde9284775a939d4c452903c27dcd214eba478febc9d4`；首次出现：`stage_al_weight_k32_buffer`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_weight32_bank_asmb_priority_reg768/20261013-105850-773584/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_weight32_bank_asmb_priority_reg768/20261013-105850-773584/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_PRIORITY=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_WEIGHT_K=32
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v126"></a>
## V126 — rolephase.cpp

候选名：`rolephase_m1_readywait_bank_asmb_reg768`

实测库 SHA256：`c7abb2ab57e3d1242942522b5ab39e8c008bde05c11bf8a56128deca4308645f`；首次出现：`stage_am_ready_wait_and_phase_stagger`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_readywait_bank_asmb_reg768/20261013-112028-155572/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_readywait_bank_asmb_reg768/20261013-112028-155572/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_READY_WAIT=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v127"></a>
## V127 — rolephase.cpp

候选名：`rolephase_m1_swap_bank_asmb_reg768`

实测库 SHA256：`4a00942f73fc05853496e5b029e988f08dc7e5367e3c95428d88d97cadd7996c`；首次出现：`stage_am_ready_wait_and_phase_stagger`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_swap_bank_asmb_reg768/20261013-112028-156025/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_swap_bank_asmb_reg768/20261013-112028-156025/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_SWAP=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v128"></a>
## V128 — rolephase.cpp

候选名：`rolephase_m1_readywait_swap_bank_asmb_reg768`

实测库 SHA256：`ec6b59103c9bd24ca901a4d2176ca6cec87f7517eb48c2f91d6165abbba3dab6`；首次出现：`stage_am_ready_wait_and_phase_stagger`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_readywait_swap_bank_asmb_reg768/20261013-112033-430778/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_readywait_swap_bank_asmb_reg768/20261013-112033-430778/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_READY_WAIT=1 -DH3_PHASE_SWAP=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v129"></a>
## V129 — rolephase.cpp

候选名：`rolephase_m1_readywait_bank_asmb_priority_reg768`

实测库 SHA256：`c681fc85954b24cbd1b086d5dfea3fe082a315690e0dd1f170d93e7f35bb4f46`；首次出现：`stage_am_ready_wait_and_phase_stagger`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_readywait_bank_asmb_priority_reg768/20261013-112033-534967/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_readywait_bank_asmb_priority_reg768/20261013-112033-534967/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_PRIORITY=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_READY_WAIT=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v130"></a>
## V130 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_dsasm_reg768`

实测库 SHA256：`27fd3a4061f5f8bf354b3486f999a9b0f6bd5ce74ebdc93ba2f3bd71759ec8df`；首次出现：`stage_an_ordered_lds_reads`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_dsasm_reg768/20261013-112733-859423/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_dsasm_reg768/20261013-112733-859423/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_DS_ASM=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v131"></a>
## V131 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_dsasm_postprio_reg768`

实测库 SHA256：`22d3e7400a85c9987265e0fd07233f7b1aca0bb84ed1abbf655842b4ca0c77e5`；首次出现：`stage_an_ordered_lds_reads`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_dsasm_postprio_reg768/20261013-112733-859902/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_dsasm_postprio_reg768/20261013-112733-859902/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_DS_ASM=1 -DH3_PHASE_POSTPRIO=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v132"></a>
## V132 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_dsprogress_reg768`

实测库 SHA256：`ab9be1505178f97ff392f7d59c159e16c66e0e4223bcd57f2983a02e101935c7`；首次出现：`stage_an_ordered_lds_reads`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_dsprogress_reg768/20261013-112739-173580/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_dsprogress_reg768/20261013-112739-173580/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_DS_ASM=1 -DH3_PHASE_DS_PROGRESS=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v133"></a>
## V133 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_lpad1_reg768`

实测库 SHA256：`9c9300e1fe52018eb926ea15102b69102fdfe7d8893352a39ab788570e39b7ab`；首次出现：`stage_ao_phase_vgpr_alignment`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_lpad1_reg768/20261013-113150-008401/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_lpad1_reg768/20261013-113150-008401/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_LIVE_PAD=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v134"></a>
## V134 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_lpad2_reg768`

实测库 SHA256：`0c0fe9e711bd8e64fef6552f6264c161cb5392a8deb2c009d9749c22e20932db`；首次出现：`stage_ao_phase_vgpr_alignment`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_lpad2_reg768/20261013-113150-008802/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_lpad2_reg768/20261013-113150-008802/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_LIVE_PAD=2
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v135"></a>
## V135 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_lpad3_reg768`

实测库 SHA256：`cb1fcef11e6837202913d68299a538e3d235393456216feda353e2f70870026a`；首次出现：`stage_ao_phase_vgpr_alignment`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_lpad3_reg768/20261013-113155-335597/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_lpad3_reg768/20261013-113155-335597/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_LIVE_PAD=3
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v136"></a>
## V136 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_dsasm_lpad1_reg768`

实测库 SHA256：`027611bc85d16ebda5296a7fbd4704098613f3f3779bc5f79879c1ea790d318a`；首次出现：`stage_ao_phase_vgpr_alignment`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_dsasm_lpad1_reg768/20261013-113155-367605/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_dsasm_lpad1_reg768/20261013-113155-367605/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_DS_ASM=1 -DH3_LIVE_PAD=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v137"></a>
## V137 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_mid_reg768`

实测库 SHA256：`93666e53d9cbe0fbbba31abbf1fb661183114b24f3b7d3d5404931dc8256b389`；首次出现：`stage_ap_mixed_prefetch_and_structured_buffer`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_mid_reg768/20261013-113814-593398/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_mid_reg768/20261013-113814-593398/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

**留存缺口：当前同名.so与该历史哈希不同，暂未找到匹配原始.so；尚未重建核对。**

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v138"></a>
## V138 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_midgroup_reg768`

实测库 SHA256：`f27df94c16da3a02809a92f90f6c821e82d92ab9c83536073177b9357c6a90fe`；首次出现：`stage_ap_mixed_prefetch_and_structured_buffer`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_midgroup_reg768/20261013-113814-593710/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_midgroup_reg768/20261013-113814-593710/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v139"></a>
## V139 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_cachea_reg768`

实测库 SHA256：`1441d33d22735da9331b06444345bd68bf794771531c87e583f4dc611bf0574d`；首次出现：`stage_ap_mixed_prefetch_and_structured_buffer`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_cachea_reg768/20261013-113819-827152/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_cachea_reg768/20261013-113819-827152/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_CACHE_A=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v140"></a>
## V140 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_cacheb_reg768`

实测库 SHA256：`9f7b9e811a445c3ec6de6f76189191f6ec0758a29ab511d44bf7199bbcc90975`；首次出现：`stage_ap_mixed_prefetch_and_structured_buffer`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_cacheb_reg768/20261013-113819-941958/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_cacheb_reg768/20261013-113819-941958/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_CACHE_B=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v141"></a>
## V141 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_cacheab_reg768`

实测库 SHA256：`69944abb7307754013f56825d37e38625959a9b24e93ae8226b6306b91bec304`；首次出现：`stage_ap_mixed_prefetch_and_structured_buffer`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_cacheab_reg768/20261013-113825-021278/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_cacheab_reg768/20261013-113825-021278/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_CACHE_A=1 -DH3_PHASE_CACHE_B=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v142"></a>
## V142 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_reg768`；`rolephase_m1_bank_asmb_always_unroll2_reg768`

实测库 SHA256：`cd4e1da9bcce00cc64c51dfc81e3173d9f43f5e092b7d2385e3854446d21bf86`；首次出现：`stage_aq_branchfree_prefetch_and_unroll`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_reg768/20261013-114241-388028/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_reg768/20261013-114241-388028/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_ALWAYS_PREFETCH=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v143"></a>
## V143 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_cacheb_reg768`

实测库 SHA256：`2f8437bafc3e14eced0c3d1c9f9939029fae8de656d0baef1bef9b93ae50d92d`；首次出现：`stage_aq_branchfree_prefetch_and_unroll`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_cacheb_reg768/20261013-114241-388445/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_cacheb_reg768/20261013-114241-388445/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_CACHE_B=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_ALWAYS_PREFETCH=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v144"></a>
## V144 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_mid_reg768`

实测库 SHA256：`70c43fb3186a613064ae6967a87860c3c893b749abf663521d57fb8dcd257a7f`；首次出现：`stage_ar_mixed_prefetch_rebuild_verify`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_mid_reg768/20261013-114533-924080/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_mid_reg768/20261013-114533-924080/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=0
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v145"></a>
## V145 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_reg768`

实测库 SHA256：`b58492f49d35103ddba37ad4fe441de8aaf1368e4da14dfe1666d4548744eaec`；首次出现：`stage_as_combined_mixed_branchfree`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_reg768/20261013-114800-452719/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_reg768/20261013-114800-452719/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v146"></a>
## V146 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_dsprogress_reg768`

实测库 SHA256：`e854931ab8a6a7fb4aa50a53ff4da8bf1281d291b1c77e024d587224cd1445c3`；首次出现：`stage_as_combined_mixed_branchfree`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_dsprogress_reg768/20261013-114800-453053/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_dsprogress_reg768/20261013-114800-453053/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_DS_ASM=1 -DH3_PHASE_DS_PROGRESS=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v147"></a>
## V147 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_bufferepi_reg768`

实测库 SHA256：`3a6e10f971bf4add2c4f85fe772186d9c37ccc495859e44b156def000abfe2c0`；首次出现：`stage_au_bounded_buffer_output`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_bufferepi_reg768/20261013-120145-195149/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_bufferepi_reg768/20261013-120145-195149/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_ALWAYS_PREFETCH=0 -DH3_PHASE_BUFFER_EPILOGUE=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v148"></a>
## V148 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_mid_bufferepi_reg768`

实测库 SHA256：`bd1061d23f6af0ae36964585b0ec22573d2370393e786001635779831a958a35`；首次出现：`stage_au_bounded_buffer_output`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_mid_bufferepi_reg768/20261013-120145-195527/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_mid_bufferepi_reg768/20261013-120145-195527/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=0 -DH3_PHASE_BUFFER_EPILOGUE=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v149"></a>
## V149 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_compat_reg768`；`rolephase_m1_bank_asmb_always_mid_bufferepi_reg768`

实测库 SHA256：`a0e23fd8c4751c2f7843e782ab7e717dcd82c1a2c021973ad78e807745a2bc8b`；首次出现：`stage_au_bounded_buffer_output`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_compat_reg768/20261013-123615-975769/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_compat_reg768/20261013-123615-975769/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v150"></a>
## V150 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid4_bufferepi_reg768`

实测库 SHA256：`e91d70d90155ce7a8cdd00aebbdb3531e396784d5b15fcaeffcfbec468578f0d`；首次出现：`stage_av_xprof_guided_prefetch_points`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid4_bufferepi_reg768/20261013-122622-663988/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid4_bufferepi_reg768/20261013-122622-663988/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_MID_POINT=4 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v151"></a>
## V151 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid12_bufferepi_reg768`

实测库 SHA256：`8339f406a5add1a3fc5217344c2051f0ba90dcc51181432b69691eb37f04903a`；首次出现：`stage_av_xprof_guided_prefetch_points`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid12_bufferepi_reg768/20261013-122622-664327/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid12_bufferepi_reg768/20261013-122622-664327/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_MID_POINT=12 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v152"></a>
## V152 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_midsplit_bufferepi_reg768`

实测库 SHA256：`e8ba1e5c5014c4f8fdf16600f808412853ebfbd777af23aa55b46a97ec882cce`；首次出现：`stage_av_xprof_guided_prefetch_points`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_midsplit_bufferepi_reg768/20261013-122627-806394/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_midsplit_bufferepi_reg768/20261013-122627-806394/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_MID_SPLIT=1 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v153"></a>
## V153 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_mathfence_reg768`

实测库 SHA256：`651d9ccc77e07316270f1220c5a08f88926e03606c560758c03c170765edfb21`；首次出现：`stage_aw_mmac_burst_boundaries`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_mathfence_reg768/20261013-123610-653253/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_mathfence_reg768/20261013-123610-653253/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_MATH_FENCE=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v154"></a>
## V154 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_burst_reg768`

实测库 SHA256：`3fd7c06fb76757ff36145b4ed999ac2cc6f0bbfbe86121cb8e0ed1b8d0274aff`；首次出现：`stage_aw_mmac_burst_boundaries`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_burst_reg768/20261013-123610-653584/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_burst_reg768/20261013-123610-653584/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_BURST=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v155"></a>
## V155 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_burst_postprio_reg768`

实测库 SHA256：`cbb1ede642028a9acf6eaee47781fb551be1845abeab93c1a795230e082635c6`；首次出现：`stage_aw_mmac_burst_boundaries`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_burst_postprio_reg768/20261013-123615-813383/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_burst_postprio_reg768/20261013-123615-813383/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_BURST=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v156"></a>
## V156 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_b64p0_reg768`

实测库 SHA256：`77cf2915f3b705bfacd377d37f924208b6416fa8d02ccee86ffbac2eda3eb8d8`；首次出现：`stage_ax_k64_activation_banks`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_b64p0_reg768/20261013-124133-203490/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_b64p0_reg768/20261013-124133-203490/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_B64=1 -DH3_PHASE_B64_PREFETCH_PHASE=0
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v157"></a>
## V157 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_b64p1_reg768`

实测库 SHA256：`d8b5154477248ef32d0c72a363717b552d4eacaa6552f52d0ac98d28e4cdcef8`；首次出现：`stage_ax_k64_activation_banks`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_b64p1_reg768/20261013-124133-204161/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_b64p1_reg768/20261013-124133-204161/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_B64=1 -DH3_PHASE_B64_PREFETCH_PHASE=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v158"></a>
## V158 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_compat_reg768`；`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_reg768`；`rolephase_m1_bank_asmb_always_mid_bufferepi_selectrne_reg768`

实测库 SHA256：`f046d922a9fc1ea3c003bcb674b6424c0d42595c8b86e95204973c49f2a4f38f`；首次出现：`stage_ay_branchless_bf16_output`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_compat_reg768/20261013-130145-977891/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_compat_reg768/20261013-130145-977891/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v159"></a>
## V159 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_compat_reg768`；`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_reg768`

实测库 SHA256：`b44b845565a273e2986622fca58f599703511796779171aa0f0f7716ac16ac47`；首次出现：`stage_ba_producer_consumer_cadence`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_compat_reg768/20261013-133105-538232/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_compat_reg768/20261013-133105-538232/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v160"></a>
## V160 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_postprio_reg768`

实测库 SHA256：`2a22041535ceba30d8e5c653f8447025bf22282acec5f4dc652c8c852b6a8e5a`；首次出现：`stage_ba_producer_consumer_cadence`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_postprio_reg768/20261013-125904-121404/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_postprio_reg768/20261013-125904-121404/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v161"></a>
## V161 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_finalwait_reg768`

实测库 SHA256：`4c01bf180182458b2149f5f422fd99a500082c5b271becaf03272e2d72cb63fd`；首次出现：`stage_bc_native_pow2_b`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_finalwait_reg768/20261013-132701-639291/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_finalwait_reg768/20261013-132701-639291/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_FINAL_VM_WAIT=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v162"></a>
## V162 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pow2b_reg768`

实测库 SHA256：`09d02d381cb706935d401b72d5634ca3278ef4fa8939a9472a6c8012b39e049e`；首次出现：`stage_bc_native_pow2_b`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pow2b_reg768/20261013-132701-639678/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pow2b_reg768/20261013-132701-639678/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_POW2_B=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v163"></a>
## V163 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pow2b_bias16_reg768`

实测库 SHA256：`1ce75390c87b6f81e73edbcc156bea78688a39e5a5b2777ffa234e6cadab2411`；首次出现：`stage_bc_native_pow2_b`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pow2b_bias16_reg768/20261013-132706-752135/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pow2b_bias16_reg768/20261013-132706-752135/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_POW2_B=1 -DH3_PHASE_B_BIAS=16
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v164"></a>
## V164 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_abase_reg768`

实测库 SHA256：`f6e71be9c7f8441daa76f56cff7a3359fec31a8b078cef1784853479cb3a8780`；首次出现：`stage_bd_scalar_addressing`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_abase_reg768/20261013-133214-005862/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_abase_reg768/20261013-133214-005862/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_A_BASE=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v165"></a>
## V165 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_swave_reg768`

实测库 SHA256：`e3cec9b5456318067000412ea2bc1dc1465344b8f9ea55ee79b28c319b8e841d`；首次出现：`stage_bd_scalar_addressing`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_swave_reg768/20261013-133101-017834/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_swave_reg768/20261013-133101-017834/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_WAVE=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v166"></a>
## V166 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_abase_swave_reg768`

实测库 SHA256：`b893f7e3fccd9547f08bf20c6f1877528fc359bb6def496860a138a6f435f1ec`；首次出现：`stage_bd_scalar_addressing`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_abase_swave_reg768/20261013-133218-997870/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_abase_swave_reg768/20261013-133218-997870/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_A_BASE=1 -DH3_PHASE_SCALAR_WAVE=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v167"></a>
## V167 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_swave_lpad1_reg768`

实测库 SHA256：`d239b09878c14475cce35952cade2328b6b4989ffa9d98e37c8af4aff96e2269`；首次出现：`stage_be_scalar_wave_livepads`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_swave_lpad1_reg768/20261013-133958-292996/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_swave_lpad1_reg768/20261013-133958-292996/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_WAVE=1 -DH3_LIVE_PAD=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v168"></a>
## V168 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_swave_lpad2_reg768`

实测库 SHA256：`6516f0e3c1875781f1e5b5bf50dfd63635dd69a4a0ce13410ae5ee7276673cf6`；首次出现：`stage_be_scalar_wave_livepads`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_swave_lpad2_reg768/20261013-133958-293393/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_swave_lpad2_reg768/20261013-133958-293393/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_WAVE=1 -DH3_LIVE_PAD=2
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v169"></a>
## V169 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_swave_lpad3_reg768`

实测库 SHA256：`b89cb77e3a7cd9624089d45236eb39f42573258f5d12a2033a92ee4ef622f296`；首次出现：`stage_be_scalar_wave_livepads`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_swave_lpad3_reg768/20261013-134003-410721/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_swave_lpad3_reg768/20261013-134003-410721/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_WAVE=1 -DH3_LIVE_PAD=3
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v170"></a>
## V170 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_stagger_reg768`

实测库 SHA256：`c88791daae10e4cfdb4b1cd8200d37414efd04fc334bff85da001d0134c16363`；首次出现：`stage_bf_wave_group_barrier_stagger`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_stagger_reg768/20261013-135144-086590/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_stagger_reg768/20261013-135144-086590/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_STAGGER=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v171"></a>
## V171 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_stagger_postprio_reg768`

实测库 SHA256：`4ab55349c4cd62c8b74fef5fdd11b4b1ce1e28e560817c8f32d2a508a9514448`；首次出现：`stage_bf_wave_group_barrier_stagger`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_stagger_postprio_reg768/20261013-135144-087188/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_stagger_postprio_reg768/20261013-135144-087188/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_STAGGER=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v172"></a>
## V172 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_compat_reg768`；`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_reg768`

实测库 SHA256：`19453374cf0ad04f191c2407e744b79125cecc05b44836e0a209413db642039f`；首次出现：`stage_bg_explicit_pair_tiles`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_compat_reg768/20261013-142619-021216/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_compat_reg768/20261013-142619-021216/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v173"></a>
## V173 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_stagger_reg768`

实测库 SHA256：`2e383e93e03243c340d990f07201a35e832e2783e31f056dbbbdf297991ff75f`；首次出现：`stage_bg_explicit_pair_tiles`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_stagger_reg768/20261013-140643-578596/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_stagger_reg768/20261013-140643-578596/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v174"></a>
## V174 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_aburst_cc_reg768`；`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_aburst_reg768`

实测库 SHA256：`468e01a930873a7549cb8b44cbee30aadc264a7534ed2f7a1818bef7ee128505`；首次出现：`stage_bi_scalar_m0_burst`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_aburst_cc_reg768/20261013-142613-800840/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_aburst_cc_reg768/20261013-142613-800840/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_PAIR_TILES=1 -DH3_PHASE_A_BURST=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v175"></a>
## V175 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_aburst_abase_cc_reg768`；`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_aburst_abase_reg768`

实测库 SHA256：`c9d77ba5f956df4a805492d512a92439b4868385477cddb81135b2e1682597fe`；首次出现：`stage_bi_scalar_m0_burst`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_aburst_abase_cc_reg768/20261013-142613-801159/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_aburst_abase_cc_reg768/20261013-142613-801159/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_A_BASE=1 -DH3_PHASE_PAIR_TILES=1 -DH3_PHASE_A_BURST=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v176"></a>
## V176 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_g1_reg768`

实测库 SHA256：`79b8d81f2bbf8b5a6c1f37c7b8132c78fc14c49b115ebd256d3f4aeb3c66bd1b`；首次出现：`stage_bk_grid_and_token_groups`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_g1_reg768/20261013-145719-891456/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_g1_reg768/20261013-145719-891456/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_GROUP_M=1 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_PAIR_TILES=1 -DH3_PHASE_TOKEN_GROUP=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v177"></a>
## V177 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_g2_reg768`

实测库 SHA256：`dc2f70eaca7b0e00a7db3050e9c3c5cb1555b22fab124392b9e393eea32eb3bb`；首次出现：`stage_bk_grid_and_token_groups`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_g2_reg768/20261013-145719-891851/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_g2_reg768/20261013-145719-891851/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_GROUP_M=2 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_PAIR_TILES=1 -DH3_PHASE_TOKEN_GROUP=2
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v178"></a>
## V178 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_g4_reg768`

实测库 SHA256：`74fcaee2b503591275b2420af47e13612119969cb29ab07df38cd40d91cdfb81`；首次出现：`stage_bk_grid_and_token_groups`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_g4_reg768/20261013-145725-140867/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_g4_reg768/20261013-145725-140867/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_GROUP_M=4 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_PAIR_TILES=1 -DH3_PHASE_TOKEN_GROUP=4
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v179"></a>
## V179 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_g16_reg768`

实测库 SHA256：`214afe24ecc510f130244c94121d2bc2bd520630c1f7b83c6f8890c93923e646`；首次出现：`stage_bk_grid_and_token_groups`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_g16_reg768/20261013-145725-174867/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_g16_reg768/20261013-145725-174867/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_GROUP_M=16 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_PAIR_TILES=1 -DH3_PHASE_TOKEN_GROUP=16
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v180"></a>
## V180 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_grid2d_reg768`

实测库 SHA256：`77f3e8deab1a03b61ca741057fba6745636a7caa1b416370645c92f3ded82182`；首次出现：`stage_bk_grid_and_token_groups`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_grid2d_reg768/20261013-145730-316660/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_grid2d_reg768/20261013-145730-316660/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_PAIR_TILES=1 -DH3_PHASE_GRID_2D=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v181"></a>
## V181 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_grid2d_g1_reg768`

实测库 SHA256：`301fab9bdb45974efa100cdd81edd608e697a70f63ec54c20a5830931a86f1f5`；首次出现：`stage_bk_grid_and_token_groups`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_grid2d_g1_reg768/20261013-145730-341957/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_grid2d_g1_reg768/20261013-145730-341957/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_GROUP_M=1 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_PAIR_TILES=1 -DH3_PHASE_GRID_2D=1 -DH3_PHASE_TOKEN_GROUP=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v182"></a>
## V182 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_reg768`

实测库 SHA256：`f9ab9737b02e9b3ad4b64253e2e99ab7e33487898e401b5a0fc7811b74867c71`；首次出现：`stage_bl_static_wave_groups`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_reg768/20261013-150926-509972/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_reg768/20261013-150926-509972/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v183"></a>
## V183 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768`

实测库 SHA256：`b13a73b09afbc30fb75563ffa896981b0e260ab5478d40735708047257ae2c7f`；首次出现：`stage_bl_static_wave_groups`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_compat_reg768/20261013-151754-197883/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_compat_reg768/20261013-151754-197883/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v184"></a>
## V184 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_priority_reg768`

实测库 SHA256：`c8fae9add7a29976ed8ce46936cb7751c7c9c16cdfbe19b162ff41ce025f79ff`；首次出现：`stage_bl_static_wave_groups`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_priority_reg768/20261013-150932-146305/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_priority_reg768/20261013-150932-146305/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_PRIORITY=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v185"></a>
## V185 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_builtinlgkm_reg768`

实测库 SHA256：`f189a1ce084cdd55994b6eb64e1e79fa6fd2567adf648d4a046900ee49743871`；首次出现：`stage_bn_static_wave_lds_schedule`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_builtinlgkm_reg768/20261013-151742-448249/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_builtinlgkm_reg768/20261013-151742-448249/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_BUILTIN_LGKM=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v186"></a>
## V186 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_dsasm_reg768`

实测库 SHA256：`2b2a17772d75f7dfe03290eda7adb54168b9c390fc5af2727b33e0696655b9cb`；首次出现：`stage_bn_static_wave_lds_schedule`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_dsasm_reg768/20261013-151742-448689/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_dsasm_reg768/20261013-151742-448689/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_DS_ASM=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v187"></a>
## V187 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_dsprogress_reg768`

实测库 SHA256：`bc226130bd28a2707f27fdaaa1abac55a1b141a4c7794da4353caccf62fc5a1b`；首次出现：`stage_bn_static_wave_lds_schedule`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_dsprogress_reg768/20261013-151748-256140/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_dsprogress_reg768/20261013-151748-256140/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_DS_ASM=1 -DH3_PHASE_DS_PROGRESS=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v188"></a>
## V188 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid4_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768`

实测库 SHA256：`f0a3c3a23bc021a1522eb74049dc82590e9ea07e59e6a62b2d08bdb1d39e87e8`；首次出现：`stage_bn_static_wave_lds_schedule`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid4_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768/20261013-151748-279112/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid4_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768/20261013-151748-279112/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_MID_POINT=4 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v189"></a>
## V189 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid12_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768`

实测库 SHA256：`3d6851d6d9209e9109832d7ff0cf26a311693166b2af5e43ae42c24d04a2e1d2`；首次出现：`stage_bn_static_wave_lds_schedule`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid12_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768/20261013-151754-004392/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid12_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768/20261013-151754-004392/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_MID_POINT=12 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v190"></a>
## V190 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mchunk16_reg768`；`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`

实测库 SHA256：`22ef3280c50201d0f66966e4fdfe6af75741f5d6dc4a2c46704bb4d7acd73025`；首次出现：`stage_bo_static_wave_b_prefetch`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_compat_reg768/20261013-153417-740889/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_compat_reg768/20261013-153417-740889/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v191"></a>
## V191 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_pow2b_reg768`

实测库 SHA256：`1ff64c210e161e7c94480816cf7b997ff0f3d5f3fe72c9e9f2a36264d8b7f176`；首次出现：`stage_bo_static_wave_b_prefetch`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_pow2b_reg768/20261013-152614-608774/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_pow2b_reg768/20261013-152614-608774/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_POW2_B=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v192"></a>
## V192 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid6_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768`

实测库 SHA256：`defe2ede00a53604c419ddf9c0b07a932fddeb7f2e98eadc8319d9b49bab39c2`；首次出现：`stage_bo_static_wave_b_prefetch`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid6_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768/20261013-152620-494167/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid6_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768/20261013-152620-494167/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_MID_POINT=6 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v193"></a>
## V193 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_midgroup_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768`

实测库 SHA256：`716970fa242e6fc2a01b14d237e46dd58d81c664792942e796787912cea093f1`；首次出现：`stage_bo_static_wave_b_prefetch`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_midgroup_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768/20261013-152620-509609/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_midgroup_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_reg768/20261013-152620-509609/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=1 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v194"></a>
## V194 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_fpclassrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`

实测库 SHA256：`64fe24c1dfffadcd35280bcfebfe0eccdf96365f857a238970de94ec9edc3c49`；首次出现：`stage_bq_fpclass_rne_and_b_prefetch`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_fpclassrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768/20261013-153411-882112/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_fpclassrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768/20261013-153411-882112/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=3 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v195"></a>
## V195 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid4_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`

实测库 SHA256：`a673474faf0b45184f9c4e4ddde1698e499e3ed35c97cf4cf14a85f4b639eef2`；首次出现：`stage_bq_fpclass_rne_and_b_prefetch`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid4_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768/20261013-153411-882514/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid4_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768/20261013-153411-882514/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_MID_POINT=4 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v196"></a>
## V196 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid6_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768`

实测库 SHA256：`7685b226bb5527c61d560c2736d86bee8dc017de7c3f87db69f2febfd3be5d89`；首次出现：`stage_bq_fpclass_rne_and_b_prefetch`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid6_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768/20261013-153417-655726/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid6_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reg768/20261013-153417-655726/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_MID_POINT=6 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v197"></a>
## V197 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack1_aw32_reg768`

实测库 SHA256：`cfefd91ea1ed166d8979efd43e231ee362577835244fd9fc98f9e10387371650`；首次出现：`stage_bs_accumulator_subbanks`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack1_aw32_reg768/20261013-154102-676024/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack1_aw32_reg768/20261013-154102-676024/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_ACC_PACK=1 -DH3_PHASE_ACC_WIDTH=32 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v198"></a>
## V198 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack2_aw32_reg768`

实测库 SHA256：`f6b07e8baf003938aa09f773ca90b0361649cfab7ac40887c4373ab2ec0e9e83`；首次出现：`stage_bs_accumulator_subbanks`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack2_aw32_reg768/20261013-154102-676353/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack2_aw32_reg768/20261013-154102-676353/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_ACC_PACK=2 -DH3_PHASE_ACC_WIDTH=32 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v199"></a>
## V199 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack3_aw32_reg768`

实测库 SHA256：`548898cab795f10ad1faaaf907f4fa07ce48f6f46918c3dd48cfc727a811eb78`；首次出现：`stage_bs_accumulator_subbanks`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack3_aw32_reg768/20261013-154108-684828/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack3_aw32_reg768/20261013-154108-684828/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_ACC_PACK=3 -DH3_PHASE_ACC_WIDTH=32 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v200"></a>
## V200 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack2_aw16_reg768`

实测库 SHA256：`70204aa5e2d0784e34068fb2e699f211546e1945e531ab6abd51c3c06d72ded8`；首次出现：`stage_bs_accumulator_subbanks`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack2_aw16_reg768/20261013-154109-411786/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_accpack2_aw16_reg768/20261013-154109-411786/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_ACC_PACK=2 -DH3_PHASE_ACC_WIDTH=16 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v201"></a>
## V201 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_aburst_reg768`

实测库 SHA256：`4976eda54acba68390e25127682ed27de2879243cacbe1b9dfd682af9a7afe79`；首次出现：`stage_bt_producer_priority_balance`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_aburst_reg768/20261013-154738-724466/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_aburst_reg768/20261013-154738-724466/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1 -DH3_PHASE_A_BURST=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v202"></a>
## V202 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_abase_reg768`

实测库 SHA256：`079bc31dfbf406532451f6976509f7c0e8bce103e4fce042e4190a858a0014de`；首次出现：`stage_bt_producer_priority_balance`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_abase_reg768/20261013-154738-724975/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_abase_reg768/20261013-154738-724975/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_A_BASE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v203"></a>
## V203 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_cachea_reg768`

实测库 SHA256：`39c0989e3869bf121eca70b099633c2df41ed391d8ea2ee85487f30bbad7bb0a`；首次出现：`stage_bt_producer_priority_balance`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_cachea_reg768/20261013-154744-449916/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_cachea_reg768/20261013-154744-449916/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_CACHE_A=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v204"></a>
## V204 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pv2_reg768`

实测库 SHA256：`ce952da682e37e05b8e40df7606f52cd9b5238092a8e97aecd65a4e53ff4f61e`；首次出现：`stage_bt_producer_priority_balance`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pv2_reg768/20261013-154744-549046/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pv2_reg768/20261013-154744-549046/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_POSTPRIO_VALUE=2 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v205"></a>
## V205 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pv3_reg768`

实测库 SHA256：`bfadd52cf006aa6765bbbede07fc4f84930afe14878c293d3f66567bad396245`；首次出现：`stage_bt_producer_priority_balance`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pv3_reg768/20261013-154750-268664/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pv3_reg768/20261013-154750-268664/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_POSTPRIO_VALUE=3 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v206"></a>
## V206 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reserve1_reg768`

实测库 SHA256：`67cd7bad04e668dc7fe2ba152489306252d33576c1a24243706a7539bf507da9`；首次出现：`stage_bu_low_register_constraints`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reserve1_reg768/20261013-155226-467041/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reserve1_reg768/20261013-155226-467041/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_RESERVE_LOW=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v207"></a>
## V207 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reserve2_reg768`

实测库 SHA256：`31d758f44a34e5bd377ae1ae6e6cf16cfc13bfeac14e94b919c09ae8f71d4ce8`；首次出现：`stage_bu_low_register_constraints`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reserve2_reg768/20261013-155226-467397/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reserve2_reg768/20261013-155226-467397/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_RESERVE_LOW=2 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v208"></a>
## V208 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reserve3_reg768`

实测库 SHA256：`4a9d39ae536e2fc0e55e058ceac4deeff0be15d4cd18c2e634f88620cdcfc39d`；首次出现：`stage_bu_low_register_constraints`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reserve3_reg768/20261013-155232-219705/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_reserve3_reg768/20261013-155232-219705/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_RESERVE_LOW=3 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v209"></a>
## V209 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p0_reg768`

实测库 SHA256：`0031101d1bbf7d28dcc90b70f26b64cb52124c450ec0cdb5c078c09abe043cb9`；首次出现：`stage_bv_native_k64_activation_banks`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p0_reg768/20261013-160641-914670/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p0_reg768/20261013-160641-914670/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=0 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v210"></a>
## V210 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p1_reg768`

实测库 SHA256：`e7a82dd85a9ded8a138388898078a027b327cde885fa6204d371a7ff7d8832d5`；首次出现：`stage_bv_native_k64_activation_banks`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p1_reg768/20261013-160641-915185/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p1_reg768/20261013-160641-915185/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v211"></a>
## V211 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p0_late_reg768`

实测库 SHA256：`f66b8cc2396f676a02ccf3b5b599ec1bdd108800fc0d1fd9e41817085e8f332f`；首次出现：`stage_bw_native_k64_late_wait`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p0_late_reg768/20261013-161124-803979/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p0_late_reg768/20261013-161124-803979/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=0 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v212"></a>
## V212 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p1_late_reg768`

实测库 SHA256：`0d477afa11fee739b85626b91de76ce32dd6af8df0fa6a517caba96a3d179893`；首次出现：`stage_bw_native_k64_late_wait`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p1_late_reg768/20261013-161124-804211/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_nb64p1_late_reg768/20261013-161124-804211/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v213"></a>
## V213 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_fixednk_reg768`

实测库 SHA256：`5d8a5b0ba064ed72093e86b35ae4b2a45cfa7ab795737da2a56ee333f6daf553`；首次出现：`stage_bx_fixed_nk_specialization`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_fixednk_reg768/20261013-171929-776428/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_fixednk_reg768/20261013-171929-776428/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_FIXED_SHAPE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v214"></a>
## V214 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_fixedk_reg768`

实测库 SHA256：`cc35b0a903d6c6cd4cb1d1a5d0a0ee8d6e0ac99e470aad24f2f0a3ed60e74e14`；首次出现：`stage_bx_fixed_nk_specialization`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_fixedk_reg768/20261013-171929-776814/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_fixedk_reg768/20261013-171929-776814/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_FIXED_SHAPE=2 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v215"></a>
## V215 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_fixedn_reg768`

实测库 SHA256：`4156bcc7cc780a9ee2c55d9b0bc8df5351b2cd7b319cd5caff0ef9ade06fd643`；首次出现：`stage_bx_fixed_nk_specialization`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_fixedn_reg768/20261013-171939-239994/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_fixedn_reg768/20261013-171939-239994/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_FIXED_SHAPE=3 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v216"></a>
## V216 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_g1_reg768`

实测库 SHA256：`bae8f537f1bd9a6e908d618599b60f16d70c8840c5ed0675285db38e3d55365c`；首次出现：`stage_by_final_bbase_cta_traversal`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_g1_reg768/20261013-172636-506840/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_g1_reg768/20261013-172636-506840/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_GROUP_M=1 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1 -DH3_PHASE_TOKEN_GROUP=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v217"></a>
## V217 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_g2_reg768`

实测库 SHA256：`1da2fd8b8b3c7ec6ea53cd2b39a9ffbe76f91bb131f207943da652454f17a831`；首次出现：`stage_by_final_bbase_cta_traversal`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_g2_reg768/20261013-172636-507211/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_g2_reg768/20261013-172636-507211/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_GROUP_M=2 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1 -DH3_PHASE_TOKEN_GROUP=2
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v218"></a>
## V218 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_g4_reg768`

实测库 SHA256：`14b8966ac6e7e3adce9db66e733b14bfea4966277e4764456408a2f456379f28`；首次出现：`stage_by_final_bbase_cta_traversal`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_g4_reg768/20261013-172642-312065/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_g4_reg768/20261013-172642-312065/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_GROUP_M=4 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1 -DH3_PHASE_TOKEN_GROUP=4
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v219"></a>
## V219 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p0_late_reg768`

实测库 SHA256：`626833df45b0d02540fd8429223988a0ca6b61d8338b89e7de1765b7916201f9`；首次出现：`stage_bz_native_b64_addressing_and_mmac_window`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p0_late_reg768/20261013-182933-642776/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p0_late_reg768/20261013-182933-642776/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=0 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v220"></a>
## V220 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_reg768`

实测库 SHA256：`bcc5ccf7aba6b9c9177b7a84f6eef10f7ee9c60cef9d03a06197ef8dab1b3d3b`；首次出现：`stage_bz_native_b64_addressing_and_mmac_window`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_reg768/20261013-182933-643051/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_reg768/20261013-182933-643051/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v221"></a>
## V221 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p0_late_pow2b_reg768`

实测库 SHA256：`f560d5418ba3ce5fcf918af0c3ca96f28b5dbf7feaf6db1790dc6da0d663b3d2`；首次出现：`stage_bz_native_b64_addressing_and_mmac_window`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p0_late_pow2b_reg768/20261013-182939-408034/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p0_late_pow2b_reg768/20261013-182939-408034/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_POW2_B=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=0 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v222"></a>
## V222 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_pow2b_reg768`

实测库 SHA256：`5c725a2a97ae34f36cbd649e108962009449bd52803ef62a6d95a28f51bdc383`；首次出现：`stage_bz_native_b64_addressing_and_mmac_window`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_pow2b_reg768/20261013-182939-582804/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_pow2b_reg768/20261013-182939-582804/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_POW2_B=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v223"></a>
## V223 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mmacg4_reg768`

实测库 SHA256：`22691d1862dbf7ba805fe1d8ebed2935b374fd5a912164e69181d3c7705ce722`；首次出现：`stage_bz_native_b64_addressing_and_mmac_window`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mmacg4_reg768/20261013-182945-371217/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mmacg4_reg768/20261013-182945-371217/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_MMAC_GROUP4=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v224"></a>
## V224 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mmacg4_dsasm_reg768`

实测库 SHA256：`0149f3aaf39a7e5b50f8db6ee24ada721370073b8812253de877c22f8335e398`；首次出现：`stage_bz_native_b64_addressing_and_mmac_window`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mmacg4_dsasm_reg768/20261013-182945-379985/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mmacg4_dsasm_reg768/20261013-182945-379985/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_DS_ASM=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_MMAC_GROUP4=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v225"></a>
## V225 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_sf2_reg768`

实测库 SHA256：`98a4e191f5d454f63d0b8b02008fc0f6ff55fa8b8d5a54ca69f1898cf6aba261`；首次出现：`stage_ca_wave_group_prefetch_balance`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_sf2_reg768/20261013-183308-247818/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_sf2_reg768/20261013-183308-247818/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_STAGGER_FIRST_WAVE=2 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v226"></a>
## V226 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_sf6_reg768`

实测库 SHA256：`ce1f9a26cbdb731861eae4ce860631e08f77f233c2ced536dbd5f82494f92283`；首次出现：`stage_ca_wave_group_prefetch_balance`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_sf6_reg768/20261013-183308-248369/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_sf6_reg768/20261013-183308-248369/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_STAGGER_FIRST_WAVE=6 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v227"></a>
## V227 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_gp4x8_reg768`

实测库 SHA256：`f84d990441c0033154d741a6a05e91f06cbb7e591e78dcf1a386c622ab70c35f`；首次出现：`stage_ca_wave_group_prefetch_balance`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_gp4x8_reg768/20261013-183313-980626/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_gp4x8_reg768/20261013-183313-980626/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_GROUP_PREFETCH=1 -DH3_PHASE_EARLY_PREFETCH_POINT=4 -DH3_PHASE_LATE_PREFETCH_POINT=8 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v228"></a>
## V228 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_gp8x4_reg768`

实测库 SHA256：`728e7f4efc395bd9c3d3f9058156e0b455bce67fe9d38bd7bab5fd203aac4305`；首次出现：`stage_ca_wave_group_prefetch_balance`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_gp8x4_reg768/20261013-183314-121710/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_gp8x4_reg768/20261013-183314-121710/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_GROUP_PREFETCH=1 -DH3_PHASE_EARLY_PREFETCH_POINT=8 -DH3_PHASE_LATE_PREFETCH_POINT=4 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v229"></a>
## V229 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_gp8x12_reg768`

实测库 SHA256：`84f5d43d057d1cc9c4d24875cdea893ae51bcf573964b062ac05ceb1b90b8408`；首次出现：`stage_ca_wave_group_prefetch_balance`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_gp8x12_reg768/20261013-183319-697313/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_gp8x12_reg768/20261013-183319-697313/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_GROUP_PREFETCH=1 -DH3_PHASE_EARLY_PREFETCH_POINT=8 -DH3_PHASE_LATE_PREFETCH_POINT=12 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v230"></a>
## V230 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_gp12x8_reg768`

实测库 SHA256：`9a491b43d91523213754be7b32c26f37961269ded1e117b7e5e4b25e579e32ef`；首次出现：`stage_ca_wave_group_prefetch_balance`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_gp12x8_reg768/20261013-183320-089119/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_gp12x8_reg768/20261013-183320-089119/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_GROUP_PREFETCH=1 -DH3_PHASE_EARLY_PREFETCH_POINT=12 -DH3_PHASE_LATE_PREFETCH_POINT=8 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v231"></a>
## V231 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_np0_reg768`

实测库 SHA256：`725e3f078e99c8314e27e4882f5db4cac214e9389d9448b787e634d92b3ba9a1`；首次出现：`stage_cb_native_prefetch_between_math_phases`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_np0_reg768/20261013-183713-687691/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_np0_reg768/20261013-183713-687691/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_NATIVE_B64_PREFETCH_POINT=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v232"></a>
## V232 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_np4_reg768`

实测库 SHA256：`f71a84b89c35745f749005a730c4ef0db5162239b1c81ae07703f0b0f0e38a2a`；首次出现：`stage_cb_native_prefetch_between_math_phases`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_np4_reg768/20261013-183713-688088/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_np4_reg768/20261013-183713-688088/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_NATIVE_B64_PREFETCH_POINT=4 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v233"></a>
## V233 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_pow2b_np0_reg768`

实测库 SHA256：`da67350002082e867157edd605c51f9ba4db9c38fe651b6127a1c1dbc6d552c0`；首次出现：`stage_cb_native_prefetch_between_math_phases`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_pow2b_np0_reg768/20261013-183719-440803/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_pow2b_np0_reg768/20261013-183719-440803/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_NATIVE_B64_PREFETCH_POINT=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_POW2_B=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v234"></a>
## V234 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_pow2b_np4_reg768`

实测库 SHA256：`0c75143dff3f617538eac319df216718a3380c226cc493572b27c24eb39d2856`；首次出现：`stage_cb_native_prefetch_between_math_phases`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_pow2b_np4_reg768/20261013-183719-575045/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_pow2b_np4_reg768/20261013-183719-575045/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_NATIVE_B64_PREFETCH_POINT=4 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_POW2_B=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v235"></a>
## V235 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob1_reg768`

实测库 SHA256：`4cd15da65ffdd4ae13ea32487fa7c564b2215ab2cd727ee1940234593c953b06`；首次出现：`stage_cc_two_lds_bases_and_immediate_reads`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob1_reg768/20261013-184750-559520/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob1_reg768/20261013-184750-559520/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_A_TWO_BASE=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v236"></a>
## V236 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob1_areadwait_reg768`

实测库 SHA256：`b073c70d9e517f3c1a691b8fc246c27c6b2e08a6d84a49d2cad16e4c76bccdbf`；首次出现：`stage_cc_two_lds_bases_and_immediate_reads`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob1_areadwait_reg768/20261013-184750-559887/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob1_areadwait_reg768/20261013-184750-559887/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_A_TWO_BASE=1 -DH3_PHASE_A_READ_WAIT=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v237"></a>
## V237 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob2_reg768`；`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob3_reg768`

实测库 SHA256：`f747cae0af1b291c72d8aaa90f97d05ec21a3320792416c36989d062d1ee7700`；首次出现：`stage_cc_two_lds_bases_and_immediate_reads`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob2_reg768/20261013-184906-593990/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_atwob2_reg768/20261013-184906-593990/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_A_TWO_BASE=2 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v238"></a>
## V238 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_vmafter_reg768`

实测库 SHA256：`597afd4866860baca684a2620adf3951ded0d1177ee9c100fdb4391107960607`；首次出现：`stage_cd_b_wait_and_barrier_decoupling`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_vmafter_reg768/20261013-185324-556297/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_vmafter_reg768/20261013-185324-556297/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_VM_AFTER_BARRIER=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v239"></a>
## V239 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p0_endwait_reg768`

实测库 SHA256：`3f58d50a241b0d8321bfb831caa2321d2eaa9e47b4355d15dfea93d28425e492`；首次出现：`stage_cd_b_wait_and_barrier_decoupling`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p0_endwait_reg768/20261013-185324-556614/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p0_endwait_reg768/20261013-185324-556614/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_NATIVE_B64_END_WAIT=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=0 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v240"></a>
## V240 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_endwait_reg768`

实测库 SHA256：`99c3cb4b1fe1f0087e28b78a286447f35e18ae5b355bb46b6fbe98e53e0d4b2f`；首次出现：`stage_cd_b_wait_and_barrier_decoupling`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_endwait_reg768/20261013-185330-315699/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_endwait_reg768/20261013-185330-315699/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_NATIVE_B64_END_WAIT=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v241"></a>
## V241 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_endwait_pow2b_reg768`

实测库 SHA256：`8d3d307ce0acb21d1e7158152a7349a3120b6d39aa88ff46051a15e1073aef3f`；首次出现：`stage_cd_b_wait_and_barrier_decoupling`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_endwait_pow2b_reg768/20261013-185330-414863/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_endwait_pow2b_reg768/20261013-185330-414863/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_NATIVE_B64_END_WAIT=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_POW2_B=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v242"></a>
## V242 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mchunk4_reg768`

实测库 SHA256：`0a347ba514cac0cb15b5210a61de12b4fff393bfe06f0caf3df19657a2d70b41`；首次出现：`stage_ce_independent_mmac_tile_order`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mchunk4_reg768/20261013-185825-883937/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mchunk4_reg768/20261013-185825-883937/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MMAC_CHUNK=4 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v243"></a>
## V243 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mchunk8_reg768`

实测库 SHA256：`cb927af84e759c61d4ebb7320b8b1367bf17ee3da803e9e6c4f7348319d83b1b`；首次出现：`stage_ce_independent_mmac_tile_order`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mchunk8_reg768/20261013-185825-884469/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mchunk8_reg768/20261013-185825-884469/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MMAC_CHUNK=8 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v244"></a>
## V244 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mchunk8_atwob1_reg768`

实测库 SHA256：`b45492f9b06ee0a4a8ecba3ce16cb9130170c09e46fbb32ffbd94b2bed3da1e6`；首次出现：`stage_ce_independent_mmac_tile_order`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mchunk8_atwob1_reg768/20261013-185831-743928/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_mchunk8_atwob1_reg768/20261013-185831-743928/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_A_TWO_BASE=1 -DH3_PHASE_MMAC_CHUNK=8 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v245"></a>
## V245 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_aalways1_reg768`

实测库 SHA256：`508d4b6bc16291d625efd04b7fa22632590fefc13118900dc99015a706061d1a`；首次出现：`stage_cf_producer_terminal_branch`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_aalways1_reg768/20261013-190136-800332/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_aalways1_reg768/20261013-190136-800332/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_A_ALWAYS_PREFETCH=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v246"></a>
## V246 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_aalways2_reg768`

实测库 SHA256：`88f579ad390a226c10ddbf2e990f97394aba35dba7c6f739682e07f44b906c18`；首次出现：`stage_cf_producer_terminal_branch`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_aalways2_reg768/20261013-190136-800716/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_aalways2_reg768/20261013-190136-800716/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_A_ALWAYS_PREFETCH=2 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v247"></a>
## V247 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair1_reg768`；`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair2_reg768`

实测库 SHA256：`03025bda0a3fab1618265c05a1ec6ebdbb47f27193470e5d14c9e5ba1d516b53`；首次出现：`stage_cg_producer_fixed_slot_pairs`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair1_reg768/20261013-191139-822010/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair1_reg768/20261013-191139-822010/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_PRODUCER_PAIRS=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v248"></a>
## V248 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_asoff_reg768`

实测库 SHA256：`8d0f77fda874bb849b59c624794fd8523a6f201473f624bf9b586db3c0bfbd4e`；首次出现：`stage_ch_producer_scalar_offset`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_asoff_reg768/20261013-191711-688474/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_asoff_reg768/20261013-191711-688474/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_A_SOFFSET=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v249"></a>
## V249 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair1_asoff_reg768`

实测库 SHA256：`e04cfd5e83d2026cf5613f53effd11e392c2ffdf99217a0be47083a85852b78b`；首次出现：`stage_ch_producer_scalar_offset`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair1_asoff_reg768/20261013-191716-243869/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair1_asoff_reg768/20261013-191716-243869/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_PRODUCER_PAIRS=1 -DH3_PHASE_A_SOFFSET=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v250"></a>
## V250 — shape_dispatch.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768`

实测库 SHA256：`d2e34b972e3961fb99b2ea05802fc458561788dbb9587dc74a52fb96b3a5361f`；首次出现：`stage_cj_two_projection_producer_dispatch`。

当前入口：[shape_dispatch.cpp](<../shape_dispatch.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768/20261013-192744-072161/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_shapedispatch_reg768/20261013-192744-072161/shape_dispatch.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v251"></a>
## V251 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_reg768`

实测库 SHA256：`981b3ca7d6c6f547f1afd6cd428ec2a384bc8a6107e3b11647f3d0f24f868f1d`；首次出现：`stage_cl_native_b64_single_rebase`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_compat_reg768/20261013-200504-232810/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_compat_reg768/20261013-200504-232810/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_NATIVE_B64_BASE64=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v252"></a>
## V252 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_endwait_base64_reg768`

实测库 SHA256：`b00ba36b06d70b2a292200385275e3ecee7d991cc8faada1e4d0c02f7e6cae13`；首次出现：`stage_cl_native_b64_single_rebase`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_endwait_base64_reg768/20261013-195002-039688/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_endwait_base64_reg768/20261013-195002-039688/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_NATIVE_B64_END_WAIT=1 -DH3_PHASE_NATIVE_B64_BASE64=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v253"></a>
## V253 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p0_late_base64_reg768`

实测库 SHA256：`ec29d4d8771242b97b315ceb9483dc798133c592747a7ad6255f51e3d69cecaa`；首次出现：`stage_cm_native_b64_rebase_schedule`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p0_late_base64_reg768/20261013-195549-267708/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p0_late_base64_reg768/20261013-195549-267708/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_NATIVE_B64_BASE64=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=0 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v254"></a>
## V254 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_np4_reg768`

实测库 SHA256：`8c2121955f83b59e4def32b94ffdfb2320fc784b626d6b193a1873067b6b1b34`；首次出现：`stage_cm_native_b64_rebase_schedule`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_np4_reg768/20261013-195549-268101/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_np4_reg768/20261013-195549-268101/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_NATIVE_B64_BASE64=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_NATIVE_B64_PREFETCH_POINT=4 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v255"></a>
## V255 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_np12_reg768`

实测库 SHA256：`32c7f22c4bd33c6ba26a91640fc0174a2b471cb775befa991ff7255cc66d0edf`；首次出现：`stage_cm_native_b64_rebase_schedule`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_np12_reg768/20261013-195555-074458/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_np12_reg768/20261013-195555-074458/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_NATIVE_B64_BASE64=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_NATIVE_B64_PREFETCH_POINT=12 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v256"></a>
## V256 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_reg768`

实测库 SHA256：`45163df4033968743c4239a1531f5edbdd8682e95a179a85f918c8677fbd0c36`；首次出现：`stage_cm_native_b64_rebase_schedule`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_compat_reg768/20261013-200504-233340/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_compat_reg768/20261013-200504-233340/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_A_TWO_BASE=1 -DH3_PHASE_NATIVE_B64_BASE64=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v257"></a>
## V257 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_bhoist_reg768`

实测库 SHA256：`2824f87b5e3d8737acfda5acff41e9348a427fb463ca5e0e9720e33c0b706c6e`；首次出现：`stage_cn_native_b64_shared_startup`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_bhoist_reg768/20261013-200114-792557/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_bhoist_reg768/20261013-200114-792557/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_NATIVE_B64_BASE64=1 -DH3_PHASE_NATIVE_B64_HOIST=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v258"></a>
## V258 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_bhoist_reg768`

实测库 SHA256：`b26e16bf8ae47bfe85dbd7f8a5a5ed57cf2edeb5577d55a42b0c97a8d0e193a6`；首次出现：`stage_cn_native_b64_shared_startup`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_bhoist_reg768/20261013-200114-793097/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_bhoist_reg768/20261013-200114-793097/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_A_TWO_BASE=1 -DH3_PHASE_NATIVE_B64_BASE64=1 -DH3_PHASE_NATIVE_B64_HOIST=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v259"></a>
## V259 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_binit_reg768`

实测库 SHA256：`b5fc85b151edbc067d4e1b5917e9b78f62477af0fb4bfbea3bbedff0893c1411`；首次出现：`stage_co_native_b64_immutable_initial`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_binit_reg768/20261013-201052-464677/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_binit_reg768/20261013-201052-464677/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_NATIVE_B64_BASE64=1 -DH3_PHASE_NATIVE_B64_HOIST=2 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v260"></a>
## V260 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_binit_reg768`

实测库 SHA256：`a63b4481bd755bc726515b780003384aa0f29c01ae30c6fd4a154add2e3d8ea3`；首次出现：`stage_co_native_b64_immutable_initial`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_binit_reg768/20261013-201052-465046/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_binit_reg768/20261013-201052-465046/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_A_TWO_BASE=1 -DH3_PHASE_NATIVE_B64_BASE64=1 -DH3_PHASE_NATIVE_B64_HOIST=2 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v261"></a>
## V261 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_pwait_reg768`

实测库 SHA256：`0ecb2a8d0e48d76c592103f88463768cc45427d54359701ccacce955e0dd10e4`；首次出现：`stage_cp_native_b64_phase_waits`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_pwait_reg768/20261013-202243-688553/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_pwait_reg768/20261013-202243-688553/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_NATIVE_B64_BASE64=1 -DH3_PHASE_NATIVE_B64_PHASE_WAIT=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v262"></a>
## V262 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_pwait_reg768`

实测库 SHA256：`abc5d329e21693b49341b35fcafe8b04969cdde363821039072b0bcfc3e532fc`；首次出现：`stage_cp_native_b64_phase_waits`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_pwait_compat_reg768/20261013-225947-818684/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_pwait_compat_reg768/20261013-225947-818684/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_A_TWO_BASE=1 -DH3_PHASE_NATIVE_B64_BASE64=1 -DH3_PHASE_NATIVE_B64_PHASE_WAIT=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v263"></a>
## V263 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pairout1_reg768`

实测库 SHA256：`ca25a3f1ac5a4fc48c70cc1c6885347ae67946a35e652e6882d592c5cc203b31`；首次出现：`stage_cq_paired_feature_output_builtin`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pairout1_reg768/20261013-211700-085318/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pairout1_reg768/20261013-211700-085318/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_PAIRED_OUTPUT=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v264"></a>
## V264 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pairout2_reg768`

实测库 SHA256：`543659fbde3e6f1d26a15baee27dada0006e43b6ad389538b3290cb7fd0df425`；首次出现：`stage_cq_paired_feature_output_builtin`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pairout2_reg768/20261013-210800-171415/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pairout2_reg768/20261013-210800-171415/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_PAIRED_OUTPUT=2 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v265"></a>
## V265 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pairout3_reg768`

实测库 SHA256：`e48b1ff300c239becaff54fa6bfa8f8653154c055400e5016463fca2b65d0744`；首次出现：`stage_cr_paired_output_alignment_hoist`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pairout3_reg768/20261013-212534-083851/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pairout3_reg768/20261013-212534-083851/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_PAIRED_OUTPUT=3 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v266"></a>
## V266 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pairout4_reg768`

实测库 SHA256：`2aca17b7811aef6462af9fcc2a31b62cf06bc6f1e72c8a19539f14413c0ebd92`；首次出现：`stage_cr_paired_output_alignment_hoist`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pairout4_reg768/20261013-212534-084215/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_pairout4_reg768/20261013-212534-084215/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_PAIRED_OUTPUT=4 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v267"></a>
## V267 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid4_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_reg768`

实测库 SHA256：`944bf3975d8fc0b83a851836ecfdf9826f8557207c573844e655ca2db13be587`；首次出现：`stage_cs_half_feature_tile`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid4_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_reg768/20261013-214200-477311/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid4_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_reg768/20261013-214200-477311/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_FEATURE_TILE=128 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_MID_POINT=4 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v268"></a>
## V268 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid6_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_reg768`

实测库 SHA256：`4f2da55822ff12d9cb0f85c0878a41167fbf485692118c1a0747b2a5e57d98ea`；首次出现：`stage_cs_half_feature_tile`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid6_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_reg768/20261013-214200-477638/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid6_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_reg768/20261013-214200-477638/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_FEATURE_TILE=128 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_MID_POINT=6 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v269"></a>
## V269 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid4_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_cw4_pw2_sf2_reg768`

实测库 SHA256：`f602d13b67ce5f1411cdc61459b8e12027887ac520db21c2200ae7bf820a97b2`；首次出现：`stage_ct_half_tile_six_waves`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid4_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_cw4_pw2_sf2_reg768/20261013-220115-774132/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid4_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_cw4_pw2_sf2_reg768/20261013-220115-774132/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=64 -DH3_WM=6 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_FEATURE_TILE=128 -DH3_PHASE_COMPUTE_WAVES=4 -DH3_PHASE_PRODUCER_WAVES=2 -DH3_PHASE_STAGGER_FIRST_WAVE=2 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_MID_POINT=4 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v270"></a>
## V270 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid6_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_cw4_pw2_sf2_reg768`

实测库 SHA256：`85aa076fc9a7b33dac2580ccefc5a450581f28ef4cedb4f18111bdc6bc5c1785`；首次出现：`stage_ct_half_tile_six_waves`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid6_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_cw4_pw2_sf2_reg768/20261013-220115-774624/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid6_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_f128_cw4_pw2_sf2_reg768/20261013-220115-774624/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=128 -DH3_BN=256 -DH3_BK=64 -DH3_WM=6 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_FEATURE_TILE=128 -DH3_PHASE_COMPUTE_WAVES=4 -DH3_PHASE_PRODUCER_WAVES=2 -DH3_PHASE_STAGGER_FIRST_WAVE=2 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_MID_POINT=6 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v271"></a>
## V271 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_cw8_pw2_reg768`

实测库 SHA256：`7bc386db6720cb4411ffaf4c74e471b60a22dd5d7c41a2f0770af7d3e3edb549`；首次出现：`stage_cu_original_tile_two_producers`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_cw8_pw2_reg768/20261013-220709-877734/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_cw8_pw2_reg768/20261013-220709-877734/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=10 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_COMPUTE_WAVES=8 -DH3_PHASE_PRODUCER_WAVES=2 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v272"></a>
## V272 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair1_asoff_cw8_pw2_reg768`

实测库 SHA256：`b9616ab1424ba7e25ab9d5fe4697928b1907c9edbb24b771efcad7fb51745feb`；首次出现：`stage_cu_original_tile_two_producers`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair1_asoff_cw8_pw2_reg768/20261013-220709-878051/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_ppair1_asoff_cw8_pw2_reg768/20261013-220709-878051/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=10 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_PRODUCER_PAIRS=1 -DH3_PHASE_A_SOFFSET=1 -DH3_PHASE_COMPUTE_WAVES=8 -DH3_PHASE_PRODUCER_WAVES=2 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v273"></a>
## V273 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bsoff_reg768`

实测库 SHA256：`53720dc9e85de174acf75a9d595268f6877b330eff0e5b54706e64c3e394bc53`；首次出现：`stage_cv_activation_scalar_offset`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bsoff_reg768/20261013-221705-429027/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bsoff_reg768/20261013-221705-429027/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_B_SOFFSET=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v274"></a>
## V274 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bsoff_ppair1_asoff_reg768`

实测库 SHA256：`6a239d2be000aad5f1ecae11bf1174ddcf75bd1f76bb530f010a57e018f0dce3`；首次出现：`stage_cv_activation_scalar_offset`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bsoff_ppair1_asoff_reg768/20261013-221709-910750/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bsoff_ppair1_asoff_reg768/20261013-221709-910750/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_PRODUCER_PAIRS=1 -DH3_PHASE_A_SOFFSET=1 -DH3_PHASE_B_SOFFSET=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v275"></a>
## V275 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream1_reg768`

实测库 SHA256：`809be1c4e3661258e920c0b7cf317db05bd39f7c9ff54b0b913b30b06ce707f7`；首次出现：`stage_cw_phase1_a_streaming`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream1_reg768/20261013-222925-495813/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream1_reg768/20261013-222925-495813/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_A_PHASE1_PREFETCH=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v276"></a>
## V276 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream4_reg768`

实测库 SHA256：`ad15a0380ffb3a6cb6b1392e316723bd4f79a07dfa0fddf34f635d778e553ed3`；首次出现：`stage_cw_phase1_a_streaming`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream4_compat_reg768/20261013-224604-645839/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream4_compat_reg768/20261013-224604-645839/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_A_PHASE1_PREFETCH=4 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v277"></a>
## V277 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream4_win0x4_reg768`

实测库 SHA256：`8b61255c124bd154f1273ae0fc3c5b8e21b3875693a781de4a01a103297226c2`；首次出现：`stage_cx_partial_a_phase1_windows`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream4_win0x4_reg768/20261013-224552-760762/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream4_win0x4_reg768/20261013-224552-760762/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_A_PHASE1_PREFETCH=4 -DH3_PHASE_A_PHASE1_BEGIN=0 -DH3_PHASE_A_PHASE1_COUNT=4 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v278"></a>
## V278 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream4_win0x8_reg768`

实测库 SHA256：`5b99888b4a5e842dde24bf7ebc2267805b9f204417e0d1019b7290d2961129ee`；首次出现：`stage_cx_partial_a_phase1_windows`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream4_win0x8_reg768/20261013-224552-761081/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream4_win0x8_reg768/20261013-224552-761081/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_A_PHASE1_PREFETCH=4 -DH3_PHASE_A_PHASE1_BEGIN=0 -DH3_PHASE_A_PHASE1_COUNT=8 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v279"></a>
## V279 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream4_win12x4_reg768`

实测库 SHA256：`c8e48be5a33c89a8cacb8105f716df85b38b4aa44cbfed3c554fa52023dd3e44`；首次出现：`stage_cx_partial_a_phase1_windows`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream4_win12x4_reg768/20261013-224558-575747/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream4_win12x4_reg768/20261013-224558-575747/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_A_PHASE1_PREFETCH=4 -DH3_PHASE_A_PHASE1_BEGIN=12 -DH3_PHASE_A_PHASE1_COUNT=4 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v280"></a>
## V280 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream4_win8x8_reg768`

实测库 SHA256：`7b06394a864a0d345b82a64ed3e48d524ca592820c8532767ed7747fe0cc0e44`；首次出现：`stage_cx_partial_a_phase1_windows`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream4_win8x8_reg768/20261013-224558-728526/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_a1stream4_win8x8_reg768/20261013-224558-728526/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_A_PHASE1_PREFETCH=4 -DH3_PHASE_A_PHASE1_BEGIN=8 -DH3_PHASE_A_PHASE1_COUNT=8 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v281"></a>
## V281 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_brow1_reg768`

实测库 SHA256：`2c35130ae9be2ffce3c372785b67a41e4f3b535072666b7432fd589c45a01edd`；首次出现：`stage_cy_k64_row_order_immediate`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_brow1_reg768/20261013-225931-523540/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_brow1_reg768/20261013-225931-523540/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_A_TWO_BASE=1 -DH3_PHASE_NATIVE_B64_BASE64=1 -DH3_PHASE_NATIVE_B64_ROW_ORDER=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v282"></a>
## V282 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_brow2_reg768`

实测库 SHA256：`7c2663ad82c2a5e32d6fc3a3270894748e9d1c0238872cbb089865edc35053fc`；首次出现：`stage_cy_k64_row_order_immediate`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_brow2_compat_reg768/20261014-102851-446883/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_brow2_compat_reg768/20261014-102851-446883/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_A_TWO_BASE=1 -DH3_PHASE_NATIVE_B64_BASE64=1 -DH3_PHASE_NATIVE_B64_ROW_ORDER=2 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v283"></a>
## V283 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_pwait_brow2_reg768`

实测库 SHA256：`4e720d47ec309ef21e77e469935873929d7f9196d8bb2e59e3195d947046c1a6`；首次出现：`stage_cy_k64_row_order_immediate`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_pwait_brow2_reg768/20261013-225937-145731/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_pwait_brow2_reg768/20261013-225937-145731/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_A_TWO_BASE=1 -DH3_PHASE_NATIVE_B64_BASE64=1 -DH3_PHASE_NATIVE_B64_PHASE_WAIT=1 -DH3_PHASE_NATIVE_B64_ROW_ORDER=2 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v284"></a>
## V284 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_brow2_sb64c0_reg768`

实测库 SHA256：`19c162cdc7d73b55610732daf349ac0876ac9ae4cf5b423e276e7fafafbdd25a`；首次出现：`stage_cz_struct64_bit62_20261014`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_brow2_sb64c0_reg768/20261014-102835-708818/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_brow2_sb64c0_reg768/20261014-102835-708818/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_A_TWO_BASE=1 -DH3_PHASE_NATIVE_B64_BASE64=1 -DH3_PHASE_NATIVE_B64_ROW_ORDER=2 -DH3_PHASE_STRUCT_B64=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v285"></a>
## V285 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_brow2_sb64c1_reg768`

实测库 SHA256：`322f0ac4497f0b53f0d88eb0c0880aeb1ab68f8a8a7ab1ea741b6a24923b9210`；首次出现：`stage_cz_struct64_bit62_20261014`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_brow2_sb64c1_reg768/20261014-102840-166331/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_brow2_sb64c1_reg768/20261014-102840-166331/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_A_TWO_BASE=1 -DH3_PHASE_NATIVE_B64_BASE64=1 -DH3_PHASE_NATIVE_B64_ROW_ORDER=2 -DH3_PHASE_STRUCT_B64=2 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v286"></a>
## V286 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_pwait_brow2_sb64c0_reg768`

实测库 SHA256：`0ec437c4a4263709b752fede76b5a229ea377ac334366f5e3ca38f574ca58fdb`；首次出现：`stage_cz_struct64_bit62_20261014`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_pwait_brow2_sb64c0_reg768/20261014-102841-484588/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_pwait_brow2_sb64c0_reg768/20261014-102841-484588/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_A_TWO_BASE=1 -DH3_PHASE_NATIVE_B64_BASE64=1 -DH3_PHASE_NATIVE_B64_PHASE_WAIT=1 -DH3_PHASE_NATIVE_B64_ROW_ORDER=2 -DH3_PHASE_STRUCT_B64=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v287"></a>
## V287 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_pwait_brow2_sb64c1_reg768`

实测库 SHA256：`e3afdc6923d1c3340c36467aec5aa875a4a1ac21b916011ca3abf9f4fc809f5f`；首次出现：`stage_cz_struct64_bit62_20261014`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_pwait_brow2_sb64c1_reg768/20261014-102845-801989/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_nb64p1_late_base64_atwob1_pwait_brow2_sb64c1_reg768/20261014-102845-801989/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_A_TWO_BASE=1 -DH3_PHASE_NATIVE_B64_BASE64=1 -DH3_PHASE_NATIVE_B64_PHASE_WAIT=1 -DH3_PHASE_NATIVE_B64_ROW_ORDER=2 -DH3_PHASE_STRUCT_B64=2 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_NATIVE_B64=1 -DH3_PHASE_NATIVE_B64_POINT=1 -DH3_PHASE_NATIVE_B64_LATE_WAIT=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v288"></a>
## V288 — cache_dispatch.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_cachedispatch_reg768`

实测库 SHA256：`de1cd4cc831fae0153f5cb0ef8dee163582501db7d83e776e192a33de05a5484`；首次出现：`stage_db_cache_shape_dispatch_20261014`。

当前入口：[cache_dispatch.cpp](<../cache_dispatch.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_cachedispatch_compat_reg768/20261014-111525-651730/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_bbase_cachedispatch_compat_reg768/20261014-111525-651730/cache_dispatch.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_SCALAR_B_BASE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v289"></a>
## V289 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32c0_reg768`

实测库 SHA256：`07c47327f8bc875209c480bb36995c08454e46f36579159f35d7aac4efeff20a`；首次出现：`stage_dd_struct32_cache_pair_20261014`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32c0_reg768/20261014-111509-206156/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32c0_reg768/20261014-111509-206156/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_STRUCT_B32=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v290"></a>
## V290 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32c1_reg768`

实测库 SHA256：`62705257603baf0681e580bd556277e486131fc4b02253640d82318c06c4188a`；首次出现：`stage_dd_struct32_cache_pair_20261014`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32c1_reg768/20261014-111513-842109/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32c1_reg768/20261014-111513-842109/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_STRUCT_B32=2 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v291"></a>
## V291 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32c0_ppair1_asoff_reg768`

实测库 SHA256：`0a888e2698574fa9eea248d09a46d9f8cc310f390f4e95a06a40493f5e963643`；首次出现：`stage_dd_struct32_cache_pair_20261014`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32c0_ppair1_asoff_reg768/20261014-111515-053636/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32c0_ppair1_asoff_reg768/20261014-111515-053636/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_PRODUCER_PAIRS=1 -DH3_PHASE_A_SOFFSET=1 -DH3_PHASE_STRUCT_B32=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v292"></a>
## V292 — rolephase.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32c1_ppair1_asoff_reg768`

实测库 SHA256：`6c4bbf59a88321dd7658409b397a6ba7d919fc49d6643fa9af584a3fdc0a4c01`；首次出现：`stage_dd_struct32_cache_pair_20261014`。

当前入口：[rolephase.cpp](<../rolephase.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32c1_ppair1_asoff_reg768/20261014-111519-767919/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32c1_ppair1_asoff_reg768/20261014-111519-767919/rolephase.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_PRODUCER_PAIRS=1 -DH3_PHASE_A_SOFFSET=1 -DH3_PHASE_STRUCT_B32=2 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。

<a id="v293"></a>
## V293 — struct32_dispatch.cpp

候选名：`rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32dispatch_reg768`

实测库 SHA256：`cbd4638fde724142b41314e7d2578e79fbbdc0a6bcd43d6e154716e768f5f6b9`；首次出现：`stage_df_struct32_dispatch_20261014`。

当前入口：[struct32_dispatch.cpp](<../struct32_dispatch.cpp>)；构建配置：[build.py](<../build.py>)。

匹配构建记录：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32dispatch_reg768/20261014-112335-003400/build.json`。

冻结源码（已校验）：`/root/workspace/yyc/composable_kernel/example_hcu/ck_tile/04_gemm/build/attempts/rolephase_m1_bank_asmb_always_mid_bufferepi_maskrne_nativecadence_pairs_staticstagger_postprio_sb32dispatch_reg768/20261014-112335-003400/struct32_dispatch.cpp`。同目录保留 `build.json`、`build.log` 和构建时私有头文件。

编译宏（以匹配构建记录为准）：

```text
-DH3_BM=256 -DH3_BN=256 -DH3_BK=64 -DH3_WM=12 -DH3_WN=1 -DH3_SWIZZLE=0 -DH3_PAD_M=1 -DH3_PHASE_M_WAVES=1 -DH3_PHASE_BANK=1 -DH3_PHASE_ASM_B=1 -DH3_PHASE_POSTPRIO=1 -DH3_PHASE_MID_PREFETCH=1 -DH3_PHASE_MID_GROUP=0 -DH3_PHASE_ALWAYS_PREFETCH=1 -DH3_PHASE_BUFFER_EPILOGUE=1 -DH3_PHASE_OUTPUT_RNE=1 -DH3_PHASE_NATIVE_CADENCE=1 -DH3_PHASE_STATIC_STAGGER=1 -DH3_PHASE_PAIR_TILES=1
```

附加编译参数：`--enable-num-vgprs-768=true`。
