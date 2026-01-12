#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "yaml-cpp::yaml-cpp" for configuration "Release"
set_property(TARGET yaml-cpp::yaml-cpp APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(yaml-cpp::yaml-cpp PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libyaml-cpp.so.0.8.0"
  IMPORTED_SONAME_RELEASE "libyaml-cpp.so.0.8"
  )

list(APPEND _IMPORT_CHECK_TARGETS yaml-cpp::yaml-cpp )
list(APPEND _IMPORT_CHECK_FILES_FOR_yaml-cpp::yaml-cpp "${_IMPORT_PREFIX}/lib/libyaml-cpp.so.0.8.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
