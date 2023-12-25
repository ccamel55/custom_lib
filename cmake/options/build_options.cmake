if (NOT (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang") AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(SEND_ERROR "Current compiler is not supported by custom lib, please use gcc or clang.")
endif()

# clang/gcc supported options that we want to be enabled for all builds
list(APPEND LIB_BUILD_OPTIONS
        -Wno-unused-function # disable unused functions check
        -march=native # tell compiler what architecture we are building for, allows for vectorisation
        -flto # enable link time optimization
)

if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    message(STATUS "-------------------------------------------------")
    message(STATUS "Building with MSVC Clang options ${CMAKE_BUILD_TYPE}")

    # msvc front end compile options
    list(APPEND LIB_BUILD_OPTIONS
            -W3
            -GR # disable RTTI
            -EHsc # disable exceptions
            -fp:fast # fast floating point arithmetic
            -D_HAS_EXCEPTIONS=0 # tell msvs there are no exceptions
    )

    if (CMAKE_BUILD_TYPE EQUAL "Debug")
        list(APPEND LIB_BUILD_OPTIONS
                -Od # disable optimisations, should make debugging easier
        )
    else()
        list(APPEND LIB_BUILD_OPTIONS
                -Oi # allow intrinsics
                -O2 # favour speed over size
                -Oy # commit frame pointers
                -Gw # place global data in COMDAT sections
                -Gy # enable function level linking
        )
    endif()

else()
    message(STATUS "-------------------------------------------------")
    message(STATUS "Building with GNU Clang/gcc options ${CMAKE_BUILD_TYPE}")

    # options are valid for clang and gcc on GNU but if using msvc front ends we need to use msvc options
    list(APPEND LIB_BUILD_OPTIONS
            -Wall # all warnings
            -fno-rtti # disable rtti
            -fno-exceptions # disable exceptions
            -ffast-math # enable fast math functions that dont align with IEEE/ANSI standard fully.
            -ftree-vectorize # enabled by -O3 but in case we dont compile in release, enable vectorisation
    )

    if (CMAKE_BUILD_TYPE EQUAL "Debug")
        list(APPEND LIB_BUILD_OPTIONS
                -Og # compile with debugging optimizations
        )
    else()
        list(APPEND LIB_BUILD_OPTIONS
                -Ofast # Ofast is O3 with further flags
                -fomit-frame-pointer # since we should never be debugging a release build, remove frame pointers
        )
    endif()
endif()

# create an interface that will be inherited by all components so that compile options apply to all components
set(LIB_COMPILE_OPTIONS_TARGET ${PROJECT_NAME}_compile_options)

add_library(${LIB_COMPILE_OPTIONS_TARGET} INTERFACE)
target_compile_options(${LIB_COMPILE_OPTIONS_TARGET} INTERFACE ${LIB_BUILD_OPTIONS})

# link to library top level target, each component created with lib_component will link to this target too
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIB_COMPILE_OPTIONS_TARGET})
