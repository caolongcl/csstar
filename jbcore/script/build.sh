#!/bin/bash

# 如果没有设置 DSG_BUILD_DEBUG 环境变量就默认是 Debug
DSG_BUILD_TYPE=${DSG_BUILD_TYPE:-"Debug"}

# 如果没有设置 DSG_BUILD_PLATFORM 环境变量就默认是编译为本机运行
DSG_BUILD_PLATFORM=${DSG_BUILD_PLATFORM:-"host"}

echo "-------------start build jbcore ($DSG_BUILD_TYPE) platform ${DSG_BUILD_PLATFORM}-------------"

if [ ${DSG_BUILD_TYPE} != "Debug" ] && [ ${DSG_BUILD_TYPE} != "Release" ];then
  echo "invalid DSG_BUILD_TYPE ${DSG_BUILD_TYPE}"
  exit -1
fi

if [ ${DSG_BUILD_PLATFORM} != "host" ] && [ ${DSG_BUILD_PLATFORM} != "arm" ];then
  echo "invalid DSG_BUILD_PLATFORM ${DSG_BUILD_PLATFORM}"
  exit -1
fi

script_path=$(cd "$(dirname "$0")" && pwd)
root_path=${script_path}/../

if [ ${DSG_BUILD_PLATFORM} = "arm" ];then
    DSG_CMAKE_TOOLCHAINS_PATH_CONFIG="-DCMAKE_TOOLCHAIN_FILE=${root_path}/../arm_toolchains.cmake"
fi

install_dir=${root_path}/out/${DSG_BUILD_PLATFORM}/${DSG_BUILD_TYPE}
target=jbcore
build_path=build/${DSG_BUILD_PLATFORM}/${DSG_BUILD_TYPE}

pushd ${root_path}

mkdir -p ${build_path}

cmake -S . -B ${build_path} \
  -DBUILD_SHARED_LIBS=OFF \
  -DCMAKE_BUILD_TYPE=${DSG_BUILD_TYPE} \
  -DDSG_BUILD_TYPE=${DSG_BUILD_TYPE} \
  -DDSG_BUILD_PLATFORM=${DSG_BUILD_PLATFORM} \
  -DCMAKE_INSTALL_PREFIX=${install_dir}/${target}

cmake --build ${build_path} -j8 --config ${DSG_BUILD_TYPE} --target install

popd