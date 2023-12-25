lib_option(
    NAME
        LIB_RENDERING
    DESCRIPTION
        "Build rendering library"
    VALID_ARGS
        off gl3 dx9 dx10 dx12 vulkan
)

lib_option(
    NAME
        LIB_RENDERING_EXPERIMENTAL
    DESCRIPTION
        "Use WIP or experimental features in rendering library"
    VALID_ARGS
        off on
)

lib_component(lib_rendering
    SOURCES
        lib_rendering.cpp
        common/atlas_generator.cpp
        common/font_loader.cpp
        common/image_loader.cpp
        common/render_command.cpp
    INCLUDES
        include
    DEPENDS
        LIB_RENDERING
    LINK_LIBS
        lib_core
)

if (LIB_RENDERING STREQUAL gl3)
    add_subdirectory(api/opengl3)
endif ()