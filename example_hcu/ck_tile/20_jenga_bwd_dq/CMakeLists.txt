add_example_executable(tile_example_jenga_bwd_dq example_jenga_bwd_dq.cpp)
target_sources(tile_example_jenga_bwd_dq PRIVATE jenga_bwd_dq.cpp)
target_include_directories(tile_example_jenga_bwd_dq PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/)
target_compile_options(tile_example_jenga_bwd_dq PRIVATE -Wno-undefined-func-template -Wno-float-equal -save-temps)

