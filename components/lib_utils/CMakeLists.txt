lib_option(
    NAME
        LIB_UTILS
    DESCRIPTION
        "General purpose utilities"
    VALID_ARGS
        off on
)

lib_component(lib_utils
    SOURCES
        lib_utils.cpp
        fps_helper.cpp
    INCLUDES
        include
    LINK_LIBS
        lib_core
)