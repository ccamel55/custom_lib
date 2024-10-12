#
# Define a variable as an option, print out status and add preprocessor definition if enabled.
#
macro(lib_option NAME DESCRIPTION DEFAULT_VALUE)
	option(${NAME} ${DESCRIPTION} ${DEFAULT_VALUE})
	message(STATUS "\t\t${NAME}: ${${NAME}}")

	# add preprocessor if active
	if(${${NAME}})
		add_compile_definitions(${NAME})
	endif()
endmacro()