#pragma once
namespace ck_tile {
template<class Problem,class Policy> struct H3DynamicDoublePipeline
    : MmacGemmPipelineAGmemBGmemCRegV1<Problem,Policy> {
    using Base=MmacGemmPipelineAGmemBGmemCRegV1<Problem,Policy>;
    using A=typename Base::ADataType;using B=typename Base::BDataType;
    static constexpr index_t M=Base::kMPerBlock,N=Base::kNPerBlock,K=Base::kKPerBlock;
    CK_TILE_HOST_DEVICE static constexpr index_t GetStaticLdsSize(){return 2*Base::GetStaticLdsSize();}
    CK_TILE_HOST_DEVICE static constexpr index_t GetSmemSize(){return 2*Base::GetSmemSize();}
    template<class AW,class BW,class AF,class BF>
    CK_TILE_DEVICE auto operator()(const AW& aw,const AF& af,const BW& bw,const BF& bf,index_t loops,void* mem)const{
        constexpr auto ad=Policy::template MakeALdsBlockDescriptor<Problem>();
        constexpr auto bd=Policy::template MakeBLdsBlockDescriptor<Problem>();
        constexpr index_t ab=integer_divide_ceil(sizeof(A)*ad.get_element_space_size(),16)*16;
        constexpr index_t stride=Base::GetStaticLdsSize();
        auto bytes=static_cast<char*>(mem);
        const auto make_a=[&](index_t off){return make_tile_window(make_tensor_view<address_space_enum::lds>(reinterpret_cast<A*>(bytes+off),ad),make_tuple(number<M>{},number<K>{}),multi_index<2>{0,0});};
        const auto make_b=[&](index_t off){return make_tile_window(make_tensor_view<address_space_enum::lds>(reinterpret_cast<B*>(bytes+off+ab),bd),make_tuple(number<N>{},number<K>{}),multi_index<2>{0,0});};
        auto ag=make_tile_window(aw.get_bottom_tensor_view(),make_tuple(number<M>{},number<K>{}),aw.get_window_origin(),Policy::template MakeADramTileDistribution<Problem>());
        auto bg=make_tile_window(bw.get_bottom_tensor_view(),make_tuple(number<N>{},number<K>{}),bw.get_window_origin(),Policy::template MakeBDramTileDistribution<Problem>());
        constexpr auto block=Policy::template GetBlockGemm<Problem>();
        auto c=block.MakeCBlockTile();tile_elementwise_inout([](auto& x){x=0;},c);
        auto initial_a=make_a(0);auto initial_b=make_b(0);
        store_tile(initial_a,tile_elementwise_in(af,load_tile(ag)));
        store_tile(initial_b,tile_elementwise_in(bf,load_tile(bg)));
        move_tile_window(ag,{0,K});move_tile_window(bg,{0,K});
        h3_lds_barrier();
        index_t current=0;
        while(loops>1){
            const index_t next=stride-current;
            auto ar=load_tile(ag);auto br=load_tile(bg);
            block(c,make_a(current),make_b(current));
            auto write_a=make_a(next);auto write_b=make_b(next);
            store_tile(write_a,tile_elementwise_in(af,ar));
            store_tile(write_b,tile_elementwise_in(bf,br));
            move_tile_window(ag,{0,K});move_tile_window(bg,{0,K});
            h3_lds_barrier();current=next;--loops;
        }
        block(c,make_a(current),make_b(current));
        return block.MakeOuputLayout(c);
    }
    template<class AW,class BW>
    CK_TILE_DEVICE auto operator()(const AW& aw,const BW& bw,index_t loops,void* mem)const{
        return (*this)(aw,[](auto x){return x;},bw,[](auto x){return x;},loops,mem);
    }
};
}
