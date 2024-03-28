cmake_minimum_required(VERSION 3.26)

project(custom_lib)

# force modern c/c++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# custom build options for the project
option(
    CUSTOM_LIB_BUILD_FAT_LIBRARY
    "Whether or not to link all libraries to the custom_lib target."
    OFF
)

message(STATUS "CUSTOM_LIB_BUILD_FAT_LIBRARY: ${CUSTOM_LIB_BUILD_FAT_LIBRARY}")

option(
    CUSTOM_LIB_DYNAMIC_LIBRARY
    "Build custom lib as a dynamic library."
    OFF
)

message(STATUS "CUSTOM_LIB_DYNAMIC_LIBRARY: ${CUSTOM_LIB_DYNAMIC_LIBRARY}")

# enable libraries and add tests only if this repo is top level
if(PROJECT_IS_TOP_LEVEL)
    message(STATUS "-------------------------------------------------")
    message(STATUS "Custom Lib is the top level project. Enabling tests and setting default options.")

    set(LIB_COMMAND_PARSER on)
    set(LIB_GUI on)
    set(LIB_HASHING on)
    set(LIB_INPUT on)
    set(LIB_JSON_CONFIG on)
    set(LIB_THREADING on)
    set(LIB_UTILS on)
    set(LIB_LOGGER 6)

    set(LIB_RENDERING off)
    set(LIB_WINDOW_CREATION off)

    # all unit tests should be automatically registered by catch2
    add_subdirectory(unit_test)
endif()

# todo: add flag for dynamic library
add_library(${PROJECT_NAME} STATIC
        custom_lib.cpp
)

include(cmake/options/lib_option.cmake)
include(cmake/options/build_options.cmake)

include(cmake/components/lib_component.cmake)
include(cmake/components/lib_sub_component.cmake)

include(cmake/helper/gather_libraries.cmake)

# build options that will propagate to the whole project
add_subdirectory(dependencies)
add_subdirectory(components)
add_subdirectory(core_sdk)

# combine all linked libraries into one target 'custom_lib'
if (CUSTOM_LIB_BUILD_FAT_LIBRARY)
    list(APPEND SUB_LIBRARIES "")

    get_libraries_recursive(
            LIBRARY_LIST_NAME SUB_LIBRARIES
            LIBRARY_NAME custom_lib
    )

    # for each library in the list, link it to the main library
    foreach(library ${SUB_LIBRARIES})
        target_sources(${PROJECT_NAME} PUBLIC
                $<TARGET_OBJECTS:${library}>
        )
    endforeach()
endif ()

