function(lib_create_options option_name help)

    option(${option_name} ${help} none)

    if (NOT ${option_name} IN_LIST ARGN)
        message(SEND_ERROR "${option_name} invalid option, valid options: ${ARGN}")
    else ()
        message(STATUS "${option_name}=${${option_name}}")
    endif()

endfunction()