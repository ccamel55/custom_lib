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
        LIB_INPUT
        LIB_JSON_CONFIG
        LIB_RENDERING
    LINK_LIBS
        lib_core
        lib_input
        lib_json_config
        lib_rendering
)