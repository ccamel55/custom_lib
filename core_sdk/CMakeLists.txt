lib_option(
    NAME
        LIB_LOGGER
    DESCRIPTION
        "Set log level (0 = Off, 1 = Errors, 2 = Warnings, 3 = Info, 4 = Debug"
    VALID_ARGS
        0 1 2 3 4
)

lib_component(lib_core
    SOURCES
        core_sdk.cpp
        math/math.cpp
		math/intersection.cpp
    INCLUDES
        include
    LINK_LIBS
        fmt::fmt
        cxxopts
)