# Dream Engine

## About

Core for Dream Engine

## Build

### Web

Install Emscripten once (ensure you do not have Emscripten globally installed through a package manager like Homebrew):

```shell
./setup-emscripten.sh
```

Build using CMake:

```shell
cd build.web
cmake .. -DCMAKE_TOOLCHAIN_FILE=../emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake
```