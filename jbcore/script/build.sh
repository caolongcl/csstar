#!/bin/bash

script_path=$(cd "$(dirname "$0")" && pwd)

pushd ${script_path}/../
mkdir -p build
pushd build
# cmake -DBUILD_SHARED_LIBS=OFF ..
cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release ..
# cmake --build . --target install
cmake --build .
popd
popd