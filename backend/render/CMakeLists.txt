set(LIB_BACKEND_RENDER_COMMON
        ${CMAKE_CURRENT_LIST_DIR}/common/stb_truetype.hpp
        ${CMAKE_CURRENT_LIST_DIR}/common/bitmap_font.cpp
        ${CMAKE_CURRENT_LIST_DIR}/common/bitmap_font.hpp
        )

set(LIB_BACKEND_RENDER_OPENGL3
        ${CMAKE_CURRENT_LIST_DIR}/opengl3/backend_render.cpp
        ${CMAKE_CURRENT_LIST_DIR}/opengl3/backend_render.hpp
        )

set(LIB_BACKEND_RENDER_DIRECTX_9
        ${CMAKE_CURRENT_LIST_DIR}/directx9/backend_render.cpp
        ${CMAKE_CURRENT_LIST_DIR}/directx9/backend_render.hpp
        )

set(LIB_BACKEND_RENDER_DIRECTX_11
        ${CMAKE_CURRENT_LIST_DIR}/directx11/backend_render.cpp
        ${CMAKE_CURRENT_LIST_DIR}/directx11/backend_render.hpp
        )

if (${LIB_BACKEND_RENDER} STREQUAL "opengl3")

    # opengl3
    target_sources(custom_lib PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/backend_render_base.hpp
            ${LIB_BACKEND_RENDER_COMMON}
            ${LIB_BACKEND_RENDER_OPENGL3}
            )

elseif(${LIB_BACKEND_RENDER} STREQUAL "dx9")

    # dx9
    target_sources(custom_lib PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/backend_render_base.hpp
            ${LIB_BACKEND_RENDER_COMMON}
            ${LIB_BACKEND_RENDER_DIRECTX_9}
            )

elseif(${LIB_BACKEND_RENDER} STREQUAL "dx11")

    # dx11
    target_sources(custom_lib PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/backend_render_base.hpp
            ${LIB_BACKEND_RENDER_COMMON}
            ${LIB_BACKEND_RENDER_DIRECTX_11}
            )

endif()

message(STATUS "Render backend: ${LIB_BACKEND_RENDER}")