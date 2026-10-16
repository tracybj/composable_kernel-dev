"""Finite isolated two-kernel campaign. No H3, source mutation, or git writes."""
from pathlib import Path
import csv,json,statistics,subprocess,sys,traceback
root=Path(__file__).resolve().parent
configs=[(72,8,256),(72,8,512),(76,8,256),(80,8,256),(72,16,256),(80,16,256),(80,8,128)]
def key(c):return f'c{c[0]}r{c[1]}b{c[2]}'
def run(tag,c,rounds,kind,shape):
    path=root/(tag+'.log')
    if path.exists():
        assert 'PIPELINE_PASS' in path.read_text(errors='replace'),f'Incomplete existing {path}; inspect without rerun'
        print('REUSE',tag,flush=True);return
    print('START',tag,flush=True)
    with (root/(tag+'.controller.log')).open('w') as log:
        p=subprocess.run(['bash',str(root/'run.sh'),tag,*map(str,c),str(rounds),kind,shape],cwd=root,stdout=log,stderr=subprocess.STDOUT,timeout=660)
    assert p.returncode==0,(tag,p.returncode)
    assert 'PIPELINE_PASS' in path.read_text(errors='replace'),tag
    print('PASS',tag,flush=True)
def rows(tag):
    out=[json.loads(l) for l in (root/(tag+'.log')).read_text(errors='replace').splitlines() if l.startswith('{')]
    assert out and all(r['errors']==0 for r in out),tag
    return out
def trace_gate(c,k):
    reports=[]
    for rank in (0,1):
        path=root/f'trace_{k}_{key(c).replace("r","_r").replace("b","_b")}_two_overlap_rank{rank}.csv'
        rs=[{a:int(b) for a,b in r.items()} for r in csv.DictReader(path.open())]
        assert len(rs)==1554 and len({r['tile'] for r in rs})==1554,path
        assert all(0<r['g_start']<=r['g_end']<=r['r_start']<=r['peer_ready']<=r['r_end'] for r in rs),path
        groups={}
        for r in rs:groups.setdefault(r['producer'],[]).append(r)
        pairs=hits=0
        for group in groups.values():
            group.sort(key=lambda r:r['tile'])
            for a,b in zip(group,group[1:]):
                pairs+=1
                hits+=max(0,min(a['r_end'],b['g_end'])-max(a['r_start'],b['g_start']))>0
        early=min(r['r_start'] for r in rs)<max(r['g_end'] for r in rs)
        reports.append({'rank':rank,'communication_before_gemm_end':early,'adjacent_pairs':pairs,'adjacent_overlap_pairs':hits,
                        'qualifies':early and hits>0})
    return {'config':list(c),'k':k,'ranks':reports,'qualifies':all(r['qualifies'] for r in reports)}
try:
    run('gate_initial',(72,8,256),0,'gate','all')
    screens=[]
    for c in configs:
        tag='screen_'+key(c);run(tag,c,2,'full','all');data=rows(tag)
        for k in (7168,3584):
            ts={m:statistics.median(r['maxrank_ms'] for r in data if r['k']==k and r['mode']==m and r['round']>=0)
                for m in ('v08_rccl','single_serial80','single_pipeline72_8','two_serial','two_overlap','compute_only')}
            entry={'config':list(c),'k':k,'timings_ms':ts};screens.append(entry);print(json.dumps(entry),flush=True)
    selections={};evidence=[];traced=set()
    for k,shape in [(7168,'fc2'),(3584,'out')]:
        for item in sorted((s for s in screens if s['k']==k),key=lambda r:r['timings_ms']['two_overlap']):
            c=tuple(item['config'])
            if c not in traced:run('trace_'+key(c),c,1,'trace','all');traced.add(c)
            check=trace_gate(c,k);evidence.append(check);print('TRACE_GATE',json.dumps(check),flush=True)
            if check['qualifies']:
                selections[shape]={'config':list(c),'k':k,'screen':item,'trace':check};break
        if shape not in selections:
            raise RuntimeError(f'No measured configuration proves tile overlap for {shape}; preserve evidence, no adoption')
    (root/'selection.json').write_text(json.dumps({'screening':screens,'trace_checks':evidence,'selected':selections,
       'criterion':'Per shape minimum screening two_overlap time among candidates with both-rank early communication and adjacent-tile overlap. Screens are not acceptance.'},indent=2)+'\n')
    for shape,item in selections.items():
        c=tuple(item['config']);run('gate_selected_'+shape,c,0,'gate','all')
        for letter in 'abc':run('formal_'+shape+'_'+letter,c,3,'full',shape)
    with (root/'analysis.stdout.log').open('w') as out:
        subprocess.run([sys.executable,str(root/'analyze.py')],cwd=root,stdout=out,stderr=subprocess.STDOUT,check=True)
    (root/'COMPLETE').write_text('Isolated operator campaign complete. No H3, no git operations.\n')
    print('CAMPAIGN_COMPLETE',flush=True)
except BaseException:
    (root/'FAILED.txt').write_text(traceback.format_exc());traceback.print_exc();raise
