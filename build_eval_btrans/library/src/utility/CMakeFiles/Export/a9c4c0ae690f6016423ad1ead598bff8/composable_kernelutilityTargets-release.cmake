#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "composable_kernel::utility" for configuration "Release"
set_property(TARGET composable_kernel::utility APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(composable_kernel::utility PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libutility.a"
  )

list(APPEND _cmake_import_check_targets composable_kernel::utility )
list(APPEND _cmake_import_check_files_for_composable_kernel::utility "${_IMPORT_PREFIX}/lib/libutility.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
