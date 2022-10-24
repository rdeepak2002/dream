#!/bin/bash
rm -rf cmake-build-debug-web
mkdir cmake-build-debug-web
cd cmake-build-debug-web
cmake .. -DCMAKE_TOOLCHAIN_FILE=../emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake
make