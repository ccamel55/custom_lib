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
        interface/ui_base_element.cpp
        interface/ui_drag_bar.cpp
        interface/ui_form.cpp
        interface/ui_base_window.cpp
    INCLUDES
        include
    DEPENDS
        LIB_GUI
        LIB_INPUT
        LIB_RENDERING
    LINK_LIBS
        lib_core
        lib_input
        lib_rendering
)

add_subdirectory(ui/console)
add_subdirectory(ui/settings)
add_subdirectory(ui/text_ui)