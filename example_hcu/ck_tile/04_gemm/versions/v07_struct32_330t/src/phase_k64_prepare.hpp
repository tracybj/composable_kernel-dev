// Shared preparation for the two static compute-wave bodies. It may be placed
// before the wave-group selection, but still after the producer/consumer split.
#if H3_PHASE_NATIVE_B64_HOIST == 1
V8 native_bank[2][2][NT];
#elif H3_PHASE_NATIVE_B64_HOIST == 2
// Only immutable initial payload crosses the group selection. Mutable future
// banks stay private to each inlined consume body, avoiding their capture.
V8 native_initial[2][NT];
#endif
#if H3_PHASE_NATIVE_B64_ROW_ORDER
static_assert(H3_PHASE_NATIVE_B64_BASE64 && !H3_PHASE_NATIVE_B64_HOIST && NT==2,
              "Row-major B issue only tested for private raw K64 banks");
#pragma unroll
for(int j=0;j<NT;++j){
    ck_tile::static_for<0,2,1>{}([&](auto phase_tag){
        constexpr int phase=decltype(phase_tag)::value;
#if H3_PHASE_NATIVE_B64_ROW_ORDER == 2
        native_bank[0][phase][j]=ck_tile::bit_cast<V8>(h3_b_load_k64_immediate<phase>(
            res,wave_n*NT*16+row+j*16,klane,k));
#else
        native_bank[0][phase][j]=activation(res,wave_n*NT*16+row+j*16,phase*32+klane,k);
#endif
    });
}
#else
#pragma unroll
for(int phase=0;phase<2;++phase){
    #pragma unroll
    for(int j=0;j<NT;++j)
#if H3_PHASE_NATIVE_B64_HOIST == 2
        native_initial[phase][j]=activation(res,wave_n*NT*16+row+j*16,phase*32+klane,k);
#else
        native_bank[0][phase][j]=activation(res,wave_n*NT*16+row+j*16,phase*32+klane,k);
#endif
}
#endif
// The same two matched startup barriers; complete the four initial B loads.
asm volatile("s_barrier\n\ts_waitcnt vmcnt(0)\n\ts_barrier" ::: "memory");
__builtin_amdgcn_sched_barrier(0);
