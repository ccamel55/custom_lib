# include fmt
add_subdirectory(fmt)

# include JSON
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(json)

# include glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(glfw)

target_include_directories(${PROJECT_NAME} PUBLIC
        json/single_include/
        )