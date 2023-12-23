lib_option(
    NAME
        LIB_COMMAND_PARSER
    DESCRIPTION
        "Build command parser library"
    VALID_ARGS
        off on
)

lib_component(lib_command_parser
    SOURCES
        lib_command_parser.cpp
    INCLUDES
        include
    DEPENDS
        LIB_COMMAND_PARSER
)