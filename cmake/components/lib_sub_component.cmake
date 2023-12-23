include(CMakeParseArguments)

macro(lib_sub_component)
    set(LIB_SINGLE_VALUE_ARGS
        # target we will link this subdirectory too
        PARENT
    )

    set(LIB_MULTI_VALUE_ARGS
            # sources to include in target
            SOURCES
            # include directories
            INCLUDES
            # what target to link
            LINK_LIBS
    )

    cmake_parse_arguments(LIB_SUB_COMPONENT "" "${LIB_SINGLE_VALUE_ARGS}" "${LIB_MULTI_VALUE_ARGS}" ${ARGN})

    # check mandatory arguments
    if (NOT LIB_SUB_COMPONENT_PARENT)
        message( FATAL_ERROR "'PARENT' argument required.")
    endif ()

    message(STATUS "-------------------------------------------------")
    message(STATUS "Registering sub component to ${LIB_SUB_COMPONENT_PARENT}")

    if (NOT LIB_SUB_COMPONENT_SOURCES)
        message( FATAL_ERROR "'SOURCES' argument required.")
    endif ()

    if (NOT LIB_SUB_COMPONENT_INCLUDES)
        message( FATAL_ERROR "'INCLUDES' argument required.")
    endif ()

    if ("${LIB_SUB_COMPONENT_PARENT}_enabled" STREQUAL "OFF")
        message(FATAL_ERROR "parent (${LIB_SUB_COMPONENT_PARENT}) is not being built, can not add sub component.")
        return()
    endif ()

    message(STATUS "sources: ")
    foreach (source ${LIB_SUB_COMPONENT_SOURCES})
        message(STATUS "    ${source}")
    endforeach ()

    message(STATUS "includes: ")
    foreach (include ${LIB_SUB_COMPONENT_INCLUDES})
        message(STATUS "    ${include}")
    endforeach ()

    target_sources(${LIB_SUB_COMPONENT_PARENT} PRIVATE
            ${LIB_SUB_COMPONENT_SOURCES}
    )

    target_include_directories(${LIB_SUB_COMPONENT_PARENT} PUBLIC
            ${LIB_SUB_COMPONENT_INCLUDES}
    )

    if (LIB_SUB_COMPONENT_LINK_LIBS)
        # verbose
        message(STATUS "linking: ")
        foreach (link_lib ${LIB_SUB_COMPONENT_LINK_LIBS})
            message(STATUS "    ${link_lib}")
        endforeach ()

        # link to parent
        target_link_libraries(${LIB_SUB_COMPONENT_PARENT} PUBLIC
                ${LIB_SUB_COMPONENT_LINK_LIBS}
        )
    endif ()

endmacro()