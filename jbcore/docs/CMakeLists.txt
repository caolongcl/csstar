cmake_minimum_required(VERSION 3.0.0)

# 设置工程名字 版本 以及语言
project(csdemo VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# debug 生成的可执行文件名会多一个 d，可注释掉
set(CMAKE_DEBUG_POSTFIX d)

# 设置静态库和动态库生成路径，都为工程内目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

# 添加子目录，子目录下必须有 CMakeLists.txt 才会被包含
add_subdirectory(src)

# 声明测试 sdk 库的代码
add_executable(${PROJECT_NAME} main.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# 链接 sdk 库
target_link_libraries(${PROJECT_NAME} PRIVATE cscore)

# 设置安装路径为 bin
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# 设置 sdk 库安装路径
# if(APPLE)
#   set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
#   message(STATUS "@executable_path/../lib")
# elseif(UNIX)
#   set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
#   message(STATUS "$ORIGIN/../lib")
# endif()
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../app/third_party/cscore")

# 设置安装配置
include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${CSDemo_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${CSDemo_VERSION_MINOR}")
set(CPACK_SOURCE_GENERATOR "TGZ")
include(CPack)

# cscoreTargets 是 sdk/CMakeLists.txt 中定义的
# 安装 cscoreTargets.cmake，其他要使用 cscore 库的程序可以方便的集成
install(EXPORT cscoreTargets
  FILE cscoreTargets.cmake
  DESTINATION lib/cmake/cscore)

# 设置配置文件
include(CMakePackageConfigHelpers)
# generate the config file that is includes the exports
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/cscoreConfig.cmake"
  INSTALL_DESTINATION "lib/cmake/example"
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
  )
# generate the version file for the config file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/cscoreConfigVersion.cmake"
  VERSION "${CSDemo_VERSION_MAJOR}.${CSDemo_VERSION_MINOR}"
  COMPATIBILITY AnyNewerVersion
)

# 安装配置文件到 lib/cmake/cscore
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/cscoreConfig.cmake
  DESTINATION lib/cmake/cscore
)

export(EXPORT cscoreTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/cscoreTargets.cmake"
)