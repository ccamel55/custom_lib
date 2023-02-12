set(LIB_BACKEND_INPUT_LINUX
        "")

set(LIB_BACKEND_INPUT_MACOS
        "")

set(LIB_BACKEND_INPUT_WIN32
        "")

if (${LIB_BACKEND_INPUT} STREQUAL "none")

    # no implementation exists
    message(STATUS "Input backend: ${LIB_BACKEND_INPUT}")
elseif (${LIB_BACKEND_INPUT} STREQUAL "linux")

    # linux
    message(STATUS "Input backend: ${LIB_BACKEND_INPUT}")
    target_sources(custom_lib PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/backend_input_base.hpp
            ${LIB_BACKEND_INPUT_LINUX}
            )
elseif (${LIB_BACKEND_INPUT} STREQUAL "macos")

    # macos
    message(STATUS "Input backend: ${LIB_BACKEND_INPUT}")
    target_sources(custom_lib PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/backend_input_base.hpp
            ${LIB_BACKEND_INPUT_MACOS}
            )
elseif (${LIB_BACKEND_INPUT} STREQUAL "win32")

    # win32
    message(STATUS "Input backend: ${LIB_BACKEND_INPUT}")
    target_sources(custom_lib PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/backend_input_base.hpp
            ${LIB_BACKEND_INPUT_WIN32}
            )
endif()