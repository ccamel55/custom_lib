lib_create_options(LIB_BACKEND_WINDOW_CREATION "Selects window creation backend" none glfw)

if (LIB_BACKEND_WINDOW_CREATION STREQUAL glfw)
    target_sources(${PROJECT_NAME} PUBLIC
            glfw/window_creation.cpp
            )
endif()