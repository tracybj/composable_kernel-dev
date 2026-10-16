// Small assembly scheduling window, retaining four independent FP32 C tiles.
// Early-clobber outputs cannot alias any other input still used in the block.
template<bool High,bool SetPriority>
__device__ __forceinline__ void h3_mmac_group4(Acc& c0,Acc& c1,Acc& c2,Acc& c3,
    const V8& a0,const V8& a1,const V8& a2,const V8& a3,const V8& b){
    static_assert(sizeof(Acc)==16,"Four FP32 accumulator registers");
    const auto x0=ck_tile::bit_cast<ck_tile::int32x2_t>(High ? a0.hi : a0.lo);
    const auto x1=ck_tile::bit_cast<ck_tile::int32x2_t>(High ? a1.hi : a1.lo);
    const auto x2=ck_tile::bit_cast<ck_tile::int32x2_t>(High ? a2.hi : a2.lo);
    const auto x3=ck_tile::bit_cast<ck_tile::int32x2_t>(High ? a3.hi : a3.lo);
    const auto y=ck_tile::bit_cast<ck_tile::int32x2_t>(High ? b.hi : b.lo);
    if constexpr(SetPriority){
        asm volatile(
            "v_mmac_f32_16x16x16_bf16 %0, %4, %8, %0\n\t"
            "s_setprio 1\n\t"
            "v_mmac_f32_16x16x16_bf16 %1, %5, %8, %1\n\t"
            "v_mmac_f32_16x16x16_bf16 %2, %6, %8, %2\n\t"
            "v_mmac_f32_16x16x16_bf16 %3, %7, %8, %3"
            : "+&v"(c0),"+&v"(c1),"+&v"(c2),"+&v"(c3)
            : "v"(x0),"v"(x1),"v"(x2),"v"(x3),"v"(y) : "memory");
    }else{
        asm volatile(
            "v_mmac_f32_16x16x16_bf16 %0, %4, %8, %0\n\t"
            "v_mmac_f32_16x16x16_bf16 %1, %5, %8, %1\n\t"
            "v_mmac_f32_16x16x16_bf16 %2, %6, %8, %2\n\t"
            "v_mmac_f32_16x16x16_bf16 %3, %7, %8, %3"
            : "+&v"(c0),"+&v"(c1),"+&v"(c2),"+&v"(c3)
            : "v"(x0),"v"(x1),"v"(x2),"v"(x3),"v"(y) : "memory");
    }
}
