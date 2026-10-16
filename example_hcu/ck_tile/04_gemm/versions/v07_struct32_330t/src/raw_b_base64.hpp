// Rebase once for a whole K64 bank, keeping K32 subphase in voffset.
// Contract: 0<=tile<=K/64 and resource spans at least one remaining token row.
// Thus common<=K*2<=bound even for the unused terminal bank; no clamp/underflow.
__device__ __forceinline__ ck_tile::int32x4_t h3_b_rebase_k64(
    ck_tile::int32x4_t resource,int tile){
    const unsigned common=__builtin_amdgcn_readfirstlane(static_cast<unsigned>(tile*128));
    const uint64_t pointer=ck_tile::bit_cast<uint64_t>(resource.lo)+common;
    resource.lo=ck_tile::bit_cast<ck_tile::int32x2_t>(pointer);
    resource[2]-=common;
    return resource;
}

// Only within an already rebased complete K64 bank. The checked vector
// displacement retains token row and lane; K32 half is an immediate 0/64.
// Resource bound remains shrunk by the K64 common displacement, including
// the unused terminal bank. This is not scalar-offset bounds bypass.
template<int Phase>
__device__ __forceinline__ ck_tile::int32x4_t h3_b_load_k64_immediate(
    ck_tile::int32x4_t resource,unsigned row,int lane_k,int k){
    static_assert(Phase==0 || Phase==1,"Two halves of a complete K64 bank only");
    const unsigned voffset=(row*static_cast<unsigned>(k)+static_cast<unsigned>(lane_k))*2;
    ck_tile::int32x4_t data;
    asm volatile("buffer_load_dwordx4 %0, %1, %2, 0 offen offset:%3"
                 : "=v"(data) : "v"(voffset),"s"(resource),"n"(Phase*64) : "memory");
    return data;
}
