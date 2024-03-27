#!/bin/bash

script_path=$(cd "$(dirname "$0")" && pwd)

pushd ${script_path}/../
mkdir -p build/debug
pushd build/debug
cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Debug ../..
cmake --build . --target install

popd
popd