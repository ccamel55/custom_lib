lib_option(
    NAME
        LIB_THREADING
    DESCRIPTION
        "Build threading library"
    VALID_ARGS
        off on
)

lib_component(lib_threading
    SOURCES
        async_caller.cpp
        thread_pool.cpp
        timer.cpp
    INCLUDES
        include
    DEPENDS
        LIB_THREADING
    LINK_LIBS
        lib_core
)