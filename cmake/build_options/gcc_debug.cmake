list(APPEND GCC_DEBUG_COMPILE_OPTIONS
        -Og # compile with debugging optimizations
)

add_compile_options(${GCC_DEBUG_COMPILE_OPTIONS})