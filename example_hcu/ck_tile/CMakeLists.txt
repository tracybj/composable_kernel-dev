include_directories(AFTER
  ${CMAKE_CURRENT_LIST_DIR}
)

add_compile_options(-Wno-bit-int-extension)

# Only the required examples for Jenga
add_subdirectory(20_jenga_bwd_dq)
add_subdirectory(22_jenga_bwd_dkdv_pipeline)
add_subdirectory(23_jenga_preprocess)
add_subdirectory(24_jenga_lut)


