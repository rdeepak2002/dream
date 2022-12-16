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

### Desktop

Install dependencies via vcpkg:

1. Install [vcpkg](https://vcpkg.io/en/getting-started.html)
2. Install dependencies for this project
```shell
vcpkg install
```

## How to use vcpkg with CLion

Preferences > Build, Execution, Deployment > CMake > CMake options:

```shell
-DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
```