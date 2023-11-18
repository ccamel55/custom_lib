cmake_minimum_required(VERSION 3.26)

project(custom_lib)

# force modern c/c++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# todo: add flag for dynamic library
add_library(${PROJECT_NAME} STATIC
        custom_lib.cpp
)

include(cmake/options/lib_option.cmake)
include(cmake/options/build_options.cmake)

include(cmake/components/lib_component.cmake)
include(cmake/components/lib_sub_component.cmake)

# build options that will propagate to the whole project
add_subdirectory(dependencies)
add_subdirectory(components)
add_subdirectory(core_sdk)

# all unit tests should be automatically registered by catch2
add_subdirectory(unit_test)