"""CPU-only final report. Uses final_* independent processes, never screens/gates."""
from pathlib import Path
import csv,json,statistics
root=Path(__file__).resolve().parent
analysis=json.loads((root/'analysis.json').read_text())
selection=json.loads((root/'selection_extended.json').read_text())
environment=json.loads((root/'environment.json').read_text())
modes=['v08_rccl','single_serial80','single_pipeline72_8','two_serial','two_overlap','compute_only']
result={'scope':'BF16 TP2 operator-only, physical HCU4/5, no H3/E2E/INT8 substitution',
        'selection':selection,'analysis':analysis,'environment':environment,'formal':[]}
for shape,k,label in [('fc2',7168,'FC2'),('out',3584,'Attention out_proj')]:
    config=selection['selected'][shape]['config']
    row={'shape':shape,'label':label,'m':18880,'n':5376,'k':k,'config':config,'timings':{}}
    for mode in modes:
        rs=[r for r in analysis['timings'] if r['file'] in [f'final_{shape}_{c}.log' for c in 'abc'] and r['mode']==mode and r['k']==k]
        assert len(rs)==3 and all(r['n']==15 for r in rs),(shape,mode)
        assert all([r['compute'],r['comm'],r['comm_threads']]==config for r in rs)
        row['timings'][mode]={'mean_of_process_medians_ms':statistics.mean(r['median_ms'] for r in rs),'runs':rs}
    ts={m:v['mean_of_process_medians_ms'] for m,v in row['timings'].items()}
    row['time_reduction_percent']={ref:(1-ts['two_overlap']/ts[ref])*100 for ref in modes if ref not in ('two_overlap','compute_only')}
    cs,cr,b=config
    row['trace_proof']=[]
    for rank in (0,1):
        name=f'trace_{k}_c{cs}_r{cr}_b{b}_two_overlap_rank{rank}.csv'
        proof=next(t for t in analysis['traces'] if t['file']==name)
        assert proof['tile_pipeline_observed'] and proof['adjacent_overlap_pairs']>0
        control=next(t for t in analysis['traces'] if t['file']==name.replace('two_overlap','two_serial'))
        assert control['intersection_ticks']==0 and control['adjacent_overlap_pairs']==0
        row['trace_proof'].append({'rank':rank,'overlap':proof,'serial':control})
    result['formal'].append(row)
assert all(c['complete'] and c['errors']==0 for c in analysis['checks'])
(root/'comparison.json').write_text(json.dumps(result,indent=2)+'\n')
lines=['# 两 kernel、CTA/tile 级通算重叠：BF16 TP2 实验','',
'日期：2026-10-16。独立 HCU4/5 算子测试，不加载 H3、不修改模型/共享 CK、不 commit/push。','',
'## 核心结论','',
'已在两个独立 kernel / 两条非默认 stream 上实现并验证逐 tile 计算通信重叠：通信不是等整个 GEMM 完成才开始。调整通信并发后，两种真实 shape 均快于同轮 BF16 v08+RCCL。',
'下面采用补充初筛后，三次全新独立进程正式复测的结果；不是筛选最快单次或历史报告。所有完整输出、边界 guard 及随机输入门禁通过。','',
'| 节点 | GEMM CTA | 通信 CTA × threads | BF16 v08+RCCL | 两 kernel 逐 tile 重叠 | 算子段时间下降 |','|---|---:|---:|---:|---:|---:|']
for row in result['formal']:
    t=row['timings'];c,r,b=row['config']
    lines.append(f"| {row['label']} | {c} | {r}×{b} | {t['v08_rccl']['mean_of_process_medians_ms']:.4f} ms | {t['two_overlap']['mean_of_process_medians_ms']:.4f} ms | {row['time_reduction_percent']['v08_rccl']:.2f}% |")
lines += ['', '这些是完整 GEMM+TP2归约段的收益，不是 H3 E2E 或 INT8 路径的收益。FC2 和 out_proj 分别选择参数，不假设相同配比对所有 shape 最优。','',
'## 执行结构：没有等待整个 GEMM','',
'- 计算 stream：普通 HIP 启动 compute_kernel，保留 v08 的768线程、64 KiB LDS、8计算wave+4权重搬运wave。每算完一个256×256输出tile，所有写入经过system fence，发布本卡 produced[tile]=epoch，然后计算下一块。',
'- 通信 stream：普通 HIP 启动独立 communication_kernel，仅等待对应 produced[tile]，通过 dushmem device signal/wait 和 peer mapping 读取对端局部输出、求和、BF16 RNE写回。无LDS，128/256/512 threads分别编译。',
'- 两条stream只等待共同的计时start event；two_overlap没有等待gdone。最后由join stream等待两条stream结束并记录end event，保证总时间包含二者完成。',
'- 只有辅助two_serial控制组显式执行通信stream等待整个GEMM的gdone event。它与two_overlap使用完全相同的计算和通信kernel，用来隔离交叠本身的效果；主要工程基线仍是v08+RCCL。',
'- 不在CPU上逐tile发起GEMM/通信，不分4个host chunk；全尺寸partial buffer每tile独立位置，生产者不等消费者释放槽位，避免循环缓冲与驻留造成循环等待。每调用epoch递增，两rank和两个stream均完成后再进入下一调用。','',
'## 范围和资源','',
'主机root@10.212.14.166，ck_yyc，物理HCU4/5、gfx936；仅一个TP2组，无SP All-to-All及其它TP组并发。M18880、N5376；FC2 K7168，out_proj K3584。BF16输入/局部输出，FP32累加，局部结果各自舍入BF16后双卡求和再RNE，保留参考语义。',
'原始v08库SHA256 f95f93180f23f631ea27925c45c0b0b77012228e2fc08254e5b3a8ec607712c5。reference_operators.hpp 与上一轮vec8/operators.hpp逐字节相同；单kernel对照在本轮重新编译、重新计时，不直接挪用历史6.05ms。','',
'rank0 hipFuncGetAttributes报告：','',
'| kernel | VGPR/numRegs | LDS bytes | private/local bytes |','|---|---:|---:|---:|',
'| compute（768 threads） | 256 | 65536 | 24 |',
'| communication（128 threads） | 87 | 0 | 8 |',
'| communication（256 threads） | 83 | 0 | 8 |',
'| communication（512 threads） | 83 | 0 | 8 |','',
'private/local大小可能含调用帧或局部存储，不能单凭它断言寄存器spill。资源独立不代表已证明两个kernel在同一个CU共驻；本轮证明的是同GPU不同CTA的阶段交叠，不声称具体CU分配。两个kernel的CTA总数可以超过80，因为通信CTA不再被强制绑定64KiB LDS，且其驻留/调度与计算kernel不同；本轮没有采硬件CU归属。','',
'## 同轮全部正式对照','',
'| 模块 | v08+RCCL | 单kernel serial80 | 单kernel 72+8流水 | 两kernel串行控制 | 两kernel重叠 | 新compute-only |','|---|---:|---:|---:|---:|---:|---:|']
for row in result['formal']:
    lines.append('| '+row['label']+' | '+' | '.join(f"{row['timings'][m]['mean_of_process_medians_ms']:.4f} ms" for m in modes)+' |')
lines += ['', 'compute-only包含tile发布协议，不能与完整归约段直接排名；two_overlap与compute-only的差额也不能直接当作纯通信时间，因为并发会改变计算、访存和调度。',
'单kernel serial80是同一CTA内算一块再通信一块；不同CTA仍可自然交叠。固定72+8单kernel独立角色流水与本轮两个kernel是不同结构，不应只看kernel数量判断优劣。','',
'| 模块 | 相对 BF16+RCCL | 相对单kernel serial80 | 相对单kernel72+8流水 | 相对同实现two_serial |','|---|---:|---:|---:|---:|']
for row in result['formal']:
    lines.append('| '+row['label']+' | '+' | '.join(f"{row['time_reduction_percent'][m]:+.2f}%" for m in modes[:4])+' |')
lines += ['', '正数表示耗时下降，负数表示变慢。相对two_serial的收益是同一通信算法内的交叠效果；不能用它代替相对RCCL的工程收益。','',
'## 正式计时方法和逐次结果','',
'每节点3个全新独立进程，每模式3次预热，然后3轮轮换顺序×5次正式调用；每进程每模式15次。每调用取双rank较慢HIP event时间；每进程取中位数，再平均3个进程中位数。event覆盖两stream开始、kernel/通信/等待/归约、最终join，不含初始化/分配/参考/校验。trace/gate/screen均不混入正式结果；所有原始尖峰保留。','',
'| 模块 | 进程 | v08+RCCL中位数 | two_overlap中位数 | two_overlap样本范围 |','|---|---|---:|---:|---:|']
for row in result['formal']:
    for b,p in zip(row['timings']['v08_rccl']['runs'],row['timings']['two_overlap']['runs']):
        lines.append(f"| {row['label']} | {p['file']} | {b['median_ms']:.4f} | {p['median_ms']:.4f} | {p['min_ms']:.4f}–{p['max_ms']:.4f} ms |")
lines += ['', '日志中的rank0_compute_done_ms是计算stream的终点；基线模式在该stream上也运行了RCCL/单kernel通信，不能把所有模式下这个字段都当作纯GEMM。rank0_comm_done_ms也含等待。它们不是双rank的分阶段最大值。','',
'## 初筛与负结果保留','',
'初始7组参数虽通过正确性和真实交叠，但通信线程量不足，未超过基线；首轮selection.json、formal_fc2/out_*.log原样保留。随后固定同一二进制，追加4组通信并发参数；selection_extended.json按每节点two_overlap中位数最小且双rank通过真实交叠门禁选择，再做新的final_*复测。初筛不用于最终验收。','',
'| 配置 compute/comm/threads | FC2 two_overlap初筛 | out_proj two_overlap初筛 |','|---|---:|---:|']
configs=[]
for s in selection['screening']:
    if s['config'] not in configs:configs.append(s['config'])
for c in configs:
    rows=[s for s in selection['screening'] if s['config']==c]
    vals={s['k']:s['timings_ms']['two_overlap'] for s in rows}
    lines.append(f"| {c[0]}/{c[1]}/{c[2]} | {vals[7168]:.4f} ms | {vals[3584]:.4f} ms |")
lines += ['', '增加计算CTA到76/80没有自动带来更好重叠结果，不能因GEMM并发更大就认为整体更快。资源挤占/调度是可能解释；未单独采这些非选中配置的trace，不能断言它们完全没有交叠。','',
'## 时间戳证明是CTA/tile级重叠','',
'独立采样记录每tile的Gstart/Gend/发布/Rstart/对端就绪/Rend；每rank每scope1554个tile完整唯一、顺序正确。仅比较同GPU wall_clock64 ticks，不跨卡对时，不未经校准换算ms。','',
'| 模块 | rank | 通信早于整个GEMM结束 | 同一生产者G(next)与R(current)重叠对数 | 通信阶段与全局计算区间交集比例 | two_serial交集 |','|---|---:|---|---:|---:|---:|']
for row in result['formal']:
    for entry in row['trace_proof']:
        t=entry['overlap'];lines.append(f"| {row['label']} | {entry['rank']} | 是 | {t['adjacent_overlap_pairs']}/{t['adjacent_pairs']} | {t['intersection_over_comm_union_percent']:.2f}% | 0 |")
lines += ['', '通信阶段包含signal/wait/归约，交集比例不是物理链路忙碌率或加速率。两个stream存在任务不是充分证明；这里同时检查Rstart早于最大Gend，以及同一生产者下一tile计算与上一tile归约存在正时间交集。two_serial的交集为0，作为反证控制。','',
'## 正确性、状态及限制','',
f"本目录去除controller镜像重复后，共{sum(c['invocations'] for c in analysis['checks'])}次双rank调用（含预热/门禁/初筛/trace及先前正式轮次）全部完成、完整输出/256 BF16 guard错误为0。最终final_*六个进程合计648次调用（含预热），正式性能按每模式45个样本、3个独立中位数汇总。",
'周期输入每调用更换tag并独立CPU精确参考；随机BF16输入每调用更换rank相关种子，全输出逐bit比较原v08+RCCL，compute-only对照本卡v08局部结果。',
'使用现有DTK、MPI、RCCL、dushmem与过程级环境变量，没有替换任何库。正常HIP启动的通信kernel仅使用点对点device signal/wait和peer访问，未把device全局集体操作跨两kernel调用。通信CTA数限制为32以内，生产者不等消费者；这不是任意调度/跨节点环境下的通用进展保证。',
'仅单节点TP2、连续BF16、两种shape、无bias。未加载模型真实权重/激活，未验证INT8替换、生产并发、异常恢复、动态batch或SP通信；不能直接替换当前LightOp W8A8路径或推断H3 E2E提升。',
'模型源码工作树未改，未commit/push；所有GPU作业已结束。完整源码/库/二进制SHA、Git revision、编译器、时钟、测试后设备状态见environment.json。','',
'## 文件和复现','',
'- reference_operators.hpp：上一轮vec8单kernel源码快照，SHA与原文件一致。two_kernel.hpp：独立计算/通信kernel。bench.cpp：两stream事件依赖、六组对照、参考与校验。',
'- run.sh：编译、GPU空闲检查、600s有界mpirun；拒绝覆盖同名日志。controller.py是首轮7配置，extended_controller.py是额外4配置；均已结束，不是H3后台server。',
'- selection.json与formal_*保留首轮结果；selection_extended.json与final_*为最终参数选择及独立正式复测。所有trace_*.csv、analysis.json、comparison.json保留逐样本与时间戳。',
'- 在ck_yyc对应目录执行bash run.sh build；FC2最终配置示例：bash run.sh NEW_TAG 72 32 256 3 full fc2；out_proj：bash run.sh NEW_TAG 64 16 512 3 full out。kind改gate、rounds设0可跑随机门禁；kind改trace可采独立时间戳，同配置已有trace时使用新实验目录。',
'- 编译和最终设备链接均保留--enable-num-vgprs-768=true，依赖冻结v08库及现有CK include/DTK/MPI/RCCL/dushmem。build/不归档进Git；CPU工具analyze.py、summarize.py、capture.py不启动GPU负载。','']
(root/'RESULTS.md').write_text('\n'.join(lines),encoding='utf-8')
print(json.dumps({'formal':[{k:v for k,v in r.items() if k not in ('timings','trace_proof')} for r in result['formal']]},ensure_ascii=False,indent=2))
