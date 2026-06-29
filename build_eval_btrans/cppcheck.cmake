
        file(GLOB_RECURSE GSRCS  /workspace/composable_kernel-dev-github/library/src/*.cpp /workspace/composable_kernel-dev-github/library/src/*.hpp /workspace/composable_kernel-dev-github/library/src/*.cxx /workspace/composable_kernel-dev-github/library/src/*.c /workspace/composable_kernel-dev-github/library/src/*.h)
        set(CPPCHECK_COMMAND
            CPPCHECK_EXE-NOTFOUND
            -q
            # -v
            # --report-progress
            --force
            --cppcheck-build-dir=/workspace/composable_kernel-dev-github/build_eval_btrans/cppcheck-build
            --platform=native
            --template=gcc
            --error-exitcode=1
            -j 128
             -DCPPCHECK=1 -D__linux__=1
            
             -I/workspace/composable_kernel-dev-github/include -I/workspace/composable_kernel-dev-github/build_eval_btrans/include -I/workspace/composable_kernel-dev-github/library/include
            --enable=warning,style,performance,portability
            --inline-suppr
            --suppressions-list=/workspace/composable_kernel-dev-github/build_eval_btrans/cppcheck-supressions
             ${GSRCS}
        )
        string(REPLACE ";" " " CPPCHECK_SHOW_COMMAND "${CPPCHECK_COMMAND}")
        message("${CPPCHECK_SHOW_COMMAND}")
        execute_process(
            COMMAND ${CPPCHECK_COMMAND}
            WORKING_DIRECTORY /workspace/composable_kernel-dev-github
            RESULT_VARIABLE RESULT
        )
        if(NOT RESULT EQUAL 0)
            message(FATAL_ERROR "Cppcheck failed")
        endif()
