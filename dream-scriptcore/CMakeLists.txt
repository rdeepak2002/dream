cmake_minimum_required(VERSION 3.23)
project(dream_scriptcore)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_library(dream_scriptcore src/library.cpp)
