"""Read-only environment/library/source receipt; no GPU workload."""
from pathlib import Path
import hashlib,json,subprocess
root=Path(__file__).resolve().parent
def cmd(args):
    p=subprocess.run(args,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    return {'returncode':p.returncode,'output':p.stdout}
files=[root/'operators.hpp',root/'bench.cpp',root/'run.sh',root/'build/bench',
       root/'vec8/operators.hpp',root/'vec8/bench.cpp',root/'vec8/run.sh',root/'vec8/controller.py',root/'vec8/build/bench',
       Path('/workspace/composable_kernel_publish_20261016/example_hcu/ck_tile/04_gemm/build/v08_ck_tile.so'),
       Path('/opt/dtk/lib/librccl.so'),Path('/opt/dtk/dushmem/lib/libdushmem_device.a'),Path('/opt/dtk/dushmem/lib/libdushmem_host.so')]
data={'hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in files},
      'compiler':cmd(['/opt/dtk/bin/hipcc','--version']),
      'ck_revision':cmd(['git','-C','/workspace/composable_kernel','rev-parse','HEAD']),
      'v08_revision':cmd(['git','-C','/workspace/composable_kernel_publish_20261016','rev-parse','HEAD']),
      'sglang_revision':cmd(['git','-C','/workspace/minmax_h3_test/sglang-das','rev-parse','HEAD']),
      'sglang_tracked_status':cmd(['git','-C','/workspace/minmax_h3_test/sglang-das','status','--porcelain','--untracked-files=no']),
      'postflight':cmd(['/opt/dtk/bin/rocm-smi','--showuse','--showmeminfo','vram']),
      'clocks':cmd(['/opt/dtk/bin/rocm-smi','--showclocks'])}
for label,binary in [('scalar',root/'build/bench'),('vec8',root/'vec8/build/bench')]:
    receipt=cmd(['/opt/dtk/llvm/bin/dccobjdump','--show-resource-usage','--inputs='+str(binary)])
    (root/(label+'_resources.txt')).write_text(receipt['output'])
    data[label+'_resource_command_returncode']=receipt['returncode']
(root/'environment.json').write_text(json.dumps(data,indent=2)+'\n')
print(json.dumps({'source_files':len(files),'model_tracked_status':data['sglang_tracked_status']['output'],
                  'resource_status':[data['scalar_resource_command_returncode'],data['vec8_resource_command_returncode']]},indent=2))
