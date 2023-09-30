lib_create_options(LIB_BACKEND_RENDER "Selects render api backend" none gl3)

if (LIB_BACKEND_RENDER STREQUAL gl3)
    target_sources(${PROJECT_NAME} PUBLIC
            opengl3/render.cpp
            opengl3/types/shader.cpp
            opengl3/types/texture.cpp
            opengl3/utils/vertex_array_object.cpp
            opengl3/utils/vertex_buffer_object.cpp
            opengl3/utils/vertex_layout.cpp
            )
endif ()

if (NOT LIB_BACKEND_RENDER STREQUAL none)
    target_sources(${PROJECT_NAME} PUBLIC
            common/bitmap_generator.cpp
    )
endif ()