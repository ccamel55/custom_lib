set(LIB_BACKEND_WINDOW_LINUX
        "")

set(LIB_BACKEND_WINDOW_MACOS
        ${CMAKE_CURRENT_LIST_DIR}/macos/backend_window.cpp
        ${CMAKE_CURRENT_LIST_DIR}/macos/backend_window.hpp
        )

set(LIB_BACKEND_WINDOW_WIN32
        "")

if (${LIB_BACKEND_WINDOW} STREQUAL "none")

    # no implementation exists
    message(STATUS "Window backend: ${LIB_BACKEND_WINDOW}")
elseif (${LIB_BACKEND_WINDOW} STREQUAL "linux")

    # linux
    message(STATUS "Window backend: ${LIB_BACKEND_WINDOW}")
    target_sources(custom_lib PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/backend_window_base.hpp
            ${LIB_BACKEND_WINDOW_LINUX}
            )
elseif (${LIB_BACKEND_WINDOW} STREQUAL "macos")

    # macos
    message(STATUS "Window backend: ${LIB_BACKEND_WINDOW}")
    target_sources(custom_lib PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/backend_window_base.hpp
            ${LIB_BACKEND_WINDOW_MACOS}
            )
elseif (${LIB_BACKEND_WINDOW} STREQUAL "win32")

    # win32
    message(STATUS "Window backend: ${LIB_BACKEND_WINDOW}")
    target_sources(custom_lib PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/backend_window_base.hpp
            ${LIB_BACKEND_WINDOW_WIN32}
            )
endif()