#!/bin/bash

# steps to build assimp:
# 1. check if build.web/assimp-5.2.5 folder exists (not tar.gz file) -> if not, then continue, otherwise skip next steps
# 2. cd build.web
# 3. extract assimp-5.2.5.tar.gz to assimp-5.2.5 folder
# 4. cd assimp-5.2.5
# 5. cmake . -DCMAKE_TOOLCHAIN_FILE=../../emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_BUILD_TYPE=Release
# 6. make
# 7. cd ..
# 8. cd ..

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