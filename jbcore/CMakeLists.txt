cmake_minimum_required(VERSION 3.0.0)

project(jbdemo VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

if(CMAKE_BUILD_TYPE)
  message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
  if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    message(STATUS "Configuring for Debug build")
  elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    message(STATUS "Configuring for Release build")
  endif()
endif()

set(install_target_prefix jbcore)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/out/${install_target_prefix}")
set(CMAKE_DEBUG_POSTFIX d)

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(samples)

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${JBCORE_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${JBCORE_VERSION_MINOR}")
set(CPACK_SOURCE_GENERATOR "TGZ")
include(CPack)

install(EXPORT ${install_target_prefix}Targets
  FILE ${install_target_prefix}Targets.cmake
  DESTINATION lib/cmake/${install_target_prefix})

include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${install_target_prefix}Config.cmake"
  INSTALL_DESTINATION "lib/cmake/example"
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
  )
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${install_target_prefix}ConfigVersion.cmake"
  VERSION "${JBCORE_VERSION_MAJOR}.${JBCORE_VERSION_MINOR}"
  COMPATIBILITY AnyNewerVersion
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${install_target_prefix}Config.cmake
  DESTINATION lib/cmake/${install_target_prefix}
)

export(EXPORT ${install_target_prefix}Targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/${install_target_prefix}Targets.cmake"
)