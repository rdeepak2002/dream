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
# TODO: only do below if testing is enabled
if(NOT EMSCRIPTEN)
    DREAM_FIND_AND_LINK_TEST_LIBRARIES()
    DREAM_GLOB_TEST_CODE()
endif()

# define the executable application generated
add_executable(${PROJECT_NAME} ${ENGINE_SOURCE_FILES} ${TEST_FILES})

# TODO: only do below if testing is enabled
if(NOT EMSCRIPTEN)
    target_link_libraries(${PROJECT_NAME} GTest::gtest_main)
endif()

# resources folder to copy to binary directory
set(EXAMPLES_PATH ../examples)

if (EMSCRIPTEN)
    # web build Emscripten flags
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "--use-preload-plugins -std=c++1z --preload-file ../${EXAMPLES_PATH} -s ASSERTIONS -s LLD_REPORT_UNDEFINED -s ALLOW_MEMORY_GROWTH=1 -s USE_WEBGL2=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='[\"png\"]' -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -s FULL_ES3=1 -s FORCE_FILESYSTEM=1")
    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-s USE_SDL=2")
else()
    # copy files for desktop build
    file(COPY ${EXAMPLES_PATH} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()
