macro(camel_library)
	# Resolve name folder where CMakeLists.txt is located
	get_filename_component(_PROJECT_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)
	string(REPLACE " " "-" _PROJECT_NAME ${_PROJECT_NAME})
	string(TOUPPER ${_PROJECT_NAME} _PROJECT_NAME_UPPER)

	project(${_PROJECT_NAME})

	# Template - defined arguments
	set(
		_ARG_DEF
			""
	)

	# Template - one value arguments
	set(
		_ARG_ONE
			FORCE_DISABLE
	)

	# Template - multi value arguments
	set(
		_ARG_MULTI
			DEPENDENCIES
	)

	cmake_parse_arguments(
		${PROJECT_NAME}
			"${_ARG_DEF}"
			"${_ARG_ONE}"
			"${_ARG_MULTI}"
			${ARGN}
	)

	# Create alias
	set(_PROJECT_FORCE_DISABLE "${PROJECT_NAME}_FORCE_DISABLE")
	set(_PROJECT_DEPENDENCIES "${PROJECT_NAME}_DEPENDENCIES")

	# Print out some stats
	message(STATUS ${_CAMEL_CMAKE_SPACER})
	message(STATUS "Name: ${_PROJECT_NAME}")
	message(STATUS "Dependencies:")

	if(${${_PROJECT_FORCE_DISABLE}})
		message(STATUS "Force disabled: SKIPPED.")
		return()
	endif()

	foreach(_DEPENDENCY ${${_PROJECT_DEPENDENCIES}})
		message(STATUS "\t\t${_DEPENDENCY}")
	endforeach()

	# Check if we should build this module at all
	if (NOT ${${_CAMEL_ENABLE_PREFIX}${_PROJECT_NAME_UPPER}})
		message(STATUS "Module is not enabled: SKIPPED.")
		return()
	endif()

	set(_PRE_OPS_FILE "${CMAKE_CURRENT_LIST_DIR}/pre_ops.cmake")
	set(_POST_OPS_FILE "${CMAKE_CURRENT_LIST_DIR}/post_ops.cmake")

	# Call pre-ops file if it exists which is used to resolve dependencies
	if(EXISTS ${_PRE_OPS_FILE})
		message(STATUS "Running pre-ops")
		include(${_PRE_OPS_FILE})
	else()
		message(STATUS "Pre-ops file does not exist - ignoring")
	endif()

	# Look for source files under `source` subdirectory
	file(
		GLOB_RECURSE _SOURCE_FILES
		RELATIVE ${CMAKE_CURRENT_LIST_DIR}
			"${CMAKE_CURRENT_LIST_DIR}/source/**.c*"
	)

	add_library(${_PROJECT_NAME} STATIC ${_SOURCE_FILES})

	target_include_directories(${_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
	target_include_directories(${_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/private)

	target_link_libraries(${_PROJECT_NAME} PUBLIC ${${_PROJECT_DEPENDENCIES}})

	add_installs()

	# Call post ops file if it exists which is used to link additional things to the target
	if(EXISTS ${_POST_OPS_FILE})
		message(STATUS "Running post-ops")
		include(${_POST_OPS_FILE})
	else()
		message(STATUS "Post-ops file does not exist - ignoring")
	endif()

	add_tests()

	unset(_PROJECT_NAME)
	unset(_PROJECT_NAME_UPPER)
	unset(_PROJECT_DEPENDENCIES)
	unset(_PRE_OPS_FILE)
	unset(_PRE_OPS_FILE)
	unset(_SOURCE_FILES)
endmacro()