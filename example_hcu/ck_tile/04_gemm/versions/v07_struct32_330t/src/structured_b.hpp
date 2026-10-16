// Isolated alternative to CK's full-stride cacheB: native-style power-of-two
// stride plus row remainder in voffset. All record counts are logical rows.
__device__ __forceinline__ unsigned h3_b_stride(int k){
    return 1u<<(31-__builtin_clz(static_cast<unsigned>(k*2)));
}
__device__ __forceinline__ ck_tile::int32x4_t h3_b_resource(
    const void* input,int rows,int k,int bias_bytes){
    const unsigned stride=h3_b_stride(k);
    const auto ptr=reinterpret_cast<const void*>(reinterpret_cast<uintptr_t>(input)-bias_bytes);
    auto res=ck_tile::make_wave_buffer_resource(ptr,static_cast<unsigned>(rows)*stride,stride);
    res[2]=rows; // Do not use logical_byte_size/stride for a non-power-of-two K.
    return res;
}
__device__ __forceinline__ ck_tile::int32x4_t h3_b_load(
    ck_tile::int32x4_t res,int row,int kk,int k,int bias_bytes){
    const int index=kk<k ? row : res[2]; // terminal unused prefetch must be OOB
    const int offset=row*(k*2-h3_b_stride(k))+kk*2+bias_bytes;
    return ck_tile::bit_cast<ck_tile::int32x4_t>(
        ck_tile::amd_buffer_load_impl_inline_asm<ck_tile::bf16_t,8>(
            res,ck_tile::int32x2_t{index,offset}));
}
