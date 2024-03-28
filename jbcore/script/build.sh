#!/bin/bash

# 如果没有设置 DSG_BUILD_DEBUG 环境变量就默认是 Debug
DSG_BUILD_TYPE=${DSG_BUILD_TYPE:-"Debug"}

echo "-------------start build jbcore ($DSG_BUILD_TYPE)-------------"

if [ ${DSG_BUILD_TYPE} != "Debug" ] && [ ${DSG_BUILD_TYPE} != "Release" ];then
  echo "invalid DSG_BUILD_TYPE ${DSG_BUILD_TYPE}"
  exit -1
fi

script_path=$(cd "$(dirname "$0")" && pwd)

pushd ${script_path}/../
mkdir -p build/${DSG_BUILD_TYPE}
pushd build/${DSG_BUILD_TYPE}
cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=${DSG_BUILD_TYPE} ../..
cmake --build . --config ${DSG_BUILD_TYPE} --target install

popd
popd