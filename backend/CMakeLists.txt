add_subdirectory(render)
add_subdirectory(window_creation)

target_include_directories(${PROJECT_NAME} PUBLIC
        include
        )