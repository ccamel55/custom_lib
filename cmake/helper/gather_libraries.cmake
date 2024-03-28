macro(get_libraries_recursive)
    set(LIB_SINGLE_VALUE_ARGS
        # list to append libraries to
        LIBRARY_LIST_NAME
        # library to search recursively
        LIBRARY_NAME
    )

    cmake_parse_arguments(
            GET_LIB_ARGS
            ""
            ${LIB_SINGLE_VALUE_ARGS}
            ""
            ${ARGN}
    )

    message(STATUS "-------------------------------------------------")

    if(NOT GET_LIB_ARGS_LIBRARY_LIST_NAME)
        message(FATAL_ERROR "LIBRARY_LIST_NAME is not set, please specify a list to append libraries to.")
    endif()

    if (NOT GET_LIB_ARGS_LIBRARY_NAME)
        message(FATAL_ERROR "LIBRARY_NAME is not set, please specify a library to search.")
    endif()

    message(STATUS "Recursively gathering libraries for ${GET_LIB_ARGS_LIBRARY_NAME}")

    # get the libraries static libaries
    _search_recursively(${GET_LIB_ARGS_LIBRARY_LIST_NAME} ${GET_LIB_ARGS_LIBRARY_NAME})

    list(REMOVE_DUPLICATES ${GET_LIB_ARGS_LIBRARY_LIST_NAME})

    # print out name of each library we will generate
    foreach(library ${${GET_LIB_ARGS_LIBRARY_LIST_NAME}})
        message(STATUS "    ${library}")
    endforeach()
endmacro()

macro(_search_recursively LIBRARY_LIST LIBRARY_NAME)
    # get all linked libraries
    get_target_property(
            _TARGET_LINKED_LIBRARIES
            ${LIBRARY_NAME}
            LINK_LIBRARIES
    )

    # iterate over all linked libraries
    foreach(library ${_TARGET_LINKED_LIBRARIES})
        if (NOT TARGET ${library})
            continue()
        endif()

        # make sure its a linkable target
        get_target_property(_TARGET_TYPE ${library} TYPE)

        if (NOT _TARGET_TYPE STREQUAL "SHARED_LIBRARY" AND
            NOT _TARGET_TYPE STREQUAL "STATIC_LIBRARY" AND
            NOT _TARGET_TYPE STREQUAL "MODULE_LIBRARY" AND
            NOT _TARGET_TYPE STREQUAL "OBJECT_LIBRARY" AND
            NOT _TARGET_TYPE STREQUAL "EXECUTABLE")
            continue()
        endif()

        list(APPEND ${LIBRARY_LIST} ${library})

        _search_recursively(${LIBRARY_LIST} ${library})
    endforeach()
endmacro()