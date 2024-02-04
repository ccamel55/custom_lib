add_subdirectory(dependencies/glfw)

lib_option(
    NAME
        LIB_WINDOW_CREATION
    DESCRIPTION
        "Build window creation library"
    VALID_ARGS
        off on
)

lib_component(lib_window_creation
    SOURCES
        lib_window_creation.cpp
    INCLUDES
        include
    DEPENDS
        LIB_WINDOW_CREATION
    LINK_LIBS
        lib_core
        glfw
)

if (NOT LIB_RENDERING STREQUAL off)
    target_link_libraries(lib_window_creation PUBLIC
        lib_rendering
    )
endif ()

if (NOT LIB_INPUT STREQUAL off)
    target_link_libraries(lib_window_creation PUBLIC
        lib_input
    )
endif ()
