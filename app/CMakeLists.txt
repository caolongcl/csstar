cmake_minimum_required(VERSION 3.0.0)
project(csapp VERSION 0.1.0 LANGUAGES C CXX)

set(cscore_root ${CMAKE_CURRENT_SOURCE_DIR}/lib/cscore)

# 设置库 cmake 配置目录
set(cscore_DIR ${cscore_root}/lib/cmake/cscore)

# 通过上一步 cmake 配置找到 sdk 库
find_package(cscore REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

# 设置需要 sdk 暴露的头文件
target_include_directories(${PROJECT_NAME} PUBLIC ${cscore_root}/include)

# 链接 sdk 库
target_link_libraries(${PROJECT_NAME} cscore)

# set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}")
# install(TARGETS ${PROJECT_NAME} DESTINATION bin)
# file(GLOB_RECURSE libs "${cscore_root}/lib/libcscore*")
# install(FILES ${libs} DESTINATION bin/lib)
