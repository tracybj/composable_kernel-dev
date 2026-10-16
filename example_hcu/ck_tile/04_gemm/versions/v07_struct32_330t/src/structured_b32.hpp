// Private K32 structured resource, paired bit62 OFF/ON. No shared CK edits.
__device__ __forceinline__ unsigned h3_struct_b32_stride(int k){
    return 1u<<(31-__builtin_clz(static_cast<unsigned>(k*2)));
}
template<int Cache>
__device__ __forceinline__ ck_tile::int32x4_t h3_struct_b32_resource(
    const void* input,int rows,int k){
    static_assert(Cache==0 || Cache==1,"Paired cache flag");
    const unsigned stride=h3_struct_b32_stride(k);
    auto res=ck_tile::make_wave_buffer_resource(input,static_cast<unsigned>(rows)*stride,stride);
    res[2]=rows;
    uint64_t lo=ck_tile::bit_cast<uint64_t>(res.lo);
    if constexpr(!Cache)lo&=~(uint64_t(1)<<62);
    res.lo=ck_tile::bit_cast<ck_tile::int32x2_t>(lo);
    return res;
}
__device__ __forceinline__ ck_tile::int32x4_t h3_struct_b32_rebase(
    ck_tile::int32x4_t res,int kk,int k){
    const unsigned common=__builtin_amdgcn_readfirstlane(static_cast<unsigned>(kk&~31)*2);
    constexpr uint64_t mask=(uint64_t(1)<<48)-1;
    const uint64_t descriptor=ck_tile::bit_cast<uint64_t>(res.lo);
    const uint64_t pointer=((descriptor&mask)+common)&mask;
    res.lo=ck_tile::bit_cast<ck_tile::int32x2_t>((descriptor&~mask)|pointer);
    // Bounds stay in logical rows. All terminal loads are masked, and the
    // caller drains VM0 at the final phase BEFORE consuming its current bank.
    res[2]=common<static_cast<unsigned>(k*2) ? res[2] : 0;
    return res;
}
__device__ __forceinline__ ck_tile::int32x4_t h3_struct_b32_load(
    ck_tile::int32x4_t res,int row,int kk,int k){
    res=h3_struct_b32_rebase(res,kk,k);
    const unsigned rem=static_cast<unsigned>(k*2)-h3_struct_b32_stride(k);
    const ck_tile::int32x2_t offsets={row,static_cast<int>(static_cast<unsigned>(row)*rem+(kk&31)*2)};
    ck_tile::int32x4_t data;
    asm volatile("buffer_load_dwordx4 %0, %1, %2, 0 idxen offen"
                 : "=v"(data) : "v"(offsets),"s"(res) : "memory");
    return data;
}
