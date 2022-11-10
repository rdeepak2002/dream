#!/bin/bash

# build assimp if necessary
cd build.web
if [ -d "assimp-5.2.5" ]; then
  echo "Assimp already built."
else
  echo "Building Assimp..."
  tar -xf assimp-5.2.5.tar.gz assimp-5.2.5
  cd assimp-5.2.5
  cmake . -DCMAKE_TOOLCHAIN_FILE=../../emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_BUILD_TYPE=Release
  make
  cd ..
fi
cd ..

# build dream engine
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