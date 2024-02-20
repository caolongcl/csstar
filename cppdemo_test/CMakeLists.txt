cmake_minimum_required(VERSION 3.0.0)
project(cppdemotest VERSION 0.1.0 LANGUAGES C CXX)

set(sdk_root libsdk)

# 设置库 cmake 配置目录
set(sdk_DIR ${sdk_root}/lib/cmake/sdk)

# 通过上一步 cmake 配置找到 sdk 库
find_package(sdk REQUIRED)

add_executable(cppdemotest main.cpp)

# 设置需要 sdk 暴露的头文件
target_include_directories(cppdemotest PUBLIC ${sdk_root}/include)

# 链接 sdk 库
target_link_libraries(cppdemotest sdk)
