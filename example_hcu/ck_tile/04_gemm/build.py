"""Build one retained milestone from its exact recorded flags and source hashes."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parent
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--version', default='v07_struct32_330t')
parser.add_argument('--variant', default='default', choices=['default','structured','producer'])
parser.add_argument('--ck-root', type=Path, default=ROOT.parents[2])
parser.add_argument('--hipcc', default='/opt/dtk/bin/hipcc')
args = parser.parse_args()
manifest_path = ROOT/'versions'/args.version/'manifest.json'
manifest = json.loads(manifest_path.read_text())
record = manifest['variants'][args.variant]
source_root = manifest_path.parent/'src'
for relative, expected in manifest['source_sha256'].items():
    assert hashlib.sha256((source_root/relative).read_bytes()).hexdigest()==expected, relative
ck_root = args.ck_root.resolve()
assert (ck_root/'include/ck_tile/core.hpp').is_file(), ck_root
name=args.version + ('' if args.variant=='default' else '__'+args.variant)
out=ROOT/'build';out.mkdir(exist_ok=True)
target=out/(name+'.so')
command=[args.hipcc,'-O3','-std=c++17','--offload-arch=gfx936','-fPIC','-shared',
         '-I'+str(ck_root/'include'),str(source_root/record['entry']),'-o',str(target),*record['flags']]
revision=subprocess.check_output(['git','-C',str(ck_root),'rev-parse','HEAD'],text=True).strip()
if revision != manifest['experiment_ck_revision']:
    print('NOTICE: CK revision differs from original experiment; validating this checkout, not historical binary identity.',flush=True)
print(' '.join(command),flush=True)
with (out/(name+'.log')).open('w') as log:
    result=subprocess.run(command,stdout=log,stderr=subprocess.STDOUT)
metadata={'name':name,'config':record['config'],'command':command,'exit_code':result.returncode,
          'ck_revision':revision,'recorded_ck_revision':manifest['experiment_ck_revision'],
          'recorded_library_sha256':record['library_sha256']}
if result.returncode==0:
    metadata['sha256']=hashlib.sha256(target.read_bytes()).hexdigest()
(out/(name+'.json')).write_text(json.dumps(metadata,indent=2)+'\n')
if result.returncode:
    print((out/(name+'.log')).read_text(errors='replace')[-12000:])
    raise SystemExit(result.returncode)
print(json.dumps(metadata,indent=2),flush=True)
