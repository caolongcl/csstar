#!/bin/bash

# 如果没有设置 DSG_BUILD_DEBUG 环境变量就默认是 Debug
DSG_BUILD_TYPE=${DSG_BUILD_TYPE:-"Debug"}

echo "-------------start build deps ($DSG_BUILD_TYPE)-------------"

if [ ${DSG_BUILD_TYPE} != "Debug" ] && [ ${DSG_BUILD_TYPE} != "Release" ];then
  echo "invalid DSG_BUILD_TYPE ${DSG_BUILD_TYPE}"
  exit -1
fi

script_path=$(cd "$(dirname "$0")" && pwd)

install_dir=${script_path}/jbcore/lib
pushd ${script_path}/deps

##### zxingcpp
target=zxingcpp
filename=zxing-cpp-2.2.1

if [ ! -d "tmp" ];then
    mkdir tmp
fi

if [ ! -d "./tmp/${filename}" ]; then
    tar -zxvf "${filename}.tar.gz" -C ./tmp
fi

pushd tmp
mkdir -p ${filename}/build
cmake -S ${filename} -B ${filename}/build -DCMAKE_BUILD_TYPE=${DSG_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=${install_dir}/${target} -DBUILD_SHARED_LIBS=OFF
cmake --build ${filename}/build -j8 --config ${DSG_BUILD_TYPE} --target install
popd

##### paho_mqtt_c
target=paho_mqtt_c
filename=paho.mqtt.c-1.3.13

if [ ! -d "./tmp/${filename}" ]; then
    tar -zxvf "${filename}.tar.gz" -C ./tmp
fi

pushd tmp
mkdir -p ${filename}/build
cmake -S ${filename} -B ${filename}/build -DCMAKE_BUILD_TYPE=${DSG_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=${install_dir}/${target} -DPAHO_BUILD_STATIC=TRUE -DPAHO_BUILD_SHARED=FALSE
cmake --build ${filename}/build -j8 --config ${DSG_BUILD_TYPE} --target install
popd

popd