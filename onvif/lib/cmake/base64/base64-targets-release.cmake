#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "aklomp::base64" for configuration "Release"
set_property(TARGET aklomp::base64 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(aklomp::base64 PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libbase64.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS aklomp::base64 )
list(APPEND _IMPORT_CHECK_FILES_FOR_aklomp::base64 "${_IMPORT_PREFIX}/lib/libbase64.a" )

# Import target "aklomp::base64-bin" for configuration "Release"
set_property(TARGET aklomp::base64-bin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(aklomp::base64-bin PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/base64"
  )

list(APPEND _IMPORT_CHECK_TARGETS aklomp::base64-bin )
list(APPEND _IMPORT_CHECK_FILES_FOR_aklomp::base64-bin "${_IMPORT_PREFIX}/bin/base64" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
