macro(DREAM_FIND_AND_LINK_LIBRARIES)
    if (EMSCRIPTEN)
        # Link assimp library
        link_libraries("${CMAKE_CURRENT_SOURCE_DIR}/build.web/assimp-5.2.5/lib/libassimp.a")
        include_directories("${CMAKE_CURRENT_SOURCE_DIR}/build.web/assimp-5.2.5/include")

        # Link yaml-cpp library
        link_libraries("${CMAKE_CURRENT_SOURCE_DIR}/build.web/yaml-cpp-yaml-cpp-0.7.0/src/libyaml-cpp.a")
    else()
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

        # Link assimp library
        find_package(ASSIMP REQUIRED)
        message(STATUS "ASSIMP_INCLUDE_DIRS = ${ASSIMP_INCLUDE_DIRS}")
        include_directories(${ASSIMP_INCLUDE_DIRS})
        message(STATUS "ASSIMP_LIBRARIES = ${ASSIMP_LIBRARIES}")
        link_libraries(${ASSIMP_LIBRARIES})

        # Link yaml-cpp library
        find_package(yaml-cpp CONFIG REQUIRED)
    endif()
endmacro()

macro(DREAM_INCLUDE_HEADERS)
    # header files
    include_directories(include)
    include_directories(include/imgui)
endmacro()

macro(DREAM_GLOB_SOURCE_CODE)
    # find source files for engine code
    FILE(GLOB ENGINE_SOURCE_FILES
            include/imgui/*.cpp
            include/imgui/backends/imgui_impl_sdl.cpp
            include/imgui/backends/imgui_impl_opengl3.cpp
            src/*.cpp
            src/editor/*.cpp
            src/window/*.cpp
            src/renderer/*.cpp
            src/scene/*.cpp
            src/scene/component/*.cpp
            src/renderer/glad.c)
endmacro()

macro(DREAM_FIND_AND_LINK_TEST_LIBRARIES)
    # find the Google test package and enable testing
    find_package(GTest CONFIG REQUIRED)
    enable_testing()
endmacro()

macro(DREAM_GLOB_TEST_CODE)
    # find sources filed for test code
    FILE(GLOB TEST_FILES
            test/scene/*.cpp)
endmacro()

macro(DREAM_DEFINE_BINARY_OUTPUT_DIRS)
    # set build folders for executables
    if (EMSCRIPTEN)
        message("Creating web build")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build.web)
    else()
        message("Creating desktop build")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build.desktop/${CMAKE_SYSTEM_NAME})
    endif()
endmacro()