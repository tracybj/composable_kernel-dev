#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
export HIP_VISIBLE_DEVICES=4,5
export LD_LIBRARY_PATH=/opt/dtk/dushmem/lib:/opt/mpi/lib:/opt/dtk/lib:/usr/local/hyhal/lib:${LD_LIBRARY_PATH:-}
export DUSHMEM_SYMMETRIC_SIZE=1G OMP_NUM_THREADS=2 NCCL_DEBUG=WARN HSA_FORCE_FINE_GRAIN_PCIE=1
mkdir -p build
CK=${CK_ROOT:-/workspace/composable_kernel}
V08=${V08_ROOT:-$(cd ../../../../04_gemm && pwd)}
if [[ "${1:-}" == build ]];then
  /opt/dtk/bin/hipcc -O3 -std=c++17 --offload-arch=gfx936 -fgpu-rdc -mcode-object-version=4 \
    -DHIP_ENABLE_WARP_SYNC_BUILTINS -mllvm --enable-num-vgprs-768=true \
    -I"$CK/include" -I"$V08/versions/v08_ck_tile/src" -I/opt/dtk/dushmem/include -I/opt/mpi/include -I/opt/dtk/include \
    -c bench.cpp -o build/bench.o >build/compile.log 2>&1
  /opt/dtk/bin/hipcc --offload-arch=gfx936 -fgpu-rdc -mcode-object-version=4 -mllvm --enable-num-vgprs-768=true build/bench.o \
    -L/opt/dtk/dushmem/lib -ldushmem_device -ldushmem_host -L/opt/mpi/lib -lmpi -L/opt/dtk/lib -lrccl -ldl \
    -o build/bench >build/link.log 2>&1
  exit
fi
tag=${1:-run_a};comm=${2:-4};rounds=${3:-3};kind=${4:-full};shape=${5:-fc2}
if [[ -e "$tag.log" ]];then echo "Refuse to overwrite $tag.log";exit 1;fi
if [[ "$kind" == trace ]] && compgen -G "trace_*_r${comm}_*.csv" >/dev/null;then echo "Refuse to overwrite existing traces";exit 1;fi
python3 - <<'PY'
import re,subprocess
s=subprocess.check_output(['/opt/dtk/bin/rocm-smi','--showmeminfo','vram','--showuse'],text=True)
for gpu in (4,5):
    m=re.search(rf'HCU\[{gpu}\].*vram Total Used Memory \(MiB\):\s*(\d+)',s)
    u=re.search(rf'HCU\[{gpu}\].*HCU use \(%\):\s*([\d.]+)',s)
    assert m and int(m[1])<128 and u and float(u[1])==0,f'HCU{gpu} occupied'
PY
timeout --signal=TERM --kill-after=15s 600s /opt/mpi/bin/mpirun --allow-run-as-root --bind-to none \
  --mca coll_hcoll_enable 0 --mca pml ob1 --mca btl self,vader,tcp -np 2 \
  -x HIP_VISIBLE_DEVICES -x LD_LIBRARY_PATH -x DUSHMEM_SYMMETRIC_SIZE -x OMP_NUM_THREADS -x NCCL_DEBUG -x HSA_FORCE_FINE_GRAIN_PCIE \
  ./build/bench "$V08/build/v08_ck_tile.so" "$comm" "$rounds" "$kind" "$shape" 2>&1 | tee "$tag.log"
