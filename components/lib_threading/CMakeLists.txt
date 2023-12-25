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
        lib_threading.cpp
        async_caller/async_caller.cpp
        thread_pool/job_collection.cpp
        thread_pool/thread_pool.cpp
        timer/timer.cpp
    INCLUDES
        include
    DEPENDS
        LIB_THREADING
    LINK_LIBS
        lib_core
)