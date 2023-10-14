lib_create_options(LIB_BACKEND_INPUT "Selects input api backend" none glfw)

if ((NOT LIB_BACKEND_WINDOW_CREATION STREQUAL none) AND (NOT LIB_BACKEND_INPUT STREQUAL none) AND (NOT LIB_BACKEND_INPUT STREQUAL LIB_BACKEND_WINDOW_CREATION))
    message(WARNING "window and input implementations are not the same")
endif()

if (LIB_BACKEND_INPUT STREQUAL glfw)
    target_sources(${PROJECT_NAME} PRIVATE
            glfw/input_handler.cpp
            )
endif ()