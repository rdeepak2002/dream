cmake_minimum_required(VERSION 3.23)
project(native)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/out/${CMAKE_SYSTEM_NAME})
include_directories(include)

add_library(native SHARED src/library.cpp)
