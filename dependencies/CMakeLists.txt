# Most dependencies are handled by CPM, thank you CPM 🙏
set(JSON_BuildTests OFF CACHE INTERNAL "")
CPMAddPackage("gh:fmtlib/fmt#10.1.1")
CPMAddPackage("gh:nlohmann/json@3.11.2")

CPMAddPackage(
        NAME Boost
        VERSION 1.81.0
        GITHUB_REPOSITORY "boostorg/boost"
        GIT_TAG "boost-1.81.0"
)

target_link_libraries(${PROJECT_NAME} PUBLIC
        nlohmann_json::nlohmann_json
        fmt::fmt
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
    CPMAddPackage("gh:g-truc/glm#0.9.9.8")
    add_subdirectory(glad)

    target_link_libraries(${PROJECT_NAME} PUBLIC
            glad
            glm::glm
            )

    #suppress volatile deprecated warnings, annoying!!
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(glm INTERFACE -Wno-volatile)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(glm INTERFACE -Wno-deprecated-volatile)
    endif()
endif ()