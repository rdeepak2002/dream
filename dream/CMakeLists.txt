cmake_minimum_required(VERSION 3.23)
project(dream)

set(CMAKE_CXX_STANDARD 17)

if (EMSCRIPTEN)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build.emscripten)
else()
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

add_executable(${PROJECT_NAME} src/glad.c src/main.cpp src/Application.cpp src/SDLWindow.cpp src/OpenGLRenderer.cpp)

if (EMSCRIPTEN)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-s DEMANGLE_SUPPORT=1 --preload-file ${CMAKE_SOURCE_DIR}/assets --bind")
endif()
