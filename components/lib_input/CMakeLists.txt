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
)