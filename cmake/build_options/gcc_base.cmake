# see https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html for more options

list(APPEND GCC_BASE_COMPILE_OPTIONS
        -Wall # enable all warning
        -Wno-unused-function # disable unused functions check
        -march=native # tell compiler what architecture we are building for
        -fno-rtti # disable rtti
        -fno-exceptions # disable exceptions
        -flto # enable link time optimization
        -ffast-math # enable fast math functions that dont align with IEEE/ANSI standard fully.
)

add_compile_options(${GCC_BASE_COMPILE_OPTIONS})
