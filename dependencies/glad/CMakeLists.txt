cmake_minimum_required(VERSION 3.19)
project (glad)

add_library(${PROJECT_NAME} STATIC glad.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC
        include
        )