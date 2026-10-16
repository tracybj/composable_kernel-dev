"""CPU-only final report. Screens select a candidate; three new processes validate."""
from pathlib import Path
import json,statistics
root=Path(__file__).resolve().parent
scalar=json.loads((root/'analysis.json').read_text())
vector=json.loads((root/'vec8/analysis.json').read_text())
selection=json.loads((root/'vec8/selection.json').read_text())
assert selection['selected_comm']==8
result={'scalar_screening':scalar,'vector_experiment':vector,'selection':selection,'formal_summary':[]}
modes=['v08_rccl','serial80','phase_roles','pipeline_roles','compute_only']
for k,label in ((7168,'FC2'),(3584,'Attention out_proj')):
    row={'label':label,'m':18880,'n':5376,'k':k,'compute_ctas':72,'communication_ctas':8,'timings':{}}
    for mode in modes:
        vals=[x for x in vector['timings'] if x['file'] in ('formal_a.log','formal_b.log','formal_c.log') and x['k']==k and x['mode']==mode]
        assert len(vals)==3 and all(v['n']==15 for v in vals)
        row['timings'][mode]={'mean_of_process_medians_ms':statistics.mean(v['median_ms'] for v in vals),'runs':vals}
    ts={k:v['mean_of_process_medians_ms'] for k,v in row['timings'].items()}
    row['pipeline_time_reduction_vs_rccl_percent']=(1-ts['pipeline_roles']/ts['v08_rccl'])*100
    row['pipeline_time_reduction_vs_phase_percent']=(1-ts['pipeline_roles']/ts['phase_roles'])*100
    row['serial80_time_reduction_vs_rccl_percent']=(1-ts['serial80']/ts['v08_rccl'])*100
    result['formal_summary'].append(row)
(root/'comparison.json').write_text(json.dumps(result,indent=2)+'\n')
lines=['# v08 CK-Tile 单 kernel 计算/通信角色流水实验','',
'日期：2026-10-16。独立双卡算子实验，不启动 H3，不改模型或共享 CK，不 commit/push。','',
'## 结论','',
'已实现并通过时间戳验证：同一个 kernel 内计算 CTA 发布 tile 后继续计算，通信 CTA 独立读取对端并归约。标量通信初版虽然形成交叠，但通信吞吐不足，远慢于基线；通信读写改为每 lane 8 个 BF16 的向量访问后，在 FC2 上获得净收益。',
'三次独立正式复测中，选定的 72 compute + 8 communication CTA 配置：FC2 约 6.8303→6.0467 ms，时间下降约 11.47%；同配置 out_proj 约 4.6499→5.4155 ms，反而变慢约 16.46%。不能将 FC2 最优配比视为 out_proj 最优。','',
'## 硬件、精度与范围','',
'- root@10.212.14.166，ck_yyc，物理 HCU4/5、gfx936，模拟一个 TP2 组；无 SP All-to-All、无模型、无其它 TP 组并发。',
'- M=18880、N=5376；FC2 K=7168，out_proj K=3584。BF16 X/W、FP32 累加、BF16 局部输出；双卡局部输出按 BF16 Sum/RNE 归约。',
'- 沿用 v08 的 256×256×64 tile、8 compute wave + 4 weight-loader wave。GEMM 内部流水源码未改；新的计算/通信角色是 CTA 级划分，不是把权重搬运 wave 直接改成通信 wave。',
'- 同一角色 kernel 所有 CTA 都声明 64 KiB LDS、768 threads，包括通信 CTA。合作启动查询容量均为 80；不是显式 CU 绑定，不声称通信 CTA 不占 LDS。',
'- full partial output 每 tile 有独立位置；没有循环覆盖槽位。produced 使用本卡 release/acquire 标志，生产者所有写入经过 system fence；通信 CTA 通过 dushmem device signal/wait 和 peer mapping 读取对端输出。每轮 epoch 递增，下一请求开始前两 rank 均完成，避免跨请求覆盖。','',
'## 同一二进制的五种对照','',
'| 模式 | 工作方式 | 用途 |','|---|---|---|',
'| v08_rccl | 原始已验证 v08 库 → RCCL BF16 AllReduce | 工程基线 |',
'| serial80 | 80 CTA，各自算一 tile 后通信归约，再算下一 tile | 无独立通信角色的对照 |',
'| phase_roles | C 计算 + R 通信 CTA；通信等全部本地计算结束后开始 | 相同角色/资源/通信实现，关闭交叠 |',
'| pipeline_roles | C 计算 + R 通信 CTA；每 tile 就绪即可消费 | 本次流水候选 |',
'| compute_only | 相同计算角色和发布协议，不通信 | 观察该结构自身的计算耗时；不与完整归约直接排名 |','',
'## 初版为什么出现 30+ ms','',
'最早的同 CTA 融合采用 80 个 CTA 分担通信。本次第一版改为仅 2/4/8 个专用通信 CTA，且每 lane 标量 BF16 读写，通信成为瓶颈。不是原来同一 out_proj 测试从 4 ms 无条件变成 30 ms。','',
'| 标量版 FC2 配比 C/R | 同轮 v08+RCCL ms | 先算后传 ms | 真正流水 ms |','|---|---:|---:|---:|']
for comm in (2,4,8):
    rows=[x for x in scalar['timings'] if x['comm']==comm and x['file'].startswith('screen_c')]
    vals={r['mode']:r['median_ms'] for r in rows}
    lines.append(f"| {80-comm}/{comm} | {vals['v08_rccl']:.4f} | {vals['phase_roles']:.4f} | {vals['pipeline_roles']:.4f} |")
lines += ['', '76/4 的关闭交叠控制组约 38.60 ms，流水约 34.24 ms：交叠有效，但无法弥补该通信实现的低吞吐。scalar 原始源码/二进制与全部负结果保留在根目录，不覆盖。','',
'## 通信向量化与配比筛选','',
'仅在 vec8/operators.hpp 的归约读写处，将每 lane 每次 1 个 BF16 改为连续 8 个 BF16（16 字节）的向量读写，逐元素相同 FP32 相加及 BF16 RNE。矩阵行、tile 列偏移均满足该向量的对齐要求，M 尾块仍按有效行保护。具体指令宽度以实际 ISA 为准，不单凭 C++ 类型声称硬件访存指令。所有对照模式使用相同通信实现；这不是只给流水版升级通信而让串行版保留旧代码。','',
'先按 FC2 流水中位数，在独立初筛中选配比；初筛不作为最终验收数据。','',
'| 配比 C/R | 同轮 v08+RCCL ms | 向量通信流水 ms |','|---|---:|---:|']
for r in selection['screening']:
    lines.append(f"| {80-r['comm']}/{r['comm']} | {r['baseline_median_ms']:.4f} | {r['pipeline_median_ms']:.4f} |")
lines += ['', '选定 72/8 后，重新执行随机输入门禁，再进行三个全新的独立进程正式复测，避免把筛选中最快单次直接当结果。out_proj 使用相同配比作为可迁移性测试，没有为它另做最优配比搜索。','',
'## 三次正式复测','',
'每进程每模式 3 次预热、3 个轮换顺序轮次×5 次正式调用（每模式15次）；取双 rank 较慢 HIP event 时间。每进程取15次中位数，再对三个进程中位数取算术平均。计时包含计算、通知/等待、对端读取、归约、必要写回和合作启动窗口开销；初始化、分配、参考、校验及 trace 不计入。','',
'| 模块 | v08+RCCL | serial80 | 同资源先算后传 | 同资源流水 | compute-only | 流水相对 RCCL 时间下降 |','|---|---:|---:|---:|---:|---:|---:|']
for row in result['formal_summary']:
    vals=[row['timings'][m]['mean_of_process_medians_ms'] for m in modes]
    lines.append('| '+row['label']+' | '+' | '.join(f'{v:.4f} ms' for v in vals)+f" | {row['pipeline_time_reduction_vs_rccl_percent']:+.2f}% |")
lines += ['', '正值表示时间减少，负值表示变慢。FC2 相对同资源 phase_roles 的时间减少约 42.01%，但工程净收益应报告相对 v08+RCCL 的 11.47%，不能混用。',
'out_proj 同轮 serial80 向量版约 4.2898 ms，比 RCCL 基线快约 7.75%；独立角色流水约 5.4155 ms 更慢。说明短 K 节点不一定适合当前预留通信角色的结构。','',
'| 模块 | 正式进程 | v08+RCCL 中位数 ms | pipeline 中位数 ms |','|---|---|---:|---:|']
for row in result['formal_summary']:
    bases=row['timings']['v08_rccl']['runs'];pipes=row['timings']['pipeline_roles']['runs']
    for b,p in zip(bases,pipes):lines.append(f"| {row['label']} | {b['file']} | {b['median_ms']:.4f} | {p['median_ms']:.4f} |")
lines += ['', '全部均值、范围和原始正式样本保留于 comparison.json，不删除尖峰，不采用最快单次。','',
'## 时间戳如何确认真正交叠','',
'独立 trace 运行记录每 tile 的计算开始/结束、发布、通信开始、对端就绪、归约完成；每 rank 每模式 1554 个 tile 均唯一且阶段顺序正确。只比较同一 GPU 的 wall_clock64 ticks，不跨 GPU 对时、不未经校准换算毫秒。','',
'| 模块 | rank | phase_roles 计算/通信交集 | pipeline 通信阶段与全局计算区间交集比例 | 同一生产者相邻 tile G(next) 与 R(current) 重叠对数 |','|---|---:|---:|---:|---:|']
for k,label in ((7168,'FC2'),(3584,'out_proj')):
    for rank in (0,1):
        t=next(t for t in vector['traces'] if f'trace_{k}_' in t['file'] and 'pipeline_roles' in t['file'] and f'rank{rank}' in t['file'])
        phase=next(t for t in vector['traces'] if f'trace_{k}_' in t['file'] and 'phase_roles' in t['file'] and f'rank{rank}' in t['file'])
        assert phase['intersection_ticks']==0 and t['adjacent_overlap_pairs']>0
        lines.append(f"| {label} | {rank} | 0 | {t['intersection_over_comm_union_percent']:.2f}% | {t['adjacent_overlap_pairs']}/{t['adjacent_pairs']} |")
lines += ['', '这些是“通信阶段（含 signal/wait/归约）”与计算阶段的时间交集，不是物理链路忙碌率，不是性能收益百分比。serial80 不同 CTA 也有自然交叠（FC2 全局比例约97%），但同一生产者的相邻 tile 重叠为0；因此不能用全局重叠率单独证明角色流水或直接给算法排名。','',
'## 正确性与工程限制','',
'周期输入每次更换 tag，并独立构造 CPU 精确分数参考；随机 BF16 输入每调用改变 rank 相关种子，完整输出对照原始 v08+RCCL，compute-only 则对照本卡 v08 局部结果。所有调用检查完整输出和256 BF16 guard；门禁及正式测试均零错误。','',
'- scalar：尾块 smoke、FC2/out_proj 随机门禁、2/4/8 初筛、76/4 trace 全通过。',
'- vec8：76/4 和选定72/8两轮随机门禁；5种配比初筛；三个正式进程（共540次双 rank 调用，含预热）；两 shape trace 全通过。controller 重定向日志只是镜像，不重复计算调用数。',
'- trace 和门禁时间不用于正式收益；尚未测试真实模型激活、INT8 计算、生产并发、异常恢复、跨节点、多于2个TP rank。',
'- 当前只实现单节点 TP2、连续 BF16 两种 shape。没有减少归约必需数据量，也仍写本地 partial output；不能把单算子收益当 H3 E2E 收益。',
'- 未重配驱动/编译器/系统库。编译及最终设备链接都传 --enable-num-vgprs-768=true。模型源码工作树未改，GPU作业已结束。','',
'## 下一组建议：两个 kernel 的 CTA 级流水','',
'值得测试，但不是因为单 kernel 必然无效：FC2 的本轮单 kernel 已有净收益。两 kernel 的意义是解除资源绑定，让 GEMM kernel 保持768 threads/64 KiB LDS，而通信 kernel 可用更少线程、较低寄存器、零 LDS。',
'两条 stream 同时启动；设备端 tile-ready 标志驱动消费，不让通信 stream 等整个 GEMM 完成，不在 CPU 上逐 tile 调度。先保持 full partial buffer，避免循环缓冲回压导致驻留不足时死锁。通信 CTA 数必须受限，验证两个 kernel 实际共同驻留和交叠，不能假定两条 stream 自动并行。',
'应比较同一通信实现的“两个 kernel 串行”和“两个 kernel 重叠”，再与本轮单 kernel 及 v08+RCCL 比较；最后才考虑缓冲复用与接入 H3。当前尚未实现或测试两 kernel 版。','',
'## 文件与复现','',
'根目录 scalar operators.hpp/bench.cpp/run.sh/analyze.py 保留初版；vec8/ 保存向量版、controller.py、formal_a/b/c.log、selection.json、analysis.json、trace_*.csv。environment.json 保存源码/二进制/依赖 SHA、Git revision/状态、编译器、设备状态；resources.txt 保存元数据工具输出（工具无法解析的字段不能当0）。',
'在 ck_yyc 内进入目标版本目录，bash run.sh build 编译；bash run.sh NEW_TAG 8 0 gate all 做随机门禁；bash run.sh NEW_TAG 8 3 full all 正式测试；bash run.sh NEW_TAG 8 1 trace all 采时间戳。所有脚本拒绝覆盖同名日志，并检查 HCU4/5 空闲。trace 若同配比文件已存在，使用新的实验目录，不能覆盖本轮证据。',
'analyze.py、summarize.py 只用 CPU；build/ 不提交，原始性能报告和源码快照应保留。未 commit/push。','']
(root/'RESULTS.md').write_text('\n'.join(lines),encoding='utf-8')
print(json.dumps({'formal_summary':[{k:v for k,v in r.items() if k!='timings'} for r in result['formal_summary']]},ensure_ascii=False,indent=2))
