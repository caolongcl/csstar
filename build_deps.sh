#!/bin/bash

# 如果没有设置 DSG_BUILD_DEBUG 环境变量就默认是 Debug
DSG_BUILD_TYPE=${DSG_BUILD_TYPE:-"Debug"}

# 如果没有设置 DSG_BUILD_PLATFORM 环境变量就默认是编译为本机运行
DSG_BUILD_PLATFORM=${DSG_BUILD_PLATFORM:-"host"}

echo "-------------start build deps ($DSG_BUILD_TYPE) platform ${DSG_BUILD_PLATFORM}-------------"

if [ ${DSG_BUILD_TYPE} != "Debug" ] && [ ${DSG_BUILD_TYPE} != "Release" ];then
  echo "invalid DSG_BUILD_TYPE ${DSG_BUILD_TYPE}"
  exit -1
fi

if [ ${DSG_BUILD_PLATFORM} != "host" ] && [ ${DSG_BUILD_PLATFORM} != "arm" ];then
  echo "invalid DSG_BUILD_PLATFORM ${DSG_BUILD_PLATFORM}"
  exit -1
fi

script_path=$(cd "$(dirname "$0")" && pwd)
root_path=${script_path}

if [ ${DSG_BUILD_PLATFORM} = "arm" ];then
    DSG_CMAKE_TOOLCHAINS_PATH_CONFIG="-DCMAKE_TOOLCHAIN_FILE=${root_path}/arm_toolchains.cmake"
fi

install_dir=${root_path}/jbcore/lib/${DSG_BUILD_PLATFORM}/${DSG_BUILD_TYPE}
pushd ${root_path}/deps

##### zxingcpp
target=zxingcpp
filename=zxing-cpp-2.2.1

if [ ! -d "tmp" ];then
    mkdir tmp
fi

if [ ! -d "./tmp/${filename}" ]; then
    tar -zxvf "${filename}.tar.gz" -C ./tmp
fi

build_path=${filename}/build/${DSG_BUILD_PLATFORM}/${DSG_BUILD_TYPE}

pushd tmp
mkdir -p ${build_path}
cmake -S ${filename} -B ${build_path} \
    -DCMAKE_BUILD_TYPE=${DSG_BUILD_TYPE} \
    -DCMAKE_INSTALL_PREFIX=${install_dir}/${target} \
    -DBUILD_SHARED_LIBS=OFF \
    ${DSG_CMAKE_TOOLCHAINS_PATH_CONFIG}
cmake --build ${build_path} -j8 --config ${DSG_BUILD_TYPE} --target install
popd

##### paho_mqtt_c
target=paho_mqtt_c
filename=paho.mqtt.c-1.3.13

if [ ! -d "./tmp/${filename}" ]; then
    tar -zxvf "${filename}.tar.gz" -C ./tmp
fi

build_path=${filename}/build/${DSG_BUILD_PLATFORM}/${DSG_BUILD_TYPE}

pushd tmp
mkdir -p ${build_path}

cmake -S ${filename} -B ${build_path} \
    -DCMAKE_BUILD_TYPE=${DSG_BUILD_TYPE} \
    -DCMAKE_INSTALL_PREFIX=${install_dir}/${target} \
    -DPAHO_BUILD_STATIC=TRUE \
    -DPAHO_BUILD_SHARED=FALSE \
    ${DSG_CMAKE_TOOLCHAINS_PATH_CONFIG}

cmake --build ${build_path} -j8 --config ${DSG_BUILD_TYPE} --target install
popd

popd