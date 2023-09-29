list(APPEND GCC_RELEASE_COMPILE_OPTIONS
        -Ofast # Ofast is O3 with further flags
        -fomit-frame-pointer # since we should never be debugging a release build, remove frame pointers
)

add_compile_options(${GCC_RELEASE_COMPILE_OPTIONS})