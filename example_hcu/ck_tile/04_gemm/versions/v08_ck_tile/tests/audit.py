"""Independent v07/v08 bitwise, boundary, determinism and API-contract audit."""
import argparse
import ctypes
import hashlib
import json
from pathlib import Path
import torch

root = Path(__file__).resolve().parents[3]
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--tag', required=True)
a = p.parse_args()
out = root/'build/runs'/a.tag
assert not out.exists(), 'Use a fresh audit tag'
out.mkdir(parents=True)
torch.set_num_threads(4)
torch.cuda.set_device(0)
assert 'gfx936' in torch.cuda.get_device_properties(0).gcnArchName
torch.backends.cuda.matmul.allow_tf32 = False
libraries = {}
functions = {}
paths = {}
for mode, candidate, symbol in [
    ('v07','v07_struct32_330t','h3_gemm'),
    ('v08','v08_ck_tile','h3_gemm'),
    ('v07_structured','v07_struct32_330t__structured','h3_gemm'),
    ('v07_producer','v07_struct32_330t__producer','h3_gemm'),
    ('v08_structured','v08_ck_tile','h3_gemm_structured'),
    ('v08_producer','v08_ck_tile','h3_gemm_producer'),
]:
    path = root/'build'/(candidate+'.so')
    lib = ctypes.CDLL(str(path))
    fn = getattr(lib,symbol)
    fn.argtypes = [ctypes.c_void_p]*3+[ctypes.c_int]*3+[ctypes.c_void_p]
    fn.restype = ctypes.c_int
    libraries[mode], functions[mode], paths[mode] = lib, fn, path
hashes = {name:hashlib.sha256(path.read_bytes()).hexdigest() for name,path in paths.items()}
stream = torch.cuda.current_stream().cuda_stream
cases = [(1,256,64),(63,256,128),(64,512,256),(255,256,512),
         (256,512,768),(257,512,1024),(511,256,64),(513,512,256),
         (18880,5376,7168),(18880,10752,5376),(18880,5376,3584),(18880,14336,5376),
         (65,512,192),(129,256,320),(258,512,448),(513,256,704),(17,512,832),
         (1,5376,7168),(63,10752,5376),(257,5376,3584),(513,14336,5376),
         (31,256,8192),(17,512,16384),(4096,4096,4096)]
rows = []

def run_case(mode, reference_mode, shape, offset, seed, kind, repeats):
    m,n,k = shape
    torch.manual_seed(seed)
    if kind == 'binary_fraction':
        x=torch.randint(-3,4,(m,k),device='cuda',dtype=torch.int16).to(torch.bfloat16).mul_(1/8)
        w=torch.randint(-3,4,(n,k),device='cuda',dtype=torch.int16).to(torch.bfloat16).mul_(1/8)
    else:
        x=torch.randn((m,k),device='cuda',dtype=torch.bfloat16).mul_(.25)
        w=torch.randn((n,k),device='cuda',dtype=torch.bfloat16).mul_(.25)
    if kind=='special':
        # BF16 bit patterns: infinities, signed zero, normal/subnormal and NaNs.
        patterns=torch.tensor([0, -32768, 1, 127, 16256, -16512, 32640, -128, 32705, -63],
                              device='cuda',dtype=torch.int16).view(torch.bfloat16)
        x.reshape(-1)[:10].copy_(patterns)
        w.reshape(-1)[20:30].copy_(patterns)
    saved_x=x.clone();saved_w=w.clone()
    storage=torch.empty(m*n+64+offset,device='cuda',dtype=torch.bfloat16)
    y=storage[offset:offset+m*n].view(m,n)
    expected=torch.empty_like(y)
    assert functions[reference_mode](x.data_ptr(),w.data_ptr(),expected.data_ptr(),m,n,k,stream)==0
    torch.cuda.synchronize()
    ref=None if kind=='special' else torch.mm(x.float(),w.float().t()).to(torch.bfloat16)
    previous=None
    repeat_rows=[]
    for repeat in range(repeats):
        storage.fill_(float('nan'))
        assert functions[mode](x.data_ptr(),w.data_ptr(),y.data_ptr(),m,n,k,stream)==0
        torch.cuda.synchronize()
        equal=bool(torch.equal(y.view(torch.int16),expected.view(torch.int16)))
        guard=bool(torch.isnan(storage[:offset]).all() and torch.isnan(storage[offset+m*n:]).all())
        deterministic=previous is None or bool(torch.equal(previous,y.view(torch.int16)))
        finite=bool(torch.isfinite(y).all())
        numeric=True
        relative=None
        if ref is not None:
            delta=(y.float()-ref.float()).abs()
            relative=float(torch.linalg.vector_norm(delta)/torch.linalg.vector_norm(ref.float()))
            numeric=bool(torch.equal(y,ref)) if kind=='binary_fraction' else (
                bool(torch.all(delta<=.0625+.01*ref.float().abs())) and relative<.005)
        repeat_rows.append({'exact_v07_bits':equal,'guard':guard,'deterministic':deterministic,
            'finite':finite,'relative_l2':relative,'passed':equal and guard and deterministic and numeric and (finite or kind=='special')})
        if previous is None:previous=y.view(torch.int16).clone()
    unchanged=bool(torch.equal(x.view(torch.int16),saved_x.view(torch.int16)) and
                   torch.equal(w.view(torch.int16),saved_w.view(torch.int16)))
    row={'mode':mode,'reference':reference_mode,'MNK':list(shape),'output_offset_bf16':offset,
         'seed':seed,'kind':kind,'repeats':repeat_rows,'inputs_unchanged':unchanged,
         'passed':unchanged and all(r['passed'] for r in repeat_rows)}
    rows.append(row)
    (out/'results.json').write_text(json.dumps(rows,indent=2)+'\n',encoding='utf-8')
    assert row['passed'],row

with torch.inference_mode():
    for offset in [0,1]:
        for shape in cases:
            for seed in [1013,2013,3013]:
                for kind in ['binary_fraction','normal']:
                    run_case('v08','v07',shape,offset,seed,kind,3)
            print('AUDIT_SHAPE_PASS',offset,*shape,flush=True)
    # Direct structured APIs, including the shape used in the 4096^3 report.
    for mode in ['structured','producer']:
        for shape in [(1,256,64),(65,512,192),(257,5376,3584),(4096,4096,4096)]:
            for offset in [0,1]:
                run_case('v08_'+mode,'v07_'+mode,shape,offset,4113,'normal',3)
    for mode in ['','_structured','_producer']:
        for offset in [0,1]:
            run_case('v08'+mode,'v07'+mode,(65,256,192),offset,5113,'special',2)
    # Rejection equivalence uses allocated pointers and values rejected on host.
    x=torch.empty((256,256),device='cuda',dtype=torch.bfloat16);w=x.clone();y=x.clone()
    contracts=[]
    for m,n,k,dx in [(0,256,64,0),(-1,256,64,0),(1,0,64,0),(1,256,0,0),
                     (1,255,64,0),(1,256,65,0),(1,256,64,2)]:
        codes=[functions[mode](x.data_ptr()+dx,w.data_ptr(),y.data_ptr(),m,n,k,stream) for mode in ['v07','v08']]
        contracts.append({'MNK':[m,n,k],'input_offset_bytes':dx,'codes':codes,'passed':codes[0]==codes[1] and codes[0]!=0})
    for mode in ['structured','producer']:
        codes=[functions[v+'_'+mode](x.data_ptr(),w.data_ptr(),y.data_ptr(),1,256,8192,stream) for v in ['v07','v08']]
        contracts.append({'mode':mode,'K':8192,'codes':codes,'passed':codes[0]==codes[1] and codes[0]!=0})
    assert all(c['passed'] for c in contracts),contracts
summary={'passed':all(row['passed'] for row in rows),'groups':len(rows),
         'candidate_launches':sum(len(r['repeats']) for r in rows),'api_contracts':contracts,
         'library_sha256':hashes,'torch':torch.__version__,'hip':torch.version.hip,
         'device':str(torch.cuda.get_device_properties(0)),
         'purpose':'Correctness/determinism audit only; not performance evidence','rows':rows}
assert hashes=={name:hashlib.sha256(path.read_bytes()).hexdigest() for name,path in paths.items()}
(out/'summary.json').write_text(json.dumps(summary,indent=2)+'\n',encoding='utf-8')
print('AUDIT_COMPLETE',summary['groups'],summary['candidate_launches'],flush=True)
