// Keep MMAC's logical layout; vary physical LDS row stride only.
#pragma once
namespace ck_tile {
struct H3PaddedPolicy:H3GemmPolicy {
    template<class P> CK_TILE_HOST_DEVICE static constexpr auto MakeALdsBlockDescriptor(){
        constexpr index_t M=P::BlockGemmShape::kM,K=P::BlockGemmShape::kK;
        constexpr index_t V=H3GemmPolicy::template GetVectorLoadSize<P,typename P::ADataType,M>();
        return make_naive_tensor_descriptor(make_tuple(number<M>{},number<K>{}),make_tuple(number<K+H3_LDS_PAD>{},number<1>{}),number<V>{},number<1>{});
    }
    template<class P> CK_TILE_HOST_DEVICE static constexpr auto MakeBLdsBlockDescriptor(){
        constexpr index_t N=P::BlockGemmShape::kN,K=P::BlockGemmShape::kK;
        constexpr index_t V=H3GemmPolicy::template GetVectorLoadSize<P,typename P::BDataType,N>();
        return make_naive_tensor_descriptor(make_tuple(number<N>{},number<K>{}),make_tuple(number<K+H3_LDS_PAD>{},number<1>{}),number<V>{},number<1>{});
    }
    template<class P> CK_TILE_HOST_DEVICE static constexpr index_t GetSmemSize(){
        constexpr index_t A=sizeof(typename P::ADataType)*MakeALdsBlockDescriptor<P>().get_element_space_size();
        constexpr index_t B=sizeof(typename P::BDataType)*MakeBLdsBlockDescriptor<P>().get_element_space_size();
        return integer_divide_ceil(A,16)*16+B;
    }
};
}
