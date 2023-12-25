lib_option(
    NAME
        LIB_INPUT
    DESCRIPTION
        "Build input library"
    VALID_ARGS
        off on
)

lib_component(lib_input
    SOURCES
        input_processor.cpp
        lib_input.cpp
    INCLUDES
        include
    DEPENDS
        LIB_INPUT
    LINK_LIBS
        lib_core
)