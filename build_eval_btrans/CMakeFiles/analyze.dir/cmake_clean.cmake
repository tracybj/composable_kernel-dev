file(REMOVE_RECURSE
  "cppcheck-build"
  "fixits"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/analyze.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
