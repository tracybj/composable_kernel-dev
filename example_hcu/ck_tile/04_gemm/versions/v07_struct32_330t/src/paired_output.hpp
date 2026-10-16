// Pair adjacent feature lanes while giving even/odd lanes different token rows.
// All lanes execute the peer exchange; no divergent store predicate is needed.
template<bool UseDS>
__device__ __forceinline__ unsigned h3_feature_peer(unsigned source){
    unsigned peer;
    if constexpr(UseDS){
        // LLVM QUAD_PERM_ENC=0x8000 plus [1,0,3,2] selectors 0xb1.
        // Earlier probe failures were encoding mistakes, not HCU evidence.
        asm volatile("ds_swizzle_b32 %0, %1 offset:0x80b1"
                     : "=v"(peer) : "v"(source) : "memory");
        asm volatile("s_waitcnt lgkmcnt(0)" ::: "memory");
    }else{
        // Keep DPP visible to the machine scheduler/hazard recognizer. Raw
        // inline asm hid its VGPR/EXEC dependency latency and failed in GEMM.
        peer=static_cast<unsigned>(__builtin_amdgcn_mov_dpp(source,0xb1,0xf,0xf,true));
    }
    return peer;
}
template<bool UseDS>
__device__ __forceinline__ unsigned h3_feature_pair(unsigned first,unsigned second,int lane){
    const bool odd=lane&1;
    const unsigned own=odd ? second : first;
    const unsigned peer=h3_feature_peer<UseDS>(odd ? first : second);
    // Even lane: feature r/r+1 on first token. Odd lane: r-1/r on second.
    return odd ? peer|(own<<16) : own|(peer<<16);
}
__device__ __forceinline__ void h3_store_feature_pair(unsigned word,
    ck_tile::int32x4_t resource,unsigned voffset,int immediate,bool aligned){
    if(aligned){
        asm volatile("buffer_store_dword %0, %1, %2, 0 offen offset:%3"
                     :: "v"(word),"v"(voffset),"s"(resource),"n"(immediate) : "memory");
    }else{
        // Preserve the old BF16-aligned output domain without relying on an
        // unaligned dword store. Both token displacement and +2 stay in voffset.
        const unsigned low=word&0xffff,high=word>>16;
        asm volatile("buffer_store_short %0, %1, %2, 0 offen offset:%3"
                     :: "v"(low),"v"(voffset),"s"(resource),"n"(immediate) : "memory");
        asm volatile("buffer_store_short %0, %1, %2, 0 offen offset:%3"
                     :: "v"(high),"v"(voffset+2),"s"(resource),"n"(immediate) : "memory");
    }
}
