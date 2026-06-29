file(REMOVE_RECURSE
  "cppcheck-build"
  "fixits"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/tidy-base.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
