#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
source /code/minmax_h3_test/minimax_h3_bw151_repro_0518/benchmarks/vae_combo_20260929/runtime_env.sh
export HIP_VISIBLE_DEVICES=4,5
export OMP_NUM_THREADS=2
export HSA_FORCE_FINE_GRAIN_PCIE=1
unset H3_EXPERIMENT_DIR
export H3_REFERENCE_TAG=${1:-current_backend}
if [[ -e "$H3_REFERENCE_TAG.log" || -e "$H3_REFERENCE_TAG.json" ]];then echo "Use a fresh reference tag";exit 1;fi
timeout --signal=TERM --kill-after=15s 600s python3 -m torch.distributed.run \
  --nproc_per_node=2 --master_port=31387 current_backend.py >"$H3_REFERENCE_TAG.log" 2>&1
