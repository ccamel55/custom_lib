lib_option(
    NAME
        LIB_RENDERING
    DESCRIPTION
        "Build rendering library"
    VALID_ARGS
        off gl3 vulkan
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
else (LIB_RENDERING STREQUAL vulkan)
    add_subdirectory(api/vulkan)
endif ()