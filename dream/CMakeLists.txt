cmake_minimum_required(VERSION 3.23)
project(dream)

set(CMAKE_CXX_STANDARD 17)

# TODO: link libraries for OpenGL and SDL2
if (NOT EMSCRIPTEN)
    # Link OpenGL library
    find_package(OpenGL REQUIRED)
    if(NOT OPENGL_FOUND)
        message(FATAL_ERROR "OpenGL not found")
    endif()
    message(STATUS "OPENGL_INCLUDE_DIRS = ${OPENGL_INCLUDE_DIRS}")
    include_directories(${OPENGL_INCLUDE_DIRS})
    message(STATUS "OPENGL_LIBRARIES = ${OPENGL_LIBRARIES}")
    link_libraries(${OPENGL_LIBRARIES})

    # Link SDL2 library
    find_package(SDL2 REQUIRED)
    if(NOT SDL2_FOUND)
        message(FATAL_ERROR "SDL2 not found")
    endif()
    message(STATUS "SDL2_INCLUDE_DIRS = ${SDL2_INCLUDE_DIRS}")
    include_directories(${SDL2_INCLUDE_DIRS})
    message(STATUS "SDL2_LIBRARIES = ${SDL2_LIBRARIES}")
    link_libraries(${SDL2_LIBRARIES})
endif()

# header files
include_directories(include)

add_executable(dream src/glad.c src/main.cpp)
