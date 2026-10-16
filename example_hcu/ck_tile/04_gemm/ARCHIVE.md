# 归档、旧路径与恢复

此次仅整理 `example_hcu/ck_tile/04_gemm`。不覆盖目标主线的 `03_gemm`、共享 include 或其它例子；不重写上游历史。

## 目录

```text
04_gemm/
  README.md                  当前入口和版本导航
  build.py / bench.py        精简的统一构建/测试入口
  verify_archive.py          CPU 校验全部报告和保留源码 SHA
  VERSIONS.json              7 个版本的结构化索引
  versions/v01…v08/          7 份历史快照 + 后续分层重构 v08
  records/                   原树的全部文档、结构化结果、日志与文本 counter
  build/                     本地重建产物，新测量输出；Git 忽略
```

## 全部报告如何保留

所有原 `.md/.json/.csv/.txt/.xml/.log/.s/.ISA/.ll` 文件原字节保留。大于 1,000,000 字节的文本使用 gzip 无损压缩，避免数 GB 的编译诊断/文本堆在 Git 中；文件名追加 `.gz`。每个原路径、原 SHA256、大小和新存储路径均在 `records/PRESERVATION_MANIFEST.json`，`python3 verify_archive.py` 会解压校验。

历史报告内的 `build/attempts/...`、源码行号与旧绝对路径属于当时的来源记录，不改写历史。当前可读代码请通过 `versions/*/README.md` 和 `manifest.json` 找到；未保留的候选源码以及 `.out/.perf/.so` 等二进制证据，可从完整备份恢复。早期 26 个缺少独立冻结源码、一个旧库同名文件已被后续构建覆盖等已知限制仍保留在原索引中。

## 服务器完整备份

整理前完整目录：

`/root/workspace/yyc/ck_gemm_archives/04_gemm_pre_cleanup_20261016.tar`

逐文件清单（原 9,577 个文件）：

`/root/workspace/yyc/ck_gemm_archives/04_gemm_pre_cleanup_20261016.manifest.json`

原目录树整理后移到同一备份目录的 `04_gemm_original_tree_20261016`，可直接查找大 profiler 文件。备份均在 Git 工作区之外，不上传 GitHub。恢复时先解到新的空目录，不覆盖当前工作区：

```bash
mkdir /tmp/ck_gemm_restore_20261016
tar -xf /root/workspace/yyc/ck_gemm_archives/04_gemm_pre_cleanup_20261016.tar -C /tmp/ck_gemm_restore_20261016
```

7 个版本的 commit 是本次按历史顺序新增的归档提交，不冒充当年的开发提交，不更改原始性能记录。保留来源 SHA256，源码字节不做清理/重排以免改变重建依据。

归档之后新增的 v08 是新实现，不是原实验的第八份冻结快照。它的源码、正确性/性能报告及新证据放在 `versions/v08_ck_tile/`；原始 3,402 份报告的 preservation manifest 保持原样。v08 通过 Git 及独立交付包保存，不在整理前的完整备份 tar 中。
