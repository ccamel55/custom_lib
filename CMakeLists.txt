cmake_minimum_required(VERSION 3.5)
project (custom_lib)

# use cpp 2020
set(CMAKE_CXX_STANDARD 20)

# custom_lib.cpp contains no code, it is a work around for building a library without initial sources
add_library(${PROJECT_NAME} STATIC custom_lib.cpp)

# register options
set(LIB_BACKEND_INPUT "none" CACHE STRING "Input handling")
set_property(CACHE LIB_BACKEND_INPUT PROPERTY STRINGS "none" "glfw" "wndproc")

set(LIB_BACKEND_RENDER "none" CACHE STRING "Render implementation")
set_property(CACHE LIB_BACKEND_RENDER PROPERTY STRINGS "none" "dx9" "dx11" "opengl3")

set(LIB_BACKEND_WINDOW "none" CACHE STRING "Window creation")
set_property(CACHE LIB_BACKEND_WINDOW PROPERTY STRINGS "none" "glfw")

# include macro and singleton header files
target_sources( ${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/macros.hpp
        ${CMAKE_CURRENT_LIST_DIR}/singleton.hpp
        )

add_subdirectory(backend)
add_subdirectory(common)
add_subdirectory(deps)
add_subdirectory(frontend)

target_include_directories(${PROJECT_NAME} PUBLIC
        .
        )

target_link_libraries(${PROJECT_NAME} PUBLIC
        nlohmann_json::nlohmann_json
        fmt::fmt
        glfw
        )