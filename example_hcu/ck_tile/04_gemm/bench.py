"""Correctness-gated BF16 GEMM benchmark for the retained historical versions."""
import argparse
import ctypes
import datetime
import hashlib
import json
import os
from pathlib import Path
import statistics
import torch
import torch.nn.functional as F

ROOT=Path(__file__).resolve().parent
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--versions',default='v07_struct32_330t')
p.add_argument('--m',type=int,default=18880)
p.add_argument('--shapes',default='fc2,qkv,out_proj,fc1')
p.add_argument('--tag',required=True)
p.add_argument('--warmup',type=int,default=5)
p.add_argument('--rounds',type=int,default=6)
p.add_argument('--iterations',type=int,default=10)
a=p.parse_args()
out=ROOT/'build'/'runs'/a.tag
if out.exists():raise SystemExit('Refusing to overwrite '+str(out))
assert min(a.m,a.rounds,a.iterations)>0 and a.warmup>=0
torch.set_num_threads(4)
assert torch.cuda.is_available()
torch.cuda.set_device(0)
assert 'gfx936' in torch.cuda.get_device_properties(0).gcnArchName
names=a.versions.split(',')
libraries={}
metadata={}
for name in names:
    path=ROOT/'build'/(name+'.so')
    lib=ctypes.CDLL(str(path));fn=lib.h3_gemm
    fn.argtypes=[ctypes.c_void_p]*3+[ctypes.c_int]*3+[ctypes.c_void_p];fn.restype=ctypes.c_int
    libraries[name]=(lib,fn)
    metadata[name]=json.loads((ROOT/'build'/(name+'.json')).read_text())
    assert hashlib.sha256(path.read_bytes()).hexdigest()==metadata[name]['sha256']
out.mkdir(parents=True)
(out/'environment.json').write_text(json.dumps({
    'date':datetime.datetime.now().astimezone().isoformat(),'torch':torch.__version__,
    'hip':torch.version.hip,'device':str(torch.cuda.get_device_properties(0)),
    'HIP_VISIBLE_DEVICES':os.environ.get('HIP_VISIBLE_DEVICES'),
    'dtype':'BF16 X/W/Y, FP32 accumulator','layout':'Y[M,N] = X[M,K] @ W[N,K].T',
    'benchmark':'HIP events; rotating order; hot-cache; median of round means',
    'arguments':vars(a),'libraries':metadata},indent=2)+'\n')
shapes={'fc2':(5376,7168),'qkv':(10752,5376),'out_proj':(5376,3584),
        'fc1':(14336,5376),'square4096':(4096,4096)}
rows=[]
with torch.inference_mode():
    for index,shape in enumerate(a.shapes.split(',')):
        n,k=shapes[shape];m=a.m
        torch.manual_seed(1012+index)
        x=torch.empty((m,k),device='cuda',dtype=torch.bfloat16)
        w=torch.empty((n,k),device='cuda',dtype=torch.bfloat16)
        storage=torch.empty((m+16,n),device='cuda',dtype=torch.bfloat16);y=storage[:m]
        original=torch.empty_like(y)
        stream=torch.cuda.current_stream().cuda_stream
        def launch(fn):
            status=fn(x.data_ptr(),w.data_ptr(),y.data_ptr(),m,n,k,stream)
            if status:raise RuntimeError(f'h3_gemm failed status={status}, MNK={m,n,k}')
            return y
        checks={name:{} for name in names}
        for distribution in ['binary_fraction','normal']:
            if distribution=='binary_fraction':
                x.copy_(torch.randint(-3,4,x.shape,device='cuda',dtype=torch.int16)).mul_(1/8)
                w.copy_(torch.randint(-3,4,w.shape,device='cuda',dtype=torch.int16)).mul_(1/8)
            else:x.normal_(0,.25);w.normal_(0,.25)
            saved_x=x.clone();saved_w=w.clone()
            reference=torch.mm(x.float(),w.float().t()).to(torch.bfloat16)
            for name,(_,fn) in libraries.items():
                storage.fill_(float('nan'));launch(fn);torch.cuda.synchronize()
                error=(y.float()-reference.float()).abs()
                relative=float(torch.linalg.vector_norm(error)/torch.linalg.vector_norm(reference.float()))
                finite=bool(torch.isfinite(y).all());exact=bool(torch.equal(y,reference))
                guard=bool(torch.isnan(storage[m:]).all())
                unchanged=bool(torch.equal(x,saved_x) and torch.equal(w,saved_w))
                passed=finite and guard and unchanged and (exact if distribution=='binary_fraction' else
                    bool(torch.all(error<=.0625+.01*reference.float().abs())) and relative<.005)
                checks[name][distribution]={'passed':passed,'finite':finite,'exact':exact,
                    'guard_unchanged':guard,'inputs_unchanged':unchanged,
                    'max_abs':float(error.max()),'relative_l2':relative}
            del saved_x,saved_w,reference,error
        if not all(v['passed'] for c in checks.values() for v in c.values()):
            (out/'FAILED.json').write_text(json.dumps(checks,indent=2)+'\n')
            raise SystemExit('Correctness failed; no performance reported')
        calls={'rocblas_linear':lambda:F.linear(x,w),'rocblas_mm_out':lambda:torch.mm(x,w.t(),out=original)}
        calls.update({name:lambda fn=fn:launch(fn) for name,(_,fn) in libraries.items()})
        for call in calls.values():
            for _ in range(a.warmup):call()
        torch.cuda.synchronize()
        modes=list(calls);times={mode:[] for mode in modes}
        for ri in range(a.rounds):
            for mode in modes[ri%len(modes):]+modes[:ri%len(modes)]:
                begin,end=torch.cuda.Event(enable_timing=True),torch.cuda.Event(enable_timing=True)
                begin.record()
                for _ in range(a.iterations):calls[mode]()
                end.record();end.synchronize()
                times[mode].append(begin.elapsed_time(end)/a.iterations)
        timing={mode:{'rounds_ms':values,'median_ms':statistics.median(values),
                      'TFLOPS':2*m*n*k/(statistics.median(values)*1e9)} for mode,values in times.items()}
        rows.append({'shape':shape,'MNK':[m,n,k],'correctness':checks,'timing':timing})
        (out/'results.json').write_text(json.dumps(rows,indent=2)+'\n')
        print(json.dumps({'shape':shape,'timing':timing}),flush=True)
        del x,w,storage,y,original,calls
        torch.cuda.empty_cache()
print('BENCH_COMPLETE',flush=True)
