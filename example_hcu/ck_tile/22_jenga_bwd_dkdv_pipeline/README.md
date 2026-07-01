# Jenga bwd dK/dV Pipeline

This folder contains the `tile_example_jenga_bwd_dkdv` example for ck_tile.

## Build

```bash
# from the repo root
mkdir -p build
cd build
sh ../script/cmake-ck-dev.sh ../ <arch>
cmake --build . --target tile_example_jenga_bwd_dkdv -j
```

This produces `build/bin/tile_example_jenga_bwd_dkdv`.

## Run

The example supports CPU validation and GPU timing.

### Correctness check

```bash
HSA_VISIBLE_DEVICES=7 ./bin/tile_example_jenga_bwd_dkdv \
  -b=1 -h=2 -n_q=256 -n_kv=256 -d=128 -m0=64 -n0=64 \
  -mask_type=random -k_active=10 \
  -warmup=0 -repeat=1 -timer=gpu -v=1
```

### Performance benchmark

```bash
DTK_VISIBLE_DEVICES=4 ./bin/tile_example_jenga_bwd_dkdv \
  -b=1 -h=40 -n_q=18048 -n_kv=18048 -d=128 -m0=64 -n0=64 \
  -mask_type=random -k_active=93 \
  -warmup=5 -repeat=10 -timer=gpu -v=0
```

### Simulating Python Benchmark Workloads (Wan2.1 / Wan2.2)

To match the performance workloads of the Python benchmark (using optimized block masks with sliding window attention):

#### Wan2.1 (seqlen=18048, top_k=28)
- Python benchmark mean active blocks/query: `84`
- C++ simulation command (diagonal window size `83`):
```bash
./bin/tile_example_jenga_bwd_dkdv \
  -b=1 -h=40 -n_q=18048 -n_kv=18048 -d=128 \
  -mask_type=synthetic -mask_radius=41 -text_blocks=0 \
  -warmup=5 -repeat=10 -timer=gpu -v=0
```

#### Wan2.2 (seqlen=75648, top_k=118)
- Python benchmark mean active blocks/query: `348`
- C++ simulation command (diagonal window size `347`):
```bash
./bin/tile_example_jenga_bwd_dkdv \
  -b=1 -h=40 -n_q=75648 -n_kv=75648 -d=128 \
  -mask_type=synthetic -mask_radius=173 -text_blocks=0 \
  -warmup=5 -repeat=10 -timer=gpu -v=0
```

## Notes

- `HSA_VISIBLE_DEVICES=7` matches the current local benchmark setup.
- The optimized full-tile path is used when the input shape and alignment satisfy the current host-side checks.
