# create an interface that will be inherited by all components so that our defines are available for all components
set(LIB_OPTIONS_TARGET ${PROJECT_NAME}_options)

add_library(${LIB_OPTIONS_TARGET} INTERFACE)

# link to library top level target, each component created with lib_component will link to this target too
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIB_OPTIONS_TARGET})

# use this function to create an option for the library, it will automatically add the compile definition to
# each target that links lib_options_target
function(lib_option)
    set(LIB_SINGLE_VALUE_ARGS
            NAME
            DESCRIPTION
    )

    set(LIB_MULTI_VALUE_ARGS
            VALID_ARGS
    )

    cmake_parse_arguments(LIB_OPTIONS "" "${LIB_SINGLE_VALUE_ARGS}" "${LIB_MULTI_VALUE_ARGS}" ${ARGN})

    message(STATUS "-------------------------------------------------")

    if (NOT LIB_OPTIONS_NAME)
        message( FATAL_ERROR "'NAME' argument required.")
    endif ()

    if (NOT LIB_OPTIONS_DESCRIPTION)
        message( FATAL_ERROR "'DESCRIPTION' argument required.")
    endif ()

    if (NOT LIB_OPTIONS_VALID_ARGS)
        message( FATAL_ERROR "'VALID_ARGS' argument required.")
    endif ()

    # remove duplicate args
    list(REMOVE_DUPLICATES LIB_OPTIONS_VALID_ARGS)

    # default to first value in argument list if not defined
    list(GET LIB_OPTIONS_VALID_ARGS 0 VALID_ARG_FIRST_ELEMENT)
    option(${LIB_OPTIONS_NAME} ${LIB_OPTIONS_DESCRIPTION} ${VALID_ARG_FIRST_ELEMENT})

    # check if the option is valid
    message(STATUS "${LIB_OPTIONS_NAME}=${${LIB_OPTIONS_NAME}}")

    # add to our interface for use later, we define both the value and a unique define
    target_compile_definitions(${LIB_OPTIONS_TARGET} INTERFACE
            DEF_${LIB_OPTIONS_NAME}=${${LIB_OPTIONS_NAME}}
            DEF_${LIB_OPTIONS_NAME}_${${LIB_OPTIONS_NAME}}
    )
endfunction()