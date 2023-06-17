set(LIB_BACKEND_WINDOW_GLFW
        ${CMAKE_CURRENT_LIST_DIR}/glfw/backend_window.cpp
        ${CMAKE_CURRENT_LIST_DIR}/glfw/backend_window.hpp
        )

if (${LIB_BACKEND_WINDOW} STREQUAL "glfw")

    # glfw
    target_sources(custom_lib PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/backend_window_base.hpp
            ${LIB_BACKEND_WINDOW_GLFW}
            )

endif()

message(STATUS "LIB_BACKEND_WINDOW: ${LIB_BACKEND_WINDOW}")