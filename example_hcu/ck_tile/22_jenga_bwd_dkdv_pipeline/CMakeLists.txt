add_example_executable(tile_example_jenga_bwd_dkdv jenga_bwd_dkdv.cpp)
set_target_properties(tile_example_jenga_bwd_dkdv PROPERTIES
    COMPILE_OPTIONS "--offload-arch=gfx936;-Xarch_gfx936;-mllvm=-enable-num-vgprs-768=true;-mllvm;-amdgpu-spill-vgpr-to-agpr=true"
)
