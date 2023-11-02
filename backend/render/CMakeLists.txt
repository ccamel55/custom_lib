lib_create_options(LIB_BACKEND_RENDER "Selects render api backend" none gl3)

if (LIB_BACKEND_RENDER STREQUAL gl3)
    target_sources(${PROJECT_NAME} PRIVATE
            api/opengl3/render_api.cpp
            api/opengl3/render_state.cpp
            api/opengl3/shaders.cpp
    )
endif ()

if (NOT LIB_BACKEND_RENDER STREQUAL none)
    target_sources(${PROJECT_NAME} PRIVATE
            renderer.cpp
            common/render_command.cpp
    )
endif ()