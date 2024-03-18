lib_option(
    NAME
        LIB_LOGGER
    DESCRIPTION
        "Set log level (0 = Off, 1 = Explicit, 2 = Errors, 3 = Warning, 4 = Info, 5 = Debug, 6 = Verbose"
    VALID_ARGS
        0 1 2 3 4 5 6
)

lib_component(lib_core
    SOURCES
        core_sdk.cpp
		logger/logger.cpp
		logger/handlers/stdio_handler.cpp
        math/math.cpp
		math/intersection.cpp
    INCLUDES
        include
    LINK_LIBS
		cxxopts
        fmt::fmt
		glm::glm
)