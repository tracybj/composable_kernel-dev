file(REMOVE_RECURSE
  "cppcheck-build"
  "fixits"
  "CMakeFiles/tidy-make-fixit-dir"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/tidy-make-fixit-dir.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
