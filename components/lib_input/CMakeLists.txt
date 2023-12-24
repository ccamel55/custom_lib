lib_option(
    NAME
        LIB_INPUT
    DESCRIPTION
        "Build input library"
    VALID_ARGS
        off glfw win32
)

lib_component(lib_input
    SOURCES
        lib_input.cpp
    INCLUDES
        include
    DEPENDS
        LIB_INPUT
        LIB_THREADING
    LINK_LIBS
        lib_core
        lib_threading
)

if (LIB_INPUT STREQUAL glfw)
    add_subdirectory(api/glfw)
endif ()