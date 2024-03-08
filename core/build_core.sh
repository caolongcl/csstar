#!/bin/bash

mkdir -p build
pushd build
# cmake -DBUILD_SHARED_LIBS=OFF ..
cmake -DBUILD_SHARED_LIBS=ON ..
# cmake --build . --target install
cmake --build .
popd