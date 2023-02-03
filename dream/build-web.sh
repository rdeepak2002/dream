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

# build lua if necessary
if [ -d "lua-5.4.4" ]; then
  echo "lua already built."
else
  echo "Building lua..."
  tar -xf lua-5.4.4.tar.gz lua-5.4.4
  cd lua-5.4.4
  cd src
  make generic CC='emcc -s WASM=1' AR='emar rcu' RANLIB='emranlib'
  make

  RESULT=$?

  if [ $RESULT -eq 0 ]; then
    echo "lua build succeeded"
  else
    echo "lua build failed"
    cd ..
    cd ..
    rm -rf lua-5.4.4
    exit 1
  fi

  cd ..
  cd ..
fi

# extract headers for bullet physics for web build
if [ -d "bullet-2.82-r2704" ]; then
  echo "bullet already extracted."
else
  echo "extracting bullet..."
  tar -xf bullet-2.82-r2704.tgz bullet-2.82-r2704

  RESULT=$?

  if [ $RESULT -eq 0 ]; then
    echo "extracted bullet"
  else
    echo "failed to extract bullet"
    exit 1
  fi
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
  if [[ $OSTYPE == 'darwin'* ]]; then
    say "Build succeeded"
  fi
  cd ..
  cd build.web
  http-server -o app
else
  if [[ $OSTYPE == 'darwin'* ]]; then
    say "Build failed"
  fi
  echo "Build failed"
  exit 1
fi