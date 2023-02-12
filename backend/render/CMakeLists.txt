set(LIB_BACKEND_RENDER_OPENGL3
        ${CMAKE_CURRENT_LIST_DIR}/opengl_3/backend_render.cpp
        ${CMAKE_CURRENT_LIST_DIR}/opengl_3/backend_render.hpp
        )

if (${LIB_BACKEND_RENDER} STREQUAL "opengl3")

    # opengl3
    target_sources(custom_lib PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/backend_render_base.hpp
            ${LIB_BACKEND_RENDER_OPENGL3}
            )

endif()

message(STATUS "Render backend: ${LIB_BACKEND_RENDER}")