// Only for valid full feature tiles and scheduled K64 weight tiles.
// Hardware scalar offset is separate from the vector-offset bounds check.
// Do not reuse this for M-tail activation/output accesses or terminal prefetch.
__device__ __forceinline__ void h3_a_load_soffset(uintptr_t m0,
    const ck_tile::int32x4_t& resource,unsigned vector_offset,unsigned scalar_offset){
    const unsigned wave_m0=__builtin_amdgcn_readfirstlane(static_cast<unsigned>(m0));
    asm volatile("s_mov_b32 m0, %0\n\t"
                 "buffer_load_dwordx4 %1, %2, %3 offen lds"
                 :: "s"(wave_m0),"v"(vector_offset),"s"(resource),"s"(scalar_offset)
                 : "memory","m0");
}
