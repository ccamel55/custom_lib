cmake_minimum_required(VERSION 3.19)
project (custom_lib)

include(cmake/CPM.cmake)
include(cmake/functions.cmake)

# if we load custom_lib as the top level directory, use default setup, build the library and then run unit tests
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    include(cmake/build_options/gcc_base.cmake)
    include(cmake/build_options/gcc_release.cmake)
    include(cmake/configs/default.cmake)
endif()

# force modern c/c++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} STATIC custom_lib.cpp)

add_subdirectory(backend)
add_subdirectory(common)
add_subdirectory(dependencies)
add_subdirectory(misc)
add_subdirectory(unit_test)