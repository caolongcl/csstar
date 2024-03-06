#!/bin/bash

pushd app
mkdir -p build
pushd build
cmake ..
# cmake --build . --target install
cmake --build .
popd
popd