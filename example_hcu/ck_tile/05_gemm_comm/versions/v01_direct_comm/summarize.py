"""CPU-only report regeneration. Formal performance: full_b/full_c, not gates."""
from pathlib import Path
import hashlib,json,statistics
root=Path(__file__).resolve().parent
files=['full_b.log','full_c.log']
def load(name):
    text=(root/name).read_text(encoding='utf-8',errors='replace')
    assert 'BENCH_PASS' in text,name
    rows=[json.loads(line) for line in text.splitlines() if line.startswith('{')]
    assert rows and all(r['errors']==0 for r in rows),name
    return rows
data={f:load(f) for f in files}
gate=load('random_gate.log')
modes=['v08_rccl','recompiled_rccl','tile_split','tile_fused','direct_split','direct_fused']
result={'scope':'TP2 operator segment, HCU4/5; not model E2E',
        'timing_binary_sha256':'0d0b76407673cac926a8bb29a5a42b9b43b02bc34493d9eeb575de3fb6f26f27',
        'timing_source_sha256':hashlib.sha256((root/'bench_timing.cpp').read_bytes()).hexdigest(),
        'formal_files':files,'checks':{'periodic_invocations':sum(map(len,data.values())),
        'random_invocations':len(gate),'all_full_output_bitwise_and_output_guards_pass':True},'shapes':[]}
for k,label in [(7168,'FC2'),(3584,'Attention out_proj')]:
    item={'label':label,'m':18880,'n':5376,'k':k,'timings':{}}
    for mode in modes:
        per_process={}
        for file,rows in data.items():
            vals=[r['maxrank_ms'] for r in rows if r['k']==k and r['mode']==mode and r['round']>=0]
            assert len(vals)==15,(file,mode,k,len(vals))
            per_process[file]={'median_ms':statistics.median(vals),'mean_ms':statistics.mean(vals),
                               'min_ms':min(vals),'max_ms':max(vals),'samples_ms':vals}
        item['timings'][mode]={'mean_of_process_medians_ms':statistics.mean(x['median_ms'] for x in per_process.values()),'per_process':per_process}
    t=item['timings'];base=t['v08_rccl']['mean_of_process_medians_ms'];fused=t['direct_fused']['mean_of_process_medians_ms']
    item['direct_fused_time_change_vs_v08_rccl_percent']=(fused/base-1)*100
    item['direct_fused_time_change_vs_direct_split_percent']=(fused/t['direct_split']['mean_of_process_medians_ms']-1)*100
    result['shapes'].append(item)
reference=json.loads((root/'current_backend.json').read_text(encoding='utf-8'))
result['installed_backend_reference']=reference
(root/'comparison.json').write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
lines=['# v08 CK-Tile GEMM 通算融合：算子级验证（2026-10-16）','',
'## 结论','',
'设备端 GEMM + TP2 归约已实现，带 M 尾块、全尺寸周期输入和随机 BF16 输入校验均通过；但当前结果不支持替换 H3 已部署的 LightOp INT8 路径。',
'直接对端读取版仅在 out_proj 相对 BF16 v08+RCCL 有约 4% 的算子段收益，FC2 无收益。不能将 BF16 候选的功能通过写成 INT8 同精度替换或模型 E2E 提升。','',
'## 测试口径','',
'- 主机 root@10.212.14.166，ck_yyc，物理 HCU4/5，gfx936；只模拟一个 TP2 组，不运行 H3、不包含 SP All-to-All，也没有同时运行第二个 TP2 组。',
'- X[M,K] × W[N,K]^T，M=18880、N=5376；FC2 K=7168，out_proj K=3584。BF16 输入/输出、FP32 累加，各 rank 先舍入 BF16，再求和并舍入 BF16，与双卡 RCCL BF16 Sum 的参考语义比较。',
'- 256×256×64 tile，v08 的 8 compute + 4 producer wave 流水；1554 个输出 tile，80 个常驻 worker CTA，合作启动容量查询为 80，末尾 token tile 只保留有效行。',
'- 每模式预热 3 次；正式 3 轮正反序，每轮 5 次。每次 HIP event 包含 GEMM、通信/等待、归约及恢复行主序输出；取双 rank 较慢值。初始化、校验、分配、权重量化不计时。两个独立进程运行各取 15 次的中位数，再取算术平均。完整原始样本含尖峰均保留。',
'- full_b/full_c 是最终正式计时；full_a 是只有打包实现的初版，不混入汇总。random_gate、smoke 和首次 JIT/加载耗时均不当作性能。','',
'## 同轮 BF16 对照','',
'| 模块 | v08 + RCCL | 重编译 v08 + RCCL | 打包分离 | 打包融合 | 对端直读分离 | 对端直读融合 | 直读融合相对 v08+RCCL 时间变化 |',
'|---|---:|---:|---:|---:|---:|---:|---:|']
for item in result['shapes']:
    vals=[item['timings'][m]['mean_of_process_medians_ms'] for m in modes]
    lines.append('| '+item['label']+' | '+' | '.join(f'{x:.4f} ms' for x in vals)+f" | {item['direct_fused_time_change_vs_v08_rccl_percent']:+.2f}% |")
lines += ['', '正数为变慢，负数为变快。这里比较的是完整 GEMM+归约算子段，不是仅 MMAC 内核。','',
'每个独立运行的中位数及正式样本范围：','',
'| 模块 | 运行 | v08+RCCL 中位数 / 范围 ms | 直读融合中位数 / 范围 ms |','|---|---|---|---|']
for item in result['shapes']:
    for f in files:
        cells=[]
        for mode in ('v08_rccl','direct_fused'):
            v=item['timings'][mode]['per_process'][f];cells.append(f"{v['median_ms']:.4f} / {v['min_ms']:.4f}–{v['max_ms']:.4f}")
        lines.append('| '+item['label']+' | '+f+' | '+' | '.join(cells)+' |')
lines += ['', '## 当前已安装 INT8 后端参照','',
'sglang_dev_yyc，Torch2.10.0；相同 HCU4/5、shape 和 TP2，随机合成输入，同进程交错测 v08/LightOp，各 5 次预热、6 轮×5 次 HIP event，取双 rank 轮均值较慢值的中位数。权重量化离线，动态激活 per-token INT8 量化纳入 quant 行。没有加载 H3 checkpoint，也不是完整 H3 线性层或旧 FC2 分块流水的复现。不同 harness 之间只作工程参照，不把差值全归因于通信；同一 harness 的 v08 对照用于核对量级。','',
'| 模块 | 同进程 v08 + RCCL | LightOp 激活量化+GEMM+RCCL | LightOp 已量化 GEMM+RCCL | BF16 直读融合（独立 C++ 测试） |','|---|---:|---:|---:|---:|']
for item,ref in zip(result['shapes'],reference['shapes']):
    ts=ref['timings'];vals=[ts[x]['median_ms'] for x in ('v08_rccl','lightop_quant_gemm_rccl','lightop_prequant_gemm_rccl')]
    vals.append(item['timings']['direct_fused']['mean_of_process_medians_ms'])
    lines.append('| '+item['label']+' | '+' | '.join(f'{v:.4f} ms' for v in vals)+' |')
lines += ['', 'LightOp 调用 gemm_ops.triton_scaled_mm，进程映射记录 gfx936/gemm_w8a8_smooth_256x256x128_WGM8_TN_BF16.co。INT8 与 BF16 数值语义不同，该参照仅做性能/后端核对，未宣称两者输出逐 bit 相同。','',
'## 实现方式与为什么暂时没有普遍收益','',
'1. `tile_split/tile_fused`：v08 计算部分输出，写 packed 256×256 BF16 tile；dushmemx_putmem_signal_block 发送整个 tile，等待对端后相加并写回行主序。前者分两个 kernel，后者同一 kernel 处理。打包中转、设备通信调用和恢复布局带来成本，不能仅凭减少 launch 认为会更快。',
'2. `direct_split/direct_fused`：保留原行主序 partial output，以 dushmem_ptr 获得节点内对端映射；所有生产线程 system fence 后，由设备端 dushmemx_signal_op 发布 tile 就绪信号，等待匹配 epoch，再直接读取对端 partial output、相加并输出。前者 compute/通信分 kernel，后者在同一 kernel 内做。',
'3. 同 CTA 内仍是“本 tile 计算→通信/归约→下一个 tile”；不同 CTA 可处于不同阶段，但本轮没有 trace 证明重叠比例，不将约 4% 的收益全称为通信隐藏。它不是专门 wave 在同 CTA 内持续计算与通信的完整流水。',
'4. FC2 K 更长，当前融合后的额外同步、持久调度和通信等待未被有效掩盖；具体各成本占比未采 profile，不能指定某个 stall 为已证实唯一原因。','',
'## 正确性与编译问题记录','',
f"- 两次最终正式运行合计 {result['checks']['periodic_invocations']} 次调用（含预热），每次检查双 rank 完整输出和 256 BF16 输出 guard，错误均为 0。周期输入独立 CPU 精确分数参考，输入 tag 每调用改变。",
f"- random_gate 共 {len(gate)} 次调用，每调用改变 rank 相关随机种子，随机 BF16 X/W；全部输出与原 v08 库+RCCL 逐 bit 一致，guard 完整。门禁运行不混入正式计时。",
'- 首轮 smoke_a/smoke_b 发现设备 RDC 最终链接阶段漏传 `-mllvm --enable-num-vgprs-768=true` 时，重编译 GEMM 输出错误。仅编译 -c 阶段传入该选项不够。补齐最终 hipcc 链接选项后，smoke_c、smoke_direct、全尺寸及随机门禁通过；失败日志保留，不作为模型或正式 v08 的缺陷结论。',
'- 仅上述修正前后构建对照通过，不声称已经用反汇编锁定错误的微架构根因。HSA_FORCE_FINE_GRAIN_PCIE=1 为所有最终模式统一使用的进程选项；未更换环境库。','',
'## 接入 H3 前仍需补齐','',
'仅限节点内 TP2、可访问 peer mapping、连续 BF16、当前两种 shape、无 bias；并非通用 RCCL AllReduce 替代。还需要处理框架通信组/rank、对称内存生命周期、流依赖、重复请求/异常退出、并发及真实激活验证。当前 INT8 后端不能直接换成 BF16 候选而仍称保持原量化实现。性能尚不足以支持该替换；未改 SGLang 正式源码，未启动模型、未 commit/push。','',
'## 文件与复现','',
'- `bench_timing.cpp`：full_b/full_c 实际计时源码快照；`bench.cpp` 在同一实现上增加可选 H3_RANDOM_GATE 门禁，默认关闭。',
'- `run.sh build` / `run.sh build_gate`：分别构建计时和随机验证；编译及最终设备链接都传扩展寄存器选项。',
'- `bash run.sh fresh_tag 80 3 full`：ck_yyc 内真实 shape；`bash run.sh fresh_gate 80 1 gate`：随机门禁。脚本拒绝覆盖同名日志，检查 HCU4/5 空闲，600s 超时仅管理自己的 mpirun。',
'- `run_current.sh fresh_reference_tag`：sglang_dev_yyc 中当前后端参考；依赖现有 runtime_env.sh，不改全局环境。',
'- `capture.py`、`environment.json`：编译器、Git revision/已有工作树状态、源码/二进制/库 SHA256、时钟与测试后显存。',
'- `comparison.json`：各次中位数、均值、范围和全部正式原始样本；`summarize.py` 仅 CPU 重新生成报告。',
'- v08 库 SHA256：`f95f93180f23f631ea27925c45c0b0b77012228e2fc08254e5b3a8ec607712c5`；正式计时 executable SHA256：`'+result['timing_binary_sha256']+'`。',
'- 时间是 GPU event 测量的算子段，不等于模型 E2E 或 CPU 调度时间；未执行四卡完整 TP2+SP2 或八卡模型测试。','']
(root/'RESULTS.md').write_text('\n'.join(lines),encoding='utf-8')
print(json.dumps({'checks':result['checks'],'summary':[{k:v for k,v in x.items() if k!='timings'} for x in result['shapes']]},ensure_ascii=False,indent=2))
