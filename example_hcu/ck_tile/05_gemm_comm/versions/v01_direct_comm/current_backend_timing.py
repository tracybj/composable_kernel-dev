"""TP2 operator-only reference: installed LightOp W8A8 and v08 BF16, same RCCL."""
import ctypes
import hashlib
import json
import os
from pathlib import Path
import statistics
import torch
import torch.distributed as dist
from lightop import gemm_ops
from lightop.quant import per_token_quant_int8

root = Path(__file__).resolve().parent
rank = int(os.environ['LOCAL_RANK'])
torch.cuda.set_device(rank)
torch.set_num_threads(2)
dist.init_process_group('nccl')
libpath = Path('/code/composable_kernel_publish_20261016/example_hcu/ck_tile/04_gemm/build/v08_ck_tile.so')
lib = ctypes.CDLL(str(libpath))
gemm = lib.h3_gemm
gemm.argtypes = [ctypes.c_void_p]*3 + [ctypes.c_int]*3 + [ctypes.c_void_p]
gemm.restype = ctypes.c_int
summary = {'physical_gpus':[4,5], 'torch':torch.__version__, 'hip':torch.version.hip,
           'arch':torch.cuda.get_device_properties(rank).gcnArchName,
           'v08_sha256':hashlib.sha256(libpath.read_bytes()).hexdigest(), 'shapes':[]}
for k in (7168,3584):
    m,n=18880,5376
    torch.manual_seed(20261016+rank+k)
    x=torch.randn((m,k),device='cuda',dtype=torch.bfloat16)*0.25
    w=torch.randn((n,k),device='cuda',dtype=torch.bfloat16)/(k**0.5)
    ws=w.float().abs().amax(1,keepdim=True).clamp_min(1e-12)/127
    wq=(w.float()/ws).round().clamp(-128,127).to(torch.int8).t()
    aq,scale=per_token_quant_int8(x)
    out=torch.empty((m,n),device='cuda',dtype=torch.bfloat16)
    def bf16():
        status=gemm(x.data_ptr(),w.data_ptr(),out.data_ptr(),m,n,k,torch.cuda.current_stream().cuda_stream)
        assert status==0,status
        dist.all_reduce(out)
        return out
    def w8a8_quant():
        a,s=per_token_quant_int8(x)
        y=gemm_ops.triton_scaled_mm(a,wq,s,ws,out_dtype=torch.bfloat16)
        dist.all_reduce(y)
        return y
    def w8a8_prequant():
        y=gemm_ops.triton_scaled_mm(aq,wq,scale,ws,out_dtype=torch.bfloat16)
        dist.all_reduce(y)
        return y
    methods={'v08_rccl':bf16,'lightop_quant_gemm_rccl':w8a8_quant,'lightop_prequant_gemm_rccl':w8a8_prequant}
    samples={name:[] for name in methods}
    for name,fn in methods.items():
        for _ in range(5):y=fn()
        torch.cuda.synchronize()
        assert bool(torch.isfinite(y).all()),name
    names=list(methods)
    for r in range(6):
        for name in names[r%3:]+names[:r%3]:
            dist.barrier();torch.cuda.synchronize()
            start,end=torch.cuda.Event(enable_timing=True),torch.cuda.Event(enable_timing=True)
            start.record()
            for _ in range(5):y=methods[name]()
            end.record();end.synchronize()
            ms=torch.tensor([start.elapsed_time(end)/5],device='cuda')
            dist.all_reduce(ms,op=dist.ReduceOp.MAX)
            samples[name].append(float(ms.item()))
    item={'m':m,'n':n,'k':k,'timings':{name:{'median_ms':statistics.median(vals),'rounds_ms':vals} for name,vals in samples.items()}}
    summary['shapes'].append(item)
    if rank==0:print(json.dumps(item),flush=True)
    del x,w,ws,wq,aq,scale,out,y
    torch.cuda.empty_cache()
summary['loaded_libraries']=sorted(set(line.split()[-1] for line in Path('/proc/self/maps').read_text().splitlines()
   if any(key in line for key in ('librccl','gemm_w8a8_smooth','liblightop','v08_ck_tile'))))
if rank==0:
    (root/'current_backend.json').write_text(json.dumps(summary,indent=2)+'\n')
    print('CURRENT_BACKEND_PASS',flush=True)
dist.destroy_process_group()
