"""CPU-only timing/trace inventory; no GPU workloads."""
from pathlib import Path
import csv,json,statistics
root=Path(__file__).resolve().parent
timings=[];checks=[]
for path in sorted(root.glob('*.log')):
    if 'controller' in path.stem:continue
    rows=[]
    for line in path.read_text(encoding='utf-8',errors='replace').splitlines():
        if line.startswith('{'):
            try:rows.append(json.loads(line))
            except json.JSONDecodeError:pass
    if not rows:continue
    passed='PIPELINE_PASS' in path.read_text(encoding='utf-8',errors='replace')
    checks.append({'file':path.name,'complete':passed,'invocations':len(rows),'errors':sum(r['errors'] for r in rows)})
    assert passed and all(r['errors']==0 for r in rows),path
    for k in sorted({r['k'] for r in rows}):
        for comm in sorted({r['comm'] for r in rows}):
            for mode in sorted({r['mode'] for r in rows}):
                vals=[r['maxrank_ms'] for r in rows if r['kind']=='full' and r['k']==k and r['comm']==comm
                      and r['mode']==mode and r['round']>=0 and not r['trace']]
                if vals:timings.append({'file':path.name,'k':k,'compute':80-comm,'comm':comm,'mode':mode,
                    'n':len(vals),'median_ms':statistics.median(vals),'mean_ms':statistics.mean(vals),
                    'min_ms':min(vals),'max_ms':max(vals),'samples_ms':vals})
def union(intervals):
    out=[]
    for lo,hi in sorted(intervals):
        if hi<=lo:continue
        if out and lo<=out[-1][1]:out[-1]=(out[-1][0],max(out[-1][1],hi))
        else:out.append((lo,hi))
    return out
def length(intervals):return sum(hi-lo for lo,hi in intervals)
def intersection(a,b):
    i=j=0;total=0
    while i<len(a) and j<len(b):
        total+=max(0,min(a[i][1],b[j][1])-max(a[i][0],b[j][0]))
        if a[i][1]<b[j][1]:i+=1
        else:j+=1
    return total
traces=[]
for path in sorted(root.glob('trace_*.csv')):
    rows=[{k:int(v) for k,v in r.items()} for r in csv.DictReader(path.open())]
    assert len(rows)==1554 and {r['tile'] for r in rows}==set(range(1554)),path
    communicating=not ('compute_only' in path.name)
    assert all(r['g_start']<=r['g_end']<=r['published'] and r['g_start']>0 for r in rows),path
    item={'file':path.name,'tiles':len(rows),'communicating':communicating}
    if communicating:
        assert all(r['g_end']<=r['r_start']<=r['peer_ready']<=r['r_end'] for r in rows),path
        gu=union([(r['g_start'],r['g_end']) for r in rows]);ru=union([(r['r_start'],r['r_end']) for r in rows])
        ov=intersection(gu,ru)
        groups={}
        for r in rows:groups.setdefault(r['producer'],[]).append(r)
        adjacent=hits=0;ticks=0
        for group in groups.values():
            group.sort(key=lambda r:r['tile'])
            for this,nxt in zip(group,group[1:]):
                assert this['g_end']<=nxt['g_start'],path
                overlap=max(0,min(this['r_end'],nxt['g_end'])-max(this['r_start'],nxt['g_start']))
                adjacent+=1;hits+=overlap>0;ticks+=overlap
        item.update({'compute_union_ticks':length(gu),'comm_union_ticks':length(ru),
          'intersection_ticks':ov,'intersection_over_comm_union_percent':100*ov/length(ru),
          'adjacent_pairs':adjacent,'adjacent_overlap_pairs':hits,'adjacent_overlap_ticks_sum':ticks,
          'compute_finish':max(r['g_end'] for r in rows),'reduction_finish':max(r['r_end'] for r in rows)})
    traces.append(item)
result={'timings':timings,'checks':checks,'traces':traces,
        'trace_units':'same-GPU wall_clock64 ticks, not ms; communication includes signal/wait/reduction, not physical link busy time'}
(root/'analysis.json').write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps({'checks':checks,'timings':[{k:v for k,v in r.items() if k!='samples_ms'} for r in timings],
                  'traces':traces},indent=2))
