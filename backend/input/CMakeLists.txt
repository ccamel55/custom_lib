set(LIB_BACKEND_INPUT_GLFW
        ${CMAKE_CURRENT_LIST_DIR}/glfw/backend_input.cpp
        ${CMAKE_CURRENT_LIST_DIR}/glfw/backend_input.hpp
        )

set(LIB_BACKEND_INPUT_WNDPROC
        ${CMAKE_CURRENT_LIST_DIR}/wndproc/backend_input.cpp
        ${CMAKE_CURRENT_LIST_DIR}/wndproc/backend_input.hpp
        )

if (${LIB_BACKEND_INPUT} STREQUAL "glfw")

    # glfw
    target_sources(custom_lib PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/backend_input_base.hpp
            ${LIB_BACKEND_INPUT_GLFW}
            )

elseif(${LIB_BACKEND_INPUT} STREQUAL "wndproc")

    # wndproc
    target_sources(custom_lib PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/backend_input_base.hpp
            ${LIB_BACKEND_INPUT_WNDPROC}
            )

endif()

message(STATUS "Input backend: ${LIB_BACKEND_INPUT}")