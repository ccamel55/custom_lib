# include JSON
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(json)

if (LIB_BACKEND_WINDOW_CREATION STREQUAL glfw)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    add_subdirectory(glfw)
    target_link_libraries(${PROJECT_NAME} PUBLIC
            glfw
            )
endif()

if (LIB_BACKEND_RENDER STREQUAL gl3)
    add_subdirectory(glad)
    add_subdirectory(glm)
    target_link_libraries(${PROJECT_NAME} PUBLIC
            glad
            glm::glm
            )

    # Suppress volatile deprecated warnings, annoying!!
    target_compile_options(glm INTERFACE -Wno-volatile)

endif ()

target_include_directories(${PROJECT_NAME} PUBLIC
        json/single_include/
        )

target_link_libraries(${PROJECT_NAME} PUBLIC
        nlohmann_json::nlohmann_json
        )