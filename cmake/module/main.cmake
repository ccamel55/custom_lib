include(module/macro)

#
# Define functions we use to create components.
#
message(STATUS ${_CAMEL_CMAKE_SPACER})
message(STATUS "Modules:")

# Globally search for files in the module subdirectory ending in .cmake and add them
file(
	GLOB_RECURSE _MODULES
	RELATIVE ${_CAMEL_CMAKE_MAIN_DIR}
		"${CMAKE_CURRENT_LIST_DIR}/module/*.cmake"
)

foreach(_MODULE ${_MODULES})
	# Include the dependency - this will resolve / error if can't find
	string(REGEX REPLACE "\\.[^.]*$" "" _MODULE ${_MODULE})
	message(STATUS "\t\t${_MODULE}")
	include(${_MODULE})
endforeach()

unset(_MODULES)