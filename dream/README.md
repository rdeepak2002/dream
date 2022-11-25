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
./build-web.sh
```

Install dependencies via vcpkg:

```shell
./vcpkg/bootstrap-vcpkg.sh
./vcpkg/vcpkg install assimp
./vcpkg/vcpkg install yaml-cpp
```

Install dependencies for web via vcpkg:

