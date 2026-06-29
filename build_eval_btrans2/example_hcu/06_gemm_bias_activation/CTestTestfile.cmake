# CMake generated Testfile for 
# Source directory: /workspace/composable_kernel-dev-github/example_hcu/06_gemm_bias_activation
# Build directory: /workspace/composable_kernel-dev-github/build_eval_btrans2/example_hcu/06_gemm_bias_activation
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(example_gemm_bias_relu_mmac_fp16 "/workspace/composable_kernel-dev-github/build_eval_btrans2/bin/example_gemm_bias_relu_mmac_fp16")
set_tests_properties(example_gemm_bias_relu_mmac_fp16 PROPERTIES  _BACKTRACE_TRIPLES "/workspace/composable_kernel-dev-github/example_hcu/CMakeLists.txt;14;add_test;/workspace/composable_kernel-dev-github/example_hcu/06_gemm_bias_activation/CMakeLists.txt;4;add_example_executable;/workspace/composable_kernel-dev-github/example_hcu/06_gemm_bias_activation/CMakeLists.txt;0;")
add_test(example_gemm_bias_relu_mmac_fp16_wavelet "/workspace/composable_kernel-dev-github/build_eval_btrans2/bin/example_gemm_bias_relu_mmac_fp16_wavelet")
set_tests_properties(example_gemm_bias_relu_mmac_fp16_wavelet PROPERTIES  _BACKTRACE_TRIPLES "/workspace/composable_kernel-dev-github/example_hcu/CMakeLists.txt;14;add_test;/workspace/composable_kernel-dev-github/example_hcu/06_gemm_bias_activation/CMakeLists.txt;5;add_example_executable;/workspace/composable_kernel-dev-github/example_hcu/06_gemm_bias_activation/CMakeLists.txt;0;")
