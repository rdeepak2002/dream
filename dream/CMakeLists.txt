cmake_minimum_required(VERSION 3.23)
project(dream)
set(CMAKE_CXX_STANDARD 17)

# use FindLibraries.cmake file to link libraries and include header files for project
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(DreamUtils)

# set build folders for executables
DREAM_DEFINE_BINARY_OUTPUT_DIRS()

# include headers
DREAM_INCLUDE_HEADERS()

# find and link the libraries and source code
DREAM_FIND_AND_LINK_LIBRARIES()
DREAM_GLOB_SOURCE_CODE()

# find and link the libraries and source code for testing
if(NOT EMSCRIPTEN)
    DREAM_FIND_AND_LINK_TEST_LIBRARIES()
    DREAM_GLOB_TEST_CODE()
endif()

# define the executable application generated
add_executable(${PROJECT_NAME} ${ENGINE_SOURCE_FILES} ${TEST_FILES})

# target link library commands
DREAM_FIND_AND_LINK_LIBRARIES_PART_2()

# resources folder to copy to binary directory
set(ASSETS_PATH assets)
set(EXAMPLES_PATH ../examples)

if (EMSCRIPTEN)
    # web build Emscripten flags
    set(ASSETS_PATH ${ASSETS_PATH})
    set(EXAMPLES_PATH ${EXAMPLES_PATH})
    set(RESOURCES_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources)
    file(COPY ${ASSETS_PATH} DESTINATION ${RESOURCES_PATH})
    file(COPY ${EXAMPLES_PATH} DESTINATION ${RESOURCES_PATH})
    set(WEB_INCLUDES "-I${CMAKE_CURRENT_SOURCE_DIR}/build.web/yaml-cpp-yaml-cpp-0.7.0/include -I${CMAKE_CURRENT_SOURCE_DIR}/build.web/bullet-2.82-r2704/src -I${CMAKE_CURRENT_SOURCE_DIR}/build.web/lua-5.4.4/src")
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "--use-preload-plugins -std=c++1z --preload-file ${RESOURCES_PATH}@/ -s ASSERTIONS -s LLD_REPORT_UNDEFINED -s ALLOW_MEMORY_GROWTH=1 -lopenal -s USE_WEBGL2=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='[\"png\"]' -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -s USE_BULLET=1 -s FULL_ES3=1 -s FORCE_FILESYSTEM=1 -s EXPORTED_RUNTIME_METHODS=['FS','ccall','cwrap'] -s EXPORTED_FUNCTIONS='['_disablePointerLock', '_main']' -s NO_DISABLE_EXCEPTION_CATCHING ${WEB_INCLUDES} -O3")
    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-s USE_SDL=2 ${WEB_INCLUDES} -O3")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${RESOURCES_PATH}
            COMMENT "Deleted resources folder used for creating .data file"
    )
else()
    # copy files for desktop build
    file(COPY ${ASSETS_PATH} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    file(COPY ${EXAMPLES_PATH} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()
