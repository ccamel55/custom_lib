lib_option(
    NAME
        LIB_GUI
    DESCRIPTION
        "Build gui library"
    VALID_ARGS
        off on
)

lib_component(lib_gui
    SOURCES
        lib_gui.cpp
    INCLUDES
        include
    DEPENDS
        LIB_GUI
)