// Restricted activation path: whole K32 phases with K%64, kk aligned8,
// common=(kk&~31)*2 is uniform per wave. Voffset retains ALL token rows.
// Scalar K offset is excluded from MUBUF bounds checking: invalidate the
// unused terminal bank, whose common>=K*2, to avoid last-row OOB reads.
// Not a generic arbitrary-column buffer load helper.
__device__ __forceinline__ ck_tile::int32x4_t h3_b_load_soffset(
    ck_tile::int32x4_t resource,unsigned row,int kk,int k){
    const unsigned common=__builtin_amdgcn_readfirstlane(static_cast<unsigned>(kk&~31)*2);
    const unsigned voffset=(row*static_cast<unsigned>(k)+static_cast<unsigned>(kk&31))*2;
    resource[2]=common<static_cast<unsigned>(k)*2 ? resource[2] : 0;
    ck_tile::int32x4_t data;
    asm volatile("buffer_load_dwordx4 %0, %2, %1, %3 offen"
                 : "=v"(data) : "s"(resource),"v"(voffset),"s"(common) : "memory");
    return data;
}
