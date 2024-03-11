#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "jbcore" for configuration "Debug"
set_property(TARGET jbcore APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(jbcore PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libjbcored.1.0.0.dylib"
  IMPORTED_SONAME_DEBUG "@rpath/libjbcored.1.dylib"
  )

list(APPEND _IMPORT_CHECK_TARGETS jbcore )
list(APPEND _IMPORT_CHECK_FILES_FOR_jbcore "${_IMPORT_PREFIX}/lib/libjbcored.1.0.0.dylib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
