"""Read-only reproducibility receipt; no GPU workload."""
from pathlib import Path
import hashlib,json,re,subprocess
root=Path(__file__).resolve().parent
def cmd(args):
    p=subprocess.run(args,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    return {'returncode':p.returncode,'output':p.stdout}
files=[root/n for n in ('reference_operators.hpp','two_kernel.hpp','bench.cpp','run.sh','controller.py','extended_controller.py','analyze.py','build/bench')]
files += [Path('/workspace/composable_kernel_publish_20261016/example_hcu/ck_tile/04_gemm/build/v08_ck_tile.so'),
          Path('/workspace/composable_kernel_publish_20261016/example_hcu/ck_tile/04_gemm/versions/v08_ck_tile/manifest.json'),
          Path('/opt/dtk/lib/librccl.so'),Path('/opt/dtk/dushmem/lib/libdushmem_host.so'),Path('/opt/dtk/dushmem/lib/libdushmem_device.a')]
data={'hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in files},
      'compiler':cmd(['/opt/dtk/bin/hipcc','--version']),
      'ck_revision':cmd(['git','-C','/workspace/composable_kernel','rev-parse','HEAD']),
      'published_ck_revision':cmd(['git','-C','/workspace/composable_kernel_publish_20261016','rev-parse','HEAD']),
      'sglang_revision':cmd(['git','-C','/workspace/minmax_h3_test/sglang-das','rev-parse','HEAD']),
      'sglang_tracked_status':cmd(['git','-C','/workspace/minmax_h3_test/sglang-das','status','--porcelain','--untracked-files=no']),
      'clocks':cmd(['/opt/dtk/bin/rocm-smi','--showclocks']),
      'postflight':cmd(['/opt/dtk/bin/rocm-smi','--showuse','--showmeminfo','vram']),
      'resource_query':'hipFuncGetAttributes, rank0; private size is not by itself proof of register spills'}
log=(root/'smoke_c72r8b256.log').read_text(errors='replace')
data['kernel_resources']=[s for s in log.splitlines() if s.startswith('RESOURCES ')]
assert hashlib.sha256((root/'reference_operators.hpp').read_bytes()).hexdigest()=='b227eaba8627fe0ac4a91846d95fd08d000e232d9625c430d44c171465337cc2'
assert data['sglang_tracked_status']['output']=='',data['sglang_tracked_status']
(root/'environment.json').write_text(json.dumps(data,indent=2)+'\n')
print(json.dumps({'hashes':data['hashes'],'resources':data['kernel_resources'],'model_tracked_clean':True},indent=2))
