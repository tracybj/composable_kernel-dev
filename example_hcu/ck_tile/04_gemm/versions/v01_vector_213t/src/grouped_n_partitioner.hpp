#pragma once
namespace ck_tile {
template<class S> struct H3GroupedNPartitioner:GemmTilePartitioner<S> {
    CK_TILE_DEVICE auto operator()() {
        const index_t mt=__builtin_amdgcn_readfirstlane(gridDim.x);
        const index_t nt=__builtin_amdgcn_readfirstlane(gridDim.y);
        const index_t flat=__builtin_amdgcn_readfirstlane(blockIdx.x+gridDim.x*blockIdx.y);
        const index_t span=H3_GROUP_N*mt,group=flat/span,first=group*H3_GROUP_N;
        const index_t size=(nt-first)<H3_GROUP_N?(nt-first):H3_GROUP_N;
        const index_t local=flat%span;
        return make_tuple((local/size)*S::kM,(first+local%size)*S::kN);
    }
};
}
