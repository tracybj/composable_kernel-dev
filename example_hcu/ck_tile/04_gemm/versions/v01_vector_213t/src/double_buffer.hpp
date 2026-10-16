// Two LDS stages for the same CK Tile BF16 block/warp GEMM.
#pragma once
namespace ck_tile {
template<class Problem,class Policy> struct H3DoubleBufferPipeline
    : MmacGemmPipelineAGmemBGmemCRegV1<Problem,Policy> {
    using Base=MmacGemmPipelineAGmemBGmemCRegV1<Problem,Policy>;
    using A=typename Base::ADataType;
    using B=typename Base::BDataType;
    static constexpr index_t M=Base::kMPerBlock,N=Base::kNPerBlock,K=Base::kKPerBlock;
    CK_TILE_HOST_DEVICE static constexpr index_t GetStaticLdsSize(){return 2*Base::GetStaticLdsSize();}
    CK_TILE_HOST_DEVICE static constexpr index_t GetSmemSize(){return 2*Base::GetSmemSize();}
    template<class AW,class BW,class AF,class BF>
    CK_TILE_DEVICE auto operator()(const AW& aw,const AF& af,const BW& bw,const BF& bf,index_t loops,void* mem)const{
        constexpr auto ad=Policy::template MakeALdsBlockDescriptor<Problem>();
        constexpr auto bd=Policy::template MakeBLdsBlockDescriptor<Problem>();
        constexpr index_t a_bytes=integer_divide_ceil(sizeof(A)*ad.get_element_space_size(),16)*16;
        constexpr index_t stage_bytes=Base::GetStaticLdsSize();
        auto bytes=static_cast<char*>(mem);
        auto at0=make_tensor_view<address_space_enum::lds>(reinterpret_cast<A*>(bytes),ad);
        auto bt0=make_tensor_view<address_space_enum::lds>(reinterpret_cast<B*>(bytes+a_bytes),bd);
        auto at1=make_tensor_view<address_space_enum::lds>(reinterpret_cast<A*>(bytes+stage_bytes),ad);
        auto bt1=make_tensor_view<address_space_enum::lds>(reinterpret_cast<B*>(bytes+stage_bytes+a_bytes),bd);
        auto a0=make_tile_window(at0,make_tuple(number<M>{},number<K>{}),{0,0});
        auto b0=make_tile_window(bt0,make_tuple(number<N>{},number<K>{}),{0,0});
        auto a1=make_tile_window(at1,make_tuple(number<M>{},number<K>{}),{0,0});
        auto b1=make_tile_window(bt1,make_tuple(number<N>{},number<K>{}),{0,0});
        auto ag=make_tile_window(aw.get_bottom_tensor_view(),make_tuple(number<M>{},number<K>{}),aw.get_window_origin(),Policy::template MakeADramTileDistribution<Problem>());
        auto bg=make_tile_window(bw.get_bottom_tensor_view(),make_tuple(number<N>{},number<K>{}),bw.get_window_origin(),Policy::template MakeBDramTileDistribution<Problem>());
        constexpr auto block=Policy::template GetBlockGemm<Problem>();
        auto c=block.MakeCBlockTile();
        tile_elementwise_inout([](auto& x){x=0;},c);
        store_tile(a0,tile_elementwise_in(af,load_tile(ag)));
        store_tile(b0,tile_elementwise_in(bf,load_tile(bg)));
        move_tile_window(ag,{0,K});move_tile_window(bg,{0,K});
        h3_lds_barrier();
        while(true){
            if(loops==1){block(c,a0,b0);break;}
            {
                auto ar=load_tile(ag);auto br=load_tile(bg);
                block(c,a0,b0);
                store_tile(a1,tile_elementwise_in(af,ar));
                store_tile(b1,tile_elementwise_in(bf,br));
                move_tile_window(ag,{0,K});move_tile_window(bg,{0,K});
                --loops;h3_lds_barrier();
            }
            if(loops==1){block(c,a1,b1);break;}
            {
                auto ar=load_tile(ag);auto br=load_tile(bg);
                block(c,a1,b1);
                store_tile(a0,tile_elementwise_in(af,ar));
                store_tile(b0,tile_elementwise_in(bf,br));
                move_tile_window(ag,{0,K});move_tile_window(bg,{0,K});
                --loops;h3_lds_barrier();
            }
        }
        return block.MakeOuputLayout(c);
    }
    template<class AW,class BW>
    CK_TILE_DEVICE auto operator()(const AW& aw,const BW& bw,index_t loops,void* mem)const{
        return (*this)(aw,[](auto x){return x;},bw,[](auto x){return x;},loops,mem);
    }
};
}
