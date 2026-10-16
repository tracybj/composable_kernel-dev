"""CPU/read-only environment receipt; run inside ck_yyc after the GPU job."""
from pathlib import Path
import hashlib,json,subprocess
root=Path(__file__).resolve().parent
def command(args):
    p=subprocess.run(args,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
    return {'exit_code':p.returncode,'output':p.stdout}
paths=[root/'bench.cpp',root/'bench_timing.cpp',root/'run.sh',root/'build/bench',root/'build/bench_gate',
       Path('/workspace/composable_kernel_publish_20261016/example_hcu/ck_tile/04_gemm/build/v08_ck_tile.so'),
       Path('/opt/dtk/lib/librccl.so'),Path('/opt/dtk/dushmem/lib/libdushmem_host.so'),
       Path('/opt/dtk/dushmem/lib/libdushmem_device.a')]
receipt={'hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in paths},
 'compiler':command(['/opt/dtk/bin/hipcc','--version']),
 'clocks':command(['/opt/dtk/bin/rocm-smi','--showclocks']),
 'postflight':command(['/opt/dtk/bin/rocm-smi','--showuse','--showmeminfo','vram']),
 'ck_revision':command(['git','-C','/workspace/composable_kernel','rev-parse','HEAD']),
 'publish_revision':command(['git','-C','/workspace/composable_kernel_publish_20261016','rev-parse','HEAD']),
 'sglang_revision':command(['git','-C','/workspace/minmax_h3_test/sglang-das','rev-parse','HEAD']),
 'sglang_tracked_status':command(['git','-C','/workspace/minmax_h3_test/sglang-das','status','--porcelain','--untracked-files=no'])}
(root/'environment.json').write_text(json.dumps(receipt,indent=2)+'\n')
print(json.dumps({'hashes':receipt['hashes']},indent=2))
