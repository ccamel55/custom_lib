# Most dependencies are handled by CPM, thank you CPM 🙏
set(JSON_BuildTests OFF CACHE INTERNAL "")
CPMAddPackage("gh:fmtlib/fmt#10.1.1")
CPMAddPackage("gh:nlohmann/json@3.11.2")
CPMAddPackage("gh:jarro2783/cxxopts@3.1.1")

target_link_libraries(${PROJECT_NAME} PUBLIC
        nlohmann_json::nlohmann_json
        fmt::fmt
        cxxopts
)

if (LIB_BACKEND_WINDOW_CREATION STREQUAL glfw)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    CPMAddPackage("gh:glfw/glfw#3.3.8")
    target_link_libraries(${PROJECT_NAME} PUBLIC
            glfw
            )
endif()

if (LIB_BACKEND_RENDER STREQUAL gl3)
    add_subdirectory(glad)

    target_link_libraries(${PROJECT_NAME} PUBLIC
            glad
            )
endif ()