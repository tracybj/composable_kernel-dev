# CMake generated Testfile for 
# Source directory: /workspace/composable_kernel-dev-github/example_hcu/08_batchnorm
# Build directory: /workspace/composable_kernel-dev-github/build_eval_btrans2/example_hcu/08_batchnorm
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(example_batchnorm_backward "/workspace/composable_kernel-dev-github/build_eval_btrans2/bin/example_batchnorm_backward")
set_tests_properties(example_batchnorm_backward PROPERTIES  _BACKTRACE_TRIPLES "/workspace/composable_kernel-dev-github/example_hcu/CMakeLists.txt;14;add_test;/workspace/composable_kernel-dev-github/example_hcu/08_batchnorm/CMakeLists.txt;4;add_example_executable;/workspace/composable_kernel-dev-github/example_hcu/08_batchnorm/CMakeLists.txt;0;")
add_test(example_batchnorm_forward_training "/workspace/composable_kernel-dev-github/build_eval_btrans2/bin/example_batchnorm_forward_training")
set_tests_properties(example_batchnorm_forward_training PROPERTIES  _BACKTRACE_TRIPLES "/workspace/composable_kernel-dev-github/example_hcu/CMakeLists.txt;14;add_test;/workspace/composable_kernel-dev-github/example_hcu/08_batchnorm/CMakeLists.txt;5;add_example_executable;/workspace/composable_kernel-dev-github/example_hcu/08_batchnorm/CMakeLists.txt;0;")
