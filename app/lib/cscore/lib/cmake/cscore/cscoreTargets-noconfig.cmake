#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "cscore" for configuration ""
set_property(TARGET cscore APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(cscore PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libcscore.1.0.0.dylib"
  IMPORTED_SONAME_NOCONFIG "@rpath/libcscore.1.dylib"
  )

list(APPEND _IMPORT_CHECK_TARGETS cscore )
list(APPEND _IMPORT_CHECK_FILES_FOR_cscore "${_IMPORT_PREFIX}/lib/libcscore.1.0.0.dylib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
