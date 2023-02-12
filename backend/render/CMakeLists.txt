set(LIB_BACKEND_RENDER_OPENGL3
        "")

set(LIB_BACKEND_RENDER_VULKAN
        "")

if (${LIB_BACKEND_RENDER} STREQUAL "none")

    # no implementation exists
    message(STATUS "Render backend: ${LIB_BACKEND_RENDER}")
elseif (${LIB_BACKEND_RENDER} STREQUAL "opengl3")

    # opengl3
    message(STATUS "Render backend: ${LIB_BACKEND_RENDER}")
    target_sources(custom_lib PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/backend_render_base.hpp
            ${LIB_BACKEND_RENDER_OPENGL3}
            )
elseif (${LIB_BACKEND_RENDER} STREQUAL "vulkan")

    # vulkan
    message(STATUS "Render backend: ${LIB_BACKEND_RENDER}")
    target_sources(custom_lib PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/backend_render_base.hpp
            ${LIB_BACKEND_RENDER_VULKAN}
            )
endif()