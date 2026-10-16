// Private structured K64 experiment. No changes to shared CK headers.
// 14-bit power-of-two stride + row remainder; bit62 OFF/ON is paired.
__device__ __forceinline__ unsigned h3_struct_b64_stride(int k){
    return 1u<<(31-__builtin_clz(static_cast<unsigned>(k*2)));
}
template<int Cache>
__device__ __forceinline__ ck_tile::int32x4_t h3_struct_b64_resource(
    const void* input,int rows,int k){
    static_assert(Cache==0 || Cache==1,"Paired cache flag only");
    const unsigned stride=h3_struct_b64_stride(k);
    auto res=ck_tile::make_wave_buffer_resource(input,static_cast<unsigned>(rows)*stride,stride);
    res[2]=rows;
    uint64_t address=ck_tile::bit_cast<uint64_t>(res.lo);
    if constexpr(!Cache)address&=~(uint64_t(1)<<62);
    res.lo=ck_tile::bit_cast<ck_tile::int32x2_t>(address);
    return res;
}
__device__ __forceinline__ ck_tile::int32x4_t h3_b_rebase_k64(
    ck_tile::int32x4_t res,int tile,int k){
    // Update only pointer bits; preserve encoded stride and cache flag.
    constexpr uint64_t mask=(uint64_t(1)<<48)-1;
    const uint64_t descriptor=ck_tile::bit_cast<uint64_t>(res.lo);
    const uint64_t pointer=((descriptor&mask)+static_cast<unsigned>(tile*128))&mask;
    res.lo=ck_tile::bit_cast<ck_tile::int32x2_t>((descriptor&~mask)|pointer);
    // Record count is logical rows, not bytes. The unused terminal bank must
    // never read beyond the input. All its idxen loads are masked with range0.
    res[2]=tile<k/64 ? res[2] : 0;
    return res;
}
template<int Phase>
__device__ __forceinline__ ck_tile::int32x4_t h3_b_load_k64_immediate(
    ck_tile::int32x4_t res,unsigned row,int lane_k,int k){
    static_assert(Phase==0 || Phase==1,"K32 half within complete K64 bank");
    const unsigned remainder=static_cast<unsigned>(k*2)-h3_struct_b64_stride(k);
    const ck_tile::int32x2_t offsets={static_cast<int>(row),static_cast<int>(row*remainder+lane_k*2)};
    ck_tile::int32x4_t data;
    asm volatile("buffer_load_dwordx4 %0, %1, %2, 0 idxen offen offset:%3"
                 : "=v"(data) : "v"(offsets),"s"(res),"n"(Phase*64) : "memory");
    return data;
}
