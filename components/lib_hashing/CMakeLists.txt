lib_option(
    NAME
        LIB_HASHING
    DESCRIPTION
        "Build hashing library"
    VALID_ARGS
        off on
)

lib_component(lib_hashing
    SOURCES
        lib_hashing.cpp
    INCLUDES
        include
    DEPENDS
        LIB_HASHING
)