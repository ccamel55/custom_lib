lib_option(
    NAME
        LIB_WINDOW_CREATION
    DESCRIPTION
        "Build window creation library"
    VALID_ARGS
        off glfw win32
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
)

# additional linking and includes for glfw
if (LIB_WINDOW_CREATION STREQUAL glfw)
    add_subdirectory(api/glfw)
endif ()

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
