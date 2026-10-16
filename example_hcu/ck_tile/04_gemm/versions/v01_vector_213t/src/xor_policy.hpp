// Lightweight pack XOR: preserve 8-element K vector packs, without the
// original policy's extra MN-layer split/merge.
#pragma once
namespace ck_tile {
struct H3XorPolicy:H3GemmPolicy {
    template<class P,class T,index_t MN> CK_TILE_HOST_DEVICE static constexpr auto Desc(){
        constexpr index_t K=P::BlockGemmShape::kK;
        constexpr index_t V=H3GemmPolicy::template GetVectorLoadSize<P,T,MN>();
        auto raw=make_naive_tensor_descriptor(make_tuple(number<K/V>{},number<MN>{},number<V>{}),make_tuple(number<V>{},number<K>{},number<1>{}),number<V>{},number<1>{});
        auto x=transform_tensor_descriptor(raw,
            make_tuple(make_xor_transform(make_tuple(number<MN>{},number<K/V>{})),make_pass_through_transform(number<V>{})),
            make_tuple(sequence<1,0>{},sequence<2>{}),make_tuple(sequence<1,0>{},sequence<2>{}));
        return transform_tensor_descriptor(x,
            make_tuple(make_pass_through_transform(number<MN>{}),make_merge_transform_v3_division_mod(make_tuple(number<K/V>{},number<V>{}))),
            make_tuple(sequence<1>{},sequence<0,2>{}),make_tuple(sequence<0>{},sequence<1>{}));
    }
    template<class P> CK_TILE_HOST_DEVICE static constexpr auto MakeALdsBlockDescriptor(){return Desc<P,typename P::ADataType,P::BlockGemmShape::kM>();}
    template<class P> CK_TILE_HOST_DEVICE static constexpr auto MakeBLdsBlockDescriptor(){return Desc<P,typename P::BDataType,P::BlockGemmShape::kN>();}
    template<class P> CK_TILE_HOST_DEVICE static constexpr index_t GetSmemSize(){
        constexpr index_t A=sizeof(typename P::ADataType)*MakeALdsBlockDescriptor<P>().get_element_space_size();
        constexpr index_t B=sizeof(typename P::BDataType)*MakeBLdsBlockDescriptor<P>().get_element_space_size();
        return integer_divide_ceil(A,16)*16+B;
    }
};
}
