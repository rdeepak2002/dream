cmake_minimum_required(VERSION 3.23)
project(dream)

set(CMAKE_CXX_STANDARD 17)

if (EMSCRIPTEN)
    message("Creating web build")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build.web)
else()
    message("Creating desktop build")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build.desktop/${CMAKE_SYSTEM_NAME})
endif()

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
include_directories(include/imgui)

# source files for engine code
FILE(GLOB ENGINE_SOURCE_FILES
        include/imgui/*.cpp
        include/imgui/backends/imgui_impl_sdl.cpp
        include/imgui/backends/imgui_impl_opengl3.cpp
        src/*.cpp
        src/editor/*.cpp
        src/window/*.cpp
        src/glad.c)

add_executable(${PROJECT_NAME} src/glad.c ${ENGINE_SOURCE_FILES})

if (EMSCRIPTEN)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "--use-preload-plugins -std=c++1z --preload-file ../../examples -s ASSERTIONS -s LLD_REPORT_UNDEFINED -s ALLOW_MEMORY_GROWTH=1 -s USE_WEBGL2=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='[\"png\"]' -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -s FULL_ES3=1 -s FORCE_FILESYSTEM=1")
    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-s USE_SDL=2")
endif()
