add_subdirectory(dependencies/glad)

lib_sub_component(
    PARENT
        lib_rendering
    SOURCES
        render_api.cpp
        render_state.cpp
        shaders.cpp
    INCLUDES
        include
    LINK_LIBS
        glad
)