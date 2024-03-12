#!/bin/bash

script_path=$(cd "$(dirname "$0")" && pwd)

install_dir=${script_path}/jbcore/lib
pushd ${script_path}/deps

##### zxingcpp
target=zxingcpp
filename=zxing-cpp-2.2.1

if [ ! -d "./tmp/${filename}" ]; then
    tar -zxvf "${filename}.tar.gz" -C ./tmp
fi

pushd tmp
mkdir -p ${filename}/build
cmake -S ${filename} -B ${filename}/build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${install_dir}/${target} -DBUILD_SHARED_LIBS=OFF
cmake --build ${filename}/build -j8 --config Release --target install
popd

##### paho_mqtt_c
target=paho_mqtt_c
filename=paho.mqtt.c-1.3.13

if [ ! -d "./tmp/${filename}" ]; then
    tar -zxvf "${filename}.tar.gz" -C ./tmp
fi

pushd tmp
mkdir -p ${filename}/build
cmake -S ${filename} -B ${filename}/build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${install_dir}/${target} -DPAHO_BUILD_STATIC=TRUE -DPAHO_BUILD_SHARED=FALSE
cmake --build ${filename}/build -j8 --config Release --target install
popd

popd