#!/bin/bash
rm -rf cmake-build-debug-web
mkdir cmake-build-debug-web

cd cmake-build-debug-web
cmake .. -DCMAKE_TOOLCHAIN_FILE=../emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake
make

RESULT=$?

if [ $RESULT -eq 0 ]; then
  cd ..
  cd build.web
  http-server -o app
else
  echo "Build failed"
fi