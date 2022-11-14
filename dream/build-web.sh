#!/bin/bash

# start building web libraries
cd build.web

# build assimp if necessary
if [ -d "assimp-5.2.5" ]; then
  echo "Assimp already built."
else
  echo "Building Assimp..."
  tar -xf assimp-5.2.5.tar.gz assimp-5.2.5
  cd assimp-5.2.5
  cmake . -DCMAKE_TOOLCHAIN_FILE=../../emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_BUILD_TYPE=Release
  make

  RESULT=$?

  if [ $RESULT -eq 0 ]; then
    echo "Assimp build succeeded"
  else
    echo "Assimp build failed"
    cd ..
    rm -rf assimp-5.2.5
    exit 1
  fi

  cd ..
fi

# build yaml-cpp if necessary
if [ -d "yaml-cpp-yaml-cpp-0.7.0" ]; then
  echo "yaml-cpp already built."
else
  echo "Building yaml-cpp..."
  tar -xf yaml-cpp-yaml-cpp-0.7.0.tar.gz yaml-cpp-yaml-cpp-0.7.0
  cd yaml-cpp-yaml-cpp-0.7.0
  cd src
  cmake .. -DCMAKE_TOOLCHAIN_FILE=../../../emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_BUILD_TYPE=Release -DYAML_CPP_BUILD_TESTS=OFF
  make

  RESULT=$?

  if [ $RESULT -eq 0 ]; then
    echo "yaml-cpp build succeeded"
  else
    echo "yaml-cpp build failed"
    cd ..
    cd ..
    rm -rf yaml-cpp-yaml-cpp-0.7.0
    exit 1
  fi

  cd ..
  cd ..
fi

# stop building web libraries
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
  exit 1
fi