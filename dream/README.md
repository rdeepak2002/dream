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

1. Install [vcpkg](https://vcpkg.io/en/getting-started.html)
2. Install dependencies for this project
```shell
vcpkg install
```
