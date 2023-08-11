lib_create_options(LIB_BACKEND_RENDER "Selects render api backend" none gl3)

if (LIB_BACKEND_RENDER STREQUAL gl3)
    target_sources(${PROJECT_NAME} PUBLIC
            opengl3/render.cpp
            )
endif ()