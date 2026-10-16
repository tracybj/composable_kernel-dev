#pragma once
namespace ck_tile {
template<class S> struct H3GroupedPartitioner:GemmTilePartitioner<S> {
    CK_TILE_DEVICE auto operator()(){
        const index_t mt=__builtin_amdgcn_readfirstlane(gridDim.x);
        const index_t nt=__builtin_amdgcn_readfirstlane(gridDim.y);
        const index_t flat=__builtin_amdgcn_readfirstlane(blockIdx.x+gridDim.x*blockIdx.y);
        const index_t group_span=H3_GROUP_M*nt;
        const index_t group=flat/group_span;
        const index_t first_m=group*H3_GROUP_M;
        const index_t group_m=(mt-first_m)<H3_GROUP_M?(mt-first_m):H3_GROUP_M;
        const index_t local=flat%group_span;
        const index_t m=first_m+local%group_m;
        const index_t n=local/group_m;
        return make_tuple(m*S::kM,n*S::kN);
    }
};
}
