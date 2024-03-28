set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CROSS_CHAIN_PATH "/root/workspace/todocker/toolchains/gcc-11.1.0-20210608-sigmastar-glibc-x86_64_arm-linux-gnueabihf")
message(STATUS "${CROSS_CHAIN_PATH}")
set(CMAKE_FIND_ROOT_PATH "${CROSS_CHAIN_PATH}")
set(CMAKE_SYSROOT "${CROSS_CHAIN_PATH}/arm-linux-gnueabihf/libc")

set(CMAKE_C_COMPILER "${CROSS_CHAIN_PATH}/bin/arm-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "${CROSS_CHAIN_PATH}/bin/arm-linux-gnueabihf-g++")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")