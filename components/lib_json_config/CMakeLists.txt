lib_option(
    NAME
        LIB_JSON_CONFIG
    DESCRIPTION
        "Build JSON config library"
    VALID_ARGS
        off on
)

lib_component(lib_json_config
    SOURCES
        lib_json_config.cpp
    INCLUDES
        include
    DEPENDS
        LIB_JSON_CONFIG
        LIB_HASHING
    LINK_LIBS
        lib_core
        lib_hashing
        nlohmann_json::nlohmann_json
)
