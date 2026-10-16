// LDS [K-pack, MN, packed-K] layout. Adjacent MMAC row lanes access
// adjacent vector packs rather than rows separated by an entire BK.
#pragma once
namespace ck_tile {
struct H3KPackPolicy:H3GemmPolicy {
    template<class P,index_t MN> CK_TILE_HOST_DEVICE static constexpr auto Desc(){
        constexpr index_t K=P::BlockGemmShape::kK,V=H3_LDS_KPACK;
        auto raw=make_naive_tensor_descriptor(make_tuple(number<K/V>{},number<MN>{},number<V>{}),make_tuple(number<MN*V>{},number<V>{},number<1>{}),number<V>{},number<1>{});
        return transform_tensor_descriptor(raw,
            make_tuple(make_pass_through_transform(number<MN>{}),make_merge_transform_v3_division_mod(make_tuple(number<K/V>{},number<V>{}))),
            make_tuple(sequence<1>{},sequence<0,2>{}),make_tuple(sequence<0>{},sequence<1>{}));
    }
    template<class P> CK_TILE_HOST_DEVICE static constexpr auto MakeALdsBlockDescriptor(){return Desc<P,P::BlockGemmShape::kM>();}
    template<class P> CK_TILE_HOST_DEVICE static constexpr auto MakeBLdsBlockDescriptor(){return Desc<P,P::BlockGemmShape::kN>();}
    template<class P> CK_TILE_HOST_DEVICE static constexpr index_t GetSmemSize(){
        return sizeof(typename P::ADataType)*P::BlockGemmShape::kM*P::BlockGemmShape::kK+sizeof(typename P::BDataType)*P::BlockGemmShape::kN*P::BlockGemmShape::kK;
    }
};
}
