set(LIB_BACKEND_INPUT_GLFW
        ${CMAKE_CURRENT_LIST_DIR}/glfw/backend_input.cpp
        ${CMAKE_CURRENT_LIST_DIR}/glfw/backend_input.hpp
        )

if (${LIB_BACKEND_WINDOW} STREQUAL "glfw")

    # glfw
    target_sources(custom_lib PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/backend_input_base.hpp
            ${LIB_BACKEND_INPUT_GLFW}
            )

endif()