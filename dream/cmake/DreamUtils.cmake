macro(DREAM_FIND_AND_LINK_LIBRARIES)
    if (EMSCRIPTEN)
        # Link QuickJS
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/external/quickjs")
        include_directories(include/quickjs)

        # Link assimp library
        link_libraries("${CMAKE_CURRENT_SOURCE_DIR}/build.web/assimp-5.2.5/lib/libassimp.a")
        include_directories("${CMAKE_CURRENT_SOURCE_DIR}/build.web/assimp-5.2.5/include")

        # Link yaml-cpp library
        link_libraries("${CMAKE_CURRENT_SOURCE_DIR}/build.web/yaml-cpp-yaml-cpp-0.7.0/src/libyaml-cpp.a")

        # Lin lua library
        link_libraries("${CMAKE_CURRENT_SOURCE_DIR}/build.web/lua-5.4.4/src/liblua.a")
    else()
        # Link QuickJS
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/external/quickjs")
        include_directories(include/quickjs)

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

        # Link SDL2-image library
        find_package(sdl2-image CONFIG REQUIRED)

        # Link assimp library
        find_package(ASSIMP REQUIRED)
        message(STATUS "ASSIMP_INCLUDE_DIRS = ${ASSIMP_INCLUDE_DIRS}")
        include_directories(${ASSIMP_INCLUDE_DIRS})
        message(STATUS "ASSIMP_LIBRARIES = ${ASSIMP_LIBRARIES}")
        link_libraries(${ASSIMP_LIBRARIES})

        # Link yaml-cpp library
        find_package(yaml-cpp CONFIG REQUIRED)

        # Link Lua library
        find_package(Lua REQUIRED)
        message(STATUS "LUA_INCLUDE_DIR ${LUA_INCLUDE_DIR}")
        include_directories(${LUA_INCLUDE_DIR})
        message(STATUS "LUA_LIBRARIES ${LUA_LIBRARIES}")
        link_libraries(${LUA_LIBRARIES})

        # Link bullet library
        find_package(Bullet CONFIG REQUIRED)
        message(STATUS BULLET_INCLUDE_DIRS=${BULLET_INCLUDE_DIRS})
        include_directories(${BULLET_INCLUDE_DIRS})
        message(STATUS BULLET_LIBRARIES=${BULLET_LIBRARIES})
        link_libraries(${BULLET_LIBRARIES})

        # Link OpenAL library
        find_package(OpenAL CONFIG REQUIRED)
    endif()
endmacro()

macro(DREAM_FIND_AND_LINK_LIBRARIES_PART_2)
    # Link QuickJS
    target_link_libraries(${PROJECT_NAME} quickjs)

    if(APPLE)
        target_link_libraries(${PROJECT_NAME} "-framework Foundation" objc)
        target_link_libraries(${PROJECT_NAME} "-framework Cocoa" objc)
    endif()

    if (EMSCRIPTEN)
        # Link lua library
        set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "... -L${CMAKE_CURRENT_SOURCE_DIR}/build.web/lua-5.4.4/src -s ...")
    else()
        # Link yaml-cpp
        target_link_libraries(${PROJECT_NAME} yaml-cpp)
        # Link g-test
        target_link_libraries(${PROJECT_NAME} GTest::gtest_main)
        # Link OpenAL
        target_link_libraries(${PROJECT_NAME} OpenAL::OpenAL)
        # Link SDL2-image
        target_link_libraries(${PROJECT_NAME} SDL2::SDL2_image)
    endif()
endmacro()

macro(DREAM_INCLUDE_HEADERS)
    # header files
    include_directories(include)
    include_directories(include/imgui)
    include_directories(include/imgui-node-editor)
endmacro()

macro(DREAM_GLOB_SOURCE_CODE)
    # find source files for engine code
    FILE(GLOB ENGINE_SOURCE_FILES
            include/imgui/*.cpp
            include/imgui/misc/cpp/*.cpp
            include/imgui/backends/imgui_impl_sdl.cpp
            include/imgui/backends/imgui_impl_opengl3.cpp
            include/imgui-texteditor/*.cpp
            include/imgui-node-editor/*.cpp
            include/ogldev/*.cpp
            src/*.cpp
            src/project/*.cpp
            src/editor/*.cpp
            src/window/*.cpp
            src/renderer/*.cpp
            src/util/*.cpp
            src/scene/*.cpp
            src/scene/component/*.cpp
            src/scene/system/*.cpp
            src/renderer/glad.c
            src/window/cocoautils.mm)
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