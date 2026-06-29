file(REMOVE_RECURSE
  "cppcheck-build"
  "fixits"
  "CMakeFiles/cppcheck"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/cppcheck.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
