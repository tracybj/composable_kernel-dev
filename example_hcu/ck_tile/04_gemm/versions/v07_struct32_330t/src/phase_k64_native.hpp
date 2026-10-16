// Native two-barrier cadence, static wave groups and fixed two K64 B banks.
static_assert(H3_PHASE_NATIVE_CADENCE && H3_PHASE_STATIC_STAGGER &&
              H3_PHASE_PAIR_TILES && WK==64 && MW==1 && NT==2 &&
              H3_PHASE_ASM_B && H3_PHASE_BANK && !H3_PHASE_SWAP &&
              !H3_PHASE_CACHE_B,
              "K64 native B supports audited raw or pow2 structured addressing");
static_assert(H3_PHASE_NATIVE_B64_POINT==0 || H3_PHASE_NATIVE_B64_POINT==1,
              "Future bank is issued during phase 0 or 1");
static_assert(H3_PHASE_NATIVE_B64_PREFETCH_POINT>=-1 && H3_PHASE_NATIVE_B64_PREFETCH_POINT<16,
              "Native-bank prefetch must precede or lie within its DS16 phase");
static_assert(H3_PHASE_A_TWO_BASE<2,"Native B64 supports only compiler-tracked A loads");
constexpr int native_prefetch_point=H3_PHASE_NATIVE_B64_PREFETCH_POINT<0 ?
    prefetch_point : H3_PHASE_NATIVE_B64_PREFETCH_POINT;
#if H3_PHASE_NATIVE_B64_ROW_ORDER
static_assert((H3_PHASE_NATIVE_B64_ROW_ORDER==1 || H3_PHASE_NATIVE_B64_ROW_ORDER==2) &&
              H3_PHASE_NATIVE_B64_BASE64 && !H3_PHASE_NATIVE_B64_HOIST,
              "B row issue order requires private single-rebase K64 banks");
#endif
#if H3_PHASE_NATIVE_B64_BASE64
static_assert(!H3_PHASE_SCALAR_B_BASE && !H3_PHASE_POW2_B && !H3_PHASE_CACHE_B,
              "One K64 rebase must not be combined with another B addressing transform");
#endif
#if H3_PHASE_NATIVE_B64_PHASE_WAIT
static_assert(H3_PHASE_NATIVE_B64_POINT==1 && H3_PHASE_NATIVE_B64_LATE_WAIT &&
              !H3_PHASE_NATIVE_B64_END_WAIT && NT==2,
              "Phase waits require phase1 future issue and the audited four-load ordering");
#endif
#if H3_PHASE_NATIVE_B64_HOIST == 1
V8 native_a[MT];
#elif H3_PHASE_NATIVE_B64_HOIST == 2
V8 native_bank[2][2][NT], native_a[MT];
#pragma unroll
for(int phase=0;phase<2;++phase){
    #pragma unroll
    for(int j=0;j<NT;++j)native_bank[0][phase][j]=native_initial[phase][j];
}
#else
V8 native_bank[2][2][NT], native_a[MT];
#include "phase_k64_prepare.hpp"
#endif
auto consume_native64 = [&](int tile,auto slot_tag) __attribute__((always_inline)) {
    constexpr int slot=decltype(slot_tag)::value;
    constexpr int future=1-slot;
    constexpr int offset=slot*256*WK;
    // All four vectors of the current bank are from the preceding tile.
    // Drain once per K64 tile before issuing any future-bank loads.
#if !H3_PHASE_NATIVE_B64_LATE_WAIT && !H3_PHASE_NATIVE_B64_END_WAIT
    asm volatile("s_waitcnt vmcnt(0)" ::: "memory");
    __builtin_amdgcn_sched_barrier(0);
#endif
    #pragma unroll
    for(int phase=0;phase<2;++phase){
#if H3_PHASE_A_TWO_BASE == 1
        const BF* native_even=shared+offset+phys(row,phase*32+klane);
        const BF* native_odd=shared+offset+phys(row+16,phase*32+klane);
#endif
        #pragma unroll
        for(int i=0;i<MT;++i){
            if(phase==H3_PHASE_NATIVE_B64_POINT && i==native_prefetch_point){
#if H3_PHASE_NATIVE_B64_BASE64
                const auto future_res=h3_b_rebase_k64(res,tile+1
#if H3_PHASE_STRUCT_B64
                                                    ,k
#endif
                                                    );
#endif
#if H3_PHASE_NATIVE_B64_ROW_ORDER
                #pragma unroll
                for(int j=0;j<NT;++j){
                    ck_tile::static_for<0,2,1>{}([&](auto phase_tag){
                        constexpr int next_phase=decltype(phase_tag)::value;
#if H3_PHASE_NATIVE_B64_ROW_ORDER == 2
                        native_bank[future][next_phase][j]=ck_tile::bit_cast<V8>(
                            h3_b_load_k64_immediate<next_phase>(future_res,
                                wave_n*NT*16+row+j*16,klane,k));
#else
                        native_bank[future][next_phase][j]=activation(future_res,
                            wave_n*NT*16+row+j*16,next_phase*32+klane,k);
#endif
                    });
                }
#else
                #pragma unroll
                for(int next_phase=0;next_phase<2;++next_phase){
                    #pragma unroll
                    for(int j=0;j<NT;++j)
#if H3_PHASE_NATIVE_B64_BASE64
                        native_bank[future][next_phase][j]=activation(future_res,
                            wave_n*NT*16+row+j*16,next_phase*32+klane,k);
#else
                        native_bank[future][next_phase][j]=activation(res,
                            wave_n*NT*16+row+j*16,(tile+1)*64+next_phase*32+klane,k);
#endif
                }
#endif
            }
#if H3_PHASE_A_TWO_BASE == 1
            native_a[i]=weight_fragment((i%2 ? native_odd : native_even)+(i/2)*2048);
#else
            native_a[i]=weight_fragment(shared+offset+phys(row+i*16,phase*32+klane));
#endif
        }
#if H3_PHASE_NATIVE_B64_LATE_WAIT && !H3_PHASE_NATIVE_B64_END_WAIT
        if(phase==0){
#if H3_PHASE_NATIVE_B64_PHASE_WAIT
            // Current phase0 is the oldest two of four current-bank loads.
            // No future loads have yet been issued, so leave current phase1
            // pending rather than waiting for data not consumed in this phase.
#if H3_PHASE_NATIVE_B64_ROW_ORDER
            // Row-major order: p0j0, p1j0, p0j1, p1j1. Phase0 consumes
            // the first and THIRD load, so at most one newer vector may remain.
            asm volatile("s_waitcnt vmcnt(1)" ::: "memory");
#else
            asm volatile("s_waitcnt vmcnt(2)" ::: "memory");
#endif
#else
#if H3_PHASE_NATIVE_B64_POINT == 0
            static_assert(NT==2,"Exactly four newer raw B loads are issued");
            // Four next-bank loads were issued after all current-bank loads.
            // Complete the current bank, leaving only those newer loads.
            asm volatile("s_waitcnt vmcnt(4)" ::: "memory");
#else
            // No newer loads yet. Complete all four current-bank vectors.
            asm volatile("s_waitcnt vmcnt(0)" ::: "memory");
#endif
#endif
            __builtin_amdgcn_sched_barrier(0);
        }
#endif
#if H3_PHASE_NATIVE_B64_PHASE_WAIT
        if(phase==1){
            // Four future-bank loads are newer than the current phase1 pair.
            // Complete all older current data while retaining only those four
            // newer loads. Terminal drain and both bank lifetimes stay intact.
            asm volatile("s_waitcnt vmcnt(4)" ::: "memory");
            __builtin_amdgcn_sched_barrier(0);
        }
#endif
        if constexpr(stagger){
            __builtin_amdgcn_sched_barrier(0);
            asm volatile("s_waitcnt lgkmcnt(0)\n\ts_barrier" ::: "memory");
            __builtin_amdgcn_sched_barrier(0);
        }
        #pragma unroll
        for(int j=0;j<NT;++j){
            #pragma unroll
            for(int i=0;i<MT;++i){
                MM{}(accum[j][i],native_a[i].lo,native_bank[slot][phase][j].lo);
#if H3_PHASE_POSTPRIO
                if(j==0 && i==0)asm volatile("s_setprio 1" ::: "memory");
#endif
            }
        }
        #pragma unroll
        for(int j=0;j<NT;++j){
            #pragma unroll
            for(int i=0;i<MT;++i)
                MM{}(accum[j][i],native_a[i].hi,native_bank[slot][phase][j].hi);
        }
#if H3_PHASE_POSTPRIO
        asm volatile("s_setprio 0" ::: "memory");
#endif
        if constexpr(!stagger){
            __builtin_amdgcn_sched_barrier(0);
            asm volatile("s_barrier" ::: "memory");
        }
    }
#if H3_PHASE_NATIVE_B64_END_WAIT
    // Startup completes the initial current bank. For every subsequent tile,
    // this post-math drain completes its entire future K64 bank before the next
    // consume call uses it. The other bank is never consumed while pending.
    asm volatile("s_waitcnt vmcnt(0)" ::: "memory");
    __builtin_amdgcn_sched_barrier(0);
#endif
};
for(int tile=0;tile<count;tile+=2){
    consume_native64(tile,ck_tile::number<0>{});
    if(tile+1<count)consume_native64(tile+1,ck_tile::number<1>{});
}
// Keep every potentially pending destination live until the terminal drain.
asm volatile("s_waitcnt vmcnt(0)" ::: "memory");
__builtin_amdgcn_sched_barrier(0);
asm volatile("" :: "v"(native_bank[0][0][0]),"v"(native_bank[0][0][1]),
                  "v"(native_bank[0][1][0]),"v"(native_bank[0][1][1]),
                  "v"(native_bank[1][0][0]),"v"(native_bank[1][0][1]),
                  "v"(native_bank[1][1][0]),"v"(native_bank[1][1][1]) : "memory");
