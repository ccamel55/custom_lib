cmake_minimum_required(VERSION 3.19)
project (custom_lib)

include(cmake/functions.cmake)

# force modern c/c++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} STATIC custom_lib.cpp)

add_subdirectory(backend)
add_subdirectory(common)
add_subdirectory(dependencies)

target_link_libraries(${PROJECT_NAME} PUBLIC
        nlohmann_json::nlohmann_json
        glfw
        )