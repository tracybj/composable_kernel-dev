"""Recompute v08's declared mean-time gate from all independent recorded runs."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import statistics

version=Path(__file__).resolve().parents[1]
root=version.parents[1]
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--runs-root',type=Path,default=root/'build/runs')
p.add_argument('--build-root',type=Path,default=root/'build')
a=p.parse_args()
evidence=version/'evidence';evidence.mkdir(exist_ok=True)
reference='v07_struct32_330t';candidate='v08_ck_tile'
manifest=json.loads((version/'manifest.json').read_text('utf-8'))
for relative,h in manifest['source_sha256'].items():
    assert hashlib.sha256((version/'src'/relative).read_bytes()).hexdigest()==h
audit=json.loads((a.runs_root/'v08_final_audit_20261016/summary.json').read_text('utf-8'))
assert audit['passed'] and audit['groups']==310 and audit['candidate_launches']==924
assert all(c['passed'] for c in audit['api_contracts'])
rounds=[]
for suffix in 'abc':
    tag='v08_final_round_'+suffix+'_20261016'
    rows=json.loads((a.runs_root/tag/'results.json').read_text('utf-8'))
    env=json.loads((a.runs_root/tag/'environment.json').read_text('utf-8'))
    assert len(rows)==4
    assert env['libraries'][candidate]['sha256']==audit['library_sha256']['v08']
    assert env['libraries'][reference]['sha256']==audit['library_sha256']['v07']
    assert all(c['passed'] for r in rows for modes in r['correctness'].values() for c in modes.values())
    rounds.append(rows)
comparisons=[]
for i in range(4):
    baseline=[r[i]['timing'][reference]['median_ms'] for r in rounds]
    current=[r[i]['timing'][candidate]['median_ms'] for r in rounds]
    old,new=statistics.mean(baseline),statistics.mean(current)
    m,n,k=rounds[0][i]['MNK']
    comparisons.append({'shape':rounds[0][i]['shape'],'MNK':[m,n,k],
        'v07_round_medians_ms':baseline,'v08_round_medians_ms':current,
        'v07_mean_ms':old,'v08_mean_ms':new,'time_change_percent':(new/old-1)*100,
        'v08_TFLOPS':2*m*n*k/(new*1e9),
        'each_round_time_change_percent':[(y/x-1)*100 for x,y in zip(baseline,current)]})
square=[]
for suffix in 'ab':
    d=json.loads((a.runs_root/f'v08_final_square_{suffix}_20261016/results.json').read_text('utf-8'))[0]
    assert all(c['passed'] for mode in d['correctness'].values() for c in mode.values())
    square.append(d['timing'])
square_comparisons=[]
for suffix in ['', '__structured','__producer']:
    old=statistics.mean(d[reference+suffix]['median_ms'] for d in square)
    new=statistics.mean(d[candidate+suffix]['median_ms'] for d in square)
    square_comparisons.append({'mode':suffix or 'default raw fallback','v07_mean_ms':old,
        'v08_mean_ms':new,'time_change_percent':(new/old-1)*100,'v08_TFLOPS':2*4096**3/(new*1e9)})
resources=json.loads((a.build_root/'v08_isa_final/resources.json').read_text('utf-8'))
assert resources[candidate]['library_sha256']==audit['library_sha256']['v08']
assert all(k['vgpr_spill_count']==0 and k['private_segment_fixed_size']==0 and
           k['group_segment_fixed_size']==65536 for k in resources[candidate]['kernels'])
gate={'metric':'mean of three independent same-run median-of-round-means times, per H3 shape',
      'maximum_mean_regression_percent':1.0,
      'h3_passed':all(c['time_change_percent']<=1.0 for c in comparisons),
      'square_passed':all(c['time_change_percent']<=1.0 for c in square_comparisons)}
assert gate['h3_passed'] and gate['square_passed'],gate
summary={'date':'2026-10-16','reference':reference,'candidate':candidate,'gate':gate,
    'h3_comparisons':comparisons,'square4096_comparisons':square_comparisons,
    'audit_groups':audit['groups'],'audit_candidate_launches':audit['candidate_launches'],
    'api_rejection_checks':len(audit['api_contracts']),'library_sha256':audit['library_sha256'],
    'resources':resources,'source_sha256':manifest['source_sha256'],
    'interpretation':'Performance-preserving under the stated 1% mean-time gate; not identical ISA or a claim that every run/shape is faster.'}
(version/'RESULTS.json').write_text(json.dumps(summary,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
tags=['v08_final_audit_20261016']+['v08_final_round_'+s+'_20261016' for s in 'abc']+[
    'v08_final_square_'+s+'_20261016' for s in 'ab']
preliminary=['v08_initial_smoke_20261016','v08_initial_full_20261016',
             'v08_forceinline_full_20261016','v08_unsigned_full_20261016','v08_policy_types_full_20261016']
for tag in tags+preliminary:
    src=a.runs_root/tag
    if not src.exists():continue
    target=evidence/tag;target.mkdir(exist_ok=True)
    for f in src.glob('*.json'):shutil.copy2(f,target/f.name)
for stage in ['initial','forceinline','unsigned','final']:
    src=a.build_root/('v08_isa_'+stage)/'resources.json'
    if src.is_file():shutil.copy2(src,evidence/('resources_'+stage+'.json'))
for name in [candidate,candidate+'__structured',candidate+'__producer']:
    shutil.copy2(a.build_root/(name+'.json'),evidence/(name+'.build.json'))
report='''# v08 CK-Tile 分层重构：性能与正确性验收

验收日期 2026-10-16。v08 保留 v07 的三条最终路径，移除历史实验宏与重复 .cpp 包含，按 Problem / Policy / Pipeline / Epilogue / Kernel / Dispatch / gfx936 arch 分层。

## 测量条件与门槛

HCU4 / gfx936 / 80 CU，ck_yyc，Torch2.9.0 / HIP6.3.26113，原 DTK / hipcc；CK include 固定 `5c8ea51...`，源码及库 SHA 在 JSON 中逐项记录。无 H3 server、无 profiler、无环境库替换。

四类真实 H3 shape，M18880，BF16 输入/输出、FP32 累加。每次 5 预热、6 轮轮换顺序、每轮 10 次 HIP event；取轮均值的中位数。再用三个独立进程复测，表中是三个中位数的算术均值。hot-cache，同轮 v07/v08 比较，未使用最快三次或历史约 330T 代替本轮参考。

门槛：每个 H3 shape 的三轮平均时间，相对同轮 v07 回退不超过 1%；4096³ 两轮补充比较同样检查 1%。这是有量化界限的性能保持，不代表 ISA 完全一致，也不声称所有单次结果都更快。

## 三轮真实 shape 结果

| shape | v07 平均 ms | v08 平均 ms | 时间变化 | v08 TFLOPS |
|---|---:|---:|---:|---:|
'''
for c in comparisons:report+=f'| {c["shape"]} | {c["v07_mean_ms"]:.6f} | {c["v08_mean_ms"]:.6f} | {c["time_change_percent"]:+.3f}% | {c["v08_TFLOPS"]:.3f} |\n'
report+='\n正数表示变慢，负数表示变快。FC2 均值略慢，其余持平或略快；不把小变化夸大为新的性能优化。\n\n| shape | A 轮变化 | B 轮变化 | C 轮变化 |\n|---|---:|---:|---:|\n'
for c in comparisons:report+=f'| {c["shape"]} | '+ ' | '.join(f'{v:+.3f}%' for v in c['each_round_time_change_percent'])+' |\n'
report+='\n保留了全部轮次，包括 FC2 某一轮约 1% 的回退和初筛中的时间尖峰；验收依据为事先声明的三轮均值，不删除不利数据。\n\n## 4096³ 补充结果\n\n| 模式 | v07 ms | v08 ms | 时间变化 | v08 TFLOPS |\n|---|---:|---:|---:|---:|\n'
for c in square_comparisons:report+=f'| {c["mode"]} | {c["v07_mean_ms"]:.6f} | {c["v08_mean_ms"]:.6f} | {c["time_change_percent"]:+.3f}% | {c["v08_TFLOPS"]:.3f} |\n'
report+='''
4096³ 的统一入口依然是 raw fallback；直接 producer 入口约 286T，与上次约 287T 的口径接近。此处以本轮 v07 对照，不用旧值计算新的加速率。

## 正确性

- 310 组、924 次候选调用全部通过；每组实际与 v07 同输入逐 bit 比较，包含 Inf/NaN payload、有符号零和 subnormal。
- `special` 组允许非有限输出，以与 v07 的位模式一致为通过条件；普通输入仍要求 finite。
- 普通/二进制分数输入还对照 FP32 GEMM→BF16，按原容差及精确规则检查。
- 三种子、三次重复、两个输出对齐（0 / 1 BF16 偏移）、输入未修改、输出 guard、determinism 均通过。
- 包含 M1/63/64/255/256/257/511/513/18880、奇数 K64 tile、四个真实 N/K、raw K8192/16384、4096³、两个直接策略。
- 9 项主机端拒绝接口与 v07 一致，包括 structured K8192 拒绝。
- 三轮正式性能测试与两轮 4096³ 测试均再次先通过正确性门槛才计时。

## 寄存器与硬件资源

| 路径 | v07 VGPR | v08 VGPR | SGPR（两者） | LDS（两者） | spill / scratch |
|---|---:|---:|---:|---:|---|
| raw fallback | 225 | 234 | 31 | 64 KiB | 均 0 |
| structured | 233 | 236 | 38 | 64 KiB | 均 0 |
| structured + producer | 233 | 237 | 38 | 64 KiB | 均 0 |

寄存器数并非完全相同，ISA 也有变化。没有把“没有 spill”当成性能保持的证明，结论来自同轮实测。未采集 xprof，不声称某项硬件 stall/counter 有确定降幅。

## 重构过程中发现的性能敏感点

1. 最初完成分层后，FC2 约回退 4%–5%，未直接验收。该失败轮次 JSON 保留在 evidence。
2. 检查到 `CK_TILE_DEVICE` 在此 CK 中只是普通 inline；补显式内联约束后，库 SHA 完全未变，说明此项不是实际修复收益来源。约束保留为热路径接口契约，但不声称它带来加速。
3. 源码分层后，LDS 地址表达式的范围推断及生成调度发生变化。把所有路径都写成 unsigned 恢复了 FC2，却让 QKV/FC1 回退，未采用统一改法。
4. 最终只让 paired-producer Policy 使用显式非负坐标类型，plain/raw 保留 signed 表达式。两种形式在有效 tile 坐标域数学等价；选择是编译期类型决定，不是运行时实验开关。三轮复测及广审计验证最终组合。

Problem 最终使用 CK 原生 `TileGemmShape2Groups` 推导相同常量。该类型契约完善后，重建库与正式测试库逐字节同 SHA；因此已有 GPU 结果继续适用，不把源码行数/组织变化误当作算法变化。

仍保留的终末 VM 排空、pending bank 生存期、A LDS last-use 与匹配 barrier、checked token voffset、BF16 RNE/NaN 语义均属于正确性协议。

## 重建与证据

源码、源文件哈希、构建入口见 README.md / manifest.json。库完整哈希、设备资源、所有平均值及逐轮数据在 RESULTS.json。evidence 包含正式每轮原始 results/environment、广审计全部组、初筛失败轮次、资源元数据和编译参数。大 ISA / device ELF 留在忽略的 build/v08_isa_*，不把编译二进制加入 Git。

重新生成本报告：

```bash
python3 versions/v08_ck_tile/tools/summarize.py
```

该命令只读已有测量 JSON 并做 CPU 汇总，不重新跑 GPU。性能结论仅针对已测 BF16 单算子、当前编译器/硬件和给定 shape；没有把它写成 H3 E2E 或任意 shape 的收益。
'''
(version/'RESULTS.md').write_text(report,encoding='utf-8')
manifest['status']='validated: bitwise v07 audit passed; three-run H3 / two-run square <=1% mean-time gate'
manifest['validation_report']='RESULTS.json'
for mode,suffix in [('default',''),('structured','__structured'),('producer','__producer')]:
    metadata=json.loads((a.build_root/(candidate+suffix+'.json')).read_text('utf-8'))
    manifest['variants'][mode]['library_sha256']=metadata['sha256']
(version/'manifest.json').write_text(json.dumps(manifest,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
print(json.dumps(summary,indent=2),flush=True)
