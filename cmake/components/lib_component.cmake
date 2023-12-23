include(CMakeParseArguments)

macro(lib_component component_name)
    set(LIB_MULTI_VALUE_ARGS
            # sources to include in target
            SOURCES
            # include directories
            INCLUDES
            # which lib_options must be non none to enable the component
            DEPENDS
            # what target to link
            LINK_LIBS
    )

    cmake_parse_arguments(LIB_COMPONENT "" "" "${LIB_MULTI_VALUE_ARGS}" ${ARGN})

    set(${component_name}_enabled OFF)

    message(STATUS "-------------------------------------------------")
    message(STATUS "Registering component ${component_name}")

    # check mandatory arguments
    if (NOT LIB_COMPONENT_SOURCES)
        message( FATAL_ERROR "'SOURCES' argument required.")
    endif ()

    if (NOT LIB_COMPONENT_INCLUDES)
        message( FATAL_ERROR "'INCLUDES' argument required.")
    endif ()

    # verbose mandatory arguments and create a new target
    message(STATUS "sources: ")
    foreach (source ${LIB_COMPONENT_SOURCES})
        message(STATUS "    ${source}")
    endforeach ()

    message(STATUS "includes: ")
    foreach (include ${LIB_COMPONENT_INCLUDES})
        message(STATUS "    ${include}")
    endforeach ()

    if (LIB_COMPONENT_DEPENDS)
        # verbose depends
        message(STATUS "depends: ")
        foreach (depend ${LIB_COMPONENT_DEPENDS})
            message(STATUS "    ${depend}")
        endforeach ()

        # only after check if depends are set to not off
        foreach (depend ${LIB_COMPONENT_DEPENDS})
            if (${depend} STREQUAL "off")
                message(WARNING "${depend} is off, skipping component")
                return()
            endif ()
        endforeach ()
    endif ()

    add_library(${component_name} STATIC
            ${LIB_COMPONENT_SOURCES}
    )

    target_include_directories(${component_name} PUBLIC
            ${LIB_COMPONENT_INCLUDES}
    )

    target_link_libraries(${component_name} PUBLIC
            ${LIB_COMPILE_OPTIONS_TARGET}
            ${LIB_OPTIONS_TARGET}
    )

    if (LIB_COMPONENT_LINK_LIBS)
        # verbose
        message(STATUS "linking: ")
        foreach (link_lib ${LIB_COMPONENT_LINK_LIBS})
            message(STATUS "    ${link_lib}")
        endforeach ()

        # link publicly
        target_link_libraries(${component_name} PUBLIC
                ${LIB_COMPONENT_LINK_LIBS}
        )
    endif ()

    # link library to top level target
    target_link_libraries(${PROJECT_NAME} PUBLIC
            ${component_name}
    )

    set(${component_name}_enabled ON)

endmacro()