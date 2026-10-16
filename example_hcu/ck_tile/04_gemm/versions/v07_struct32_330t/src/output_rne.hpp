// Bitwise equivalent to CK float_to_bf16_rtn_raw, including Inf/NaN payload
// handling. Do not replace it with finite-only rounding or canonical NaNs.
__device__ __forceinline__ unsigned short h3_rne_mask(float value){
    const unsigned u=ck_tile::bit_cast<unsigned>(value);
    const unsigned rounded=u+0x7fff+((u>>16)&1);
    const unsigned special=u|(unsigned((u&0xffff)!=0)<<16);
    const unsigned mask=0u-unsigned((u&0x7f800000)==0x7f800000);
    return static_cast<unsigned short>(((rounded&~mask)|(special&mask))>>16);
}
__device__ __forceinline__ unsigned short h3_rne_select(float value){
    const unsigned u=ck_tile::bit_cast<unsigned>(value);
    const unsigned rounded=u+0x7fff+((u>>16)&1);
    const unsigned special=u|(unsigned((u&0xffff)!=0)<<16);
    return static_cast<unsigned short>(((u&0x7f800000)==0x7f800000 ? special : rounded)>>16);
}
__device__ __forceinline__ unsigned short h3_rne_fpclass(float value){
    const unsigned u=ck_tile::bit_cast<unsigned>(value);
    const unsigned rounded=(u+0x7fff+((u>>16)&1))>>16;
    const unsigned special=(u>>16)|unsigned((u&0xffff)!=0);
    // Ordinary rounding leaves both infinities unchanged. Only NaNs require
    // CK's payload/sign preservation and low-payload nonzero handling.
    return static_cast<unsigned short>(__builtin_isnan(value) ? special : rounded);
}
