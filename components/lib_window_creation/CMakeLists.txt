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
        LIB_RENDERING
    LINK_LIBS
        lib_core
        lib_rendering
)

# additional linking and includes for glfw
if (LIB_WINDOW_CREATION STREQUAL glfw)
    add_subdirectory(api/glfw)
endif ()


