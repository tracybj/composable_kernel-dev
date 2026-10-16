"""Bounded isolated microbench campaign; never launches H3 or changes sources."""
from pathlib import Path
import json,subprocess,statistics,sys,traceback
root=Path(__file__).resolve().parent
def run(tag,comm,rounds,kind,shape):
    log=root/(tag+'.log')
    if log.exists():
        assert 'PIPELINE_PASS' in log.read_text(errors='replace'),f'Existing incomplete {log}; inspect, do not repeat'
        print('REUSE',tag,flush=True);return
    print('START',tag,flush=True)
    with (root/(tag+'.controller.log')).open('w') as out:
        p=subprocess.run(['bash',str(root/'run.sh'),tag,str(comm),str(rounds),kind,shape],cwd=root,stdout=out,stderr=subprocess.STDOUT,timeout=660)
    assert p.returncode==0,(tag,p.returncode)
    assert 'PIPELINE_PASS' in log.read_text(errors='replace'),tag
    print('PASS',tag,flush=True)
try:
    assert 'PIPELINE_PASS' in (root/'gate_c76r4.log').read_text(errors='replace')
    scores=[]
    for comm in (4,2,8,16,24):
        tag=f'screen_c{80-comm}r{comm}'
        run(tag,comm,2,'full','fc2')
        rows=[json.loads(l) for l in (root/(tag+'.log')).read_text(errors='replace').splitlines() if l.startswith('{')]
        assert all(r['errors']==0 for r in rows)
        vals=[r['maxrank_ms'] for r in rows if r['mode']=='pipeline_roles' and r['round']>=0]
        base=[r['maxrank_ms'] for r in rows if r['mode']=='v08_rccl' and r['round']>=0]
        scores.append({'comm':comm,'pipeline_median_ms':statistics.median(vals),'baseline_median_ms':statistics.median(base)})
        print(json.dumps(scores[-1]),flush=True)
    best=min(scores,key=lambda r:r['pipeline_median_ms'])['comm']
    (root/'selection.json').write_text(json.dumps({'screening':scores,'selected_comm':best,'criterion':'minimum FC2 pipeline median; screens not formal acceptance'},indent=2)+'\n')
    run('gate_selected',best,0,'gate','all')
    for tag in ('formal_a','formal_b','formal_c'):run(tag,best,3,'full','all')
    run('trace_selected',best,1,'trace','all')
    with (root/'analysis.stdout.log').open('w') as out:
        subprocess.run([sys.executable,str(root/'analyze.py')],cwd=root,stdout=out,stderr=subprocess.STDOUT,check=True)
    (root/'COMPLETE').write_text('All isolated operator stages passed. No H3 or git operations.\n')
    print('CAMPAIGN_COMPLETE',flush=True)
except BaseException:
    (root/'FAILED.txt').write_text(traceback.format_exc())
    traceback.print_exc();raise
