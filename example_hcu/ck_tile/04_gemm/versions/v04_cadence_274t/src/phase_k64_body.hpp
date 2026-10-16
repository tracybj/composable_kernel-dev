// Included inside rolephase's compute-wave scope. Audited K64 activation
// prefetch experiment, using CK BF16 MMAC and the same protected A slots.
static_assert(WK==64 && MW==1 && NT==2 && H3_PHASE_ASM_B && H3_PHASE_BANK,
              "K64 B path requires audited MW1/raw-asm configuration");
static_assert(!H3_PHASE_SWAP && !H3_PHASE_MID_GROUP && !H3_PHASE_MID_SPLIT,
              "No phase/wave-group remapping in K64 B experiment");
static_assert(H3_PHASE_B64_PREFETCH_PHASE==0 || H3_PHASE_B64_PREFETCH_PHASE==1,
              "Future K64 tile issued in phase 0 or 1");
V8 bank64[2][2][NT], av64[MT];
#pragma unroll
for(int p=0;p<2;++p){
    #pragma unroll
    for(int j=0;j<NT;++j)
        bank64[0][p][j]=activation(res,wave_n*NT*16+row+j*16,p*32+klane,k);
}
// Two statically named register banks, not dynamically indexed local arrays.
// Lambda tags keep both consume bodies at compile-time fixed bank indices.
auto consume64 = [&](int tile,auto bank_tag){
    constexpr int current_bank=decltype(bank_tag)::value;
    constexpr int future_bank=1-current_bank;
    // Current K64 B bank must be complete; future stores from the preceding
    // tile may still have been in flight. No unproven vmcnt relaxation here.
    asm volatile("s_waitcnt vmcnt(0)" ::: "memory");
    __builtin_amdgcn_sched_barrier(0);
    ready_ack();
    const int offset=(tile%2)*(256*WK);
    #pragma unroll
    for(int phase=0;phase<2;++phase){
        #pragma unroll
        for(int i=0;i<MT;++i){
            if(phase==H3_PHASE_B64_PREFETCH_PHASE && i==H3_PHASE_MID_POINT){
                // Always issue future K64. Terminal prefetch is unused but its
                // destination registers remain alive until drained at exit.
                #pragma unroll
                for(int p=0;p<2;++p){
                    #pragma unroll
                    for(int j=0;j<NT;++j)
                        bank64[future_bank][p][j]=activation(res,
                            wave_n*NT*16+row+j*16,(tile+1)*64+p*32+klane,k);
                }
            }
            av64[i]=weight_fragment(shared+offset+phys(row+i*16,phase*32+klane));
        }
        if(phase==1)ready_ack(); // protect A slot reuse, exactly as K32 path
        __builtin_amdgcn_sched_barrier(0);
        #pragma unroll
        for(int j=0;j<NT;++j){
            #pragma unroll
            for(int i=0;i<MT;++i)
                MM{}(accum[j][i],av64[i].lo,bank64[current_bank][phase][j].lo);
        }
        #pragma unroll
        for(int j=0;j<NT;++j){
            #pragma unroll
            for(int i=0;i<MT;++i)
                MM{}(accum[j][i],av64[i].hi,bank64[current_bank][phase][j].hi);
        }
    }
};
for(int tile=0;tile<count;tile+=2){
    consume64(tile,ck_tile::number<0>{});
    if(tile+1<count)consume64(tile+1,ck_tile::number<1>{});
}
asm volatile("s_waitcnt vmcnt(0)" ::: "memory");
__builtin_amdgcn_sched_barrier(0);
asm volatile("" :: "v"(bank64[0][0][0]),"v"(bank64[0][0][1]),
                  "v"(bank64[0][1][0]),"v"(bank64[0][1][1]),
                  "v"(bank64[1][0][0]),"v"(bank64[1][0][1]),
                  "v"(bank64[1][1][0]),"v"(bank64[1][1][1]) : "memory");
