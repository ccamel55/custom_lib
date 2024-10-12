#
# Scan directory for library/interface names and create options to enable/disable them.
#
macro(scan_components)
	message(STATUS ${_CAMEL_CMAKE_SPACER})
	message(STATUS "Scanning components:")

	# Look for folders in 'components' subdirectory and get their names.
	file(
		GLOB _COMPONENTS
		RELATIVE ${_CAMEL_CMAKE_TOP_LEVEL_DIR}
			"${_CAMEL_CMAKE_TOP_LEVEL_DIR}/component/*"
	)

	# Get name of each folder
	foreach(_COMPONENT ${_COMPONENTS})
		get_filename_component(_COMPONENT ${_COMPONENT} NAME)

		# sanitise name
		string(REPLACE " " "_" _COMPONENT ${_COMPONENT})
		string(REPLACE "-" "_" _COMPONENT ${_COMPONENT})
		string(TOUPPER ${_COMPONENT} _COMPONENT)

		# create option - set to enabled if CAMEL_BUILD_ALL_COMPONENTS is true
		set(_VAR_NAME "${_CAMEL_ENABLE_PREFIX}${_COMPONENT}")
		lib_option(${_VAR_NAME} "Enable component: ${_COMPONENT}" ${CAMEL_BUILD_ALL_COMPONENTS})
	endforeach()

	unset(_COMPONENTS)
	unset(_VAR_NAME)
endmacro()

#
# Add each subproject inside the `component` subdirectory.
#
macro(add_components)
	message(STATUS ${_CAMEL_CMAKE_SPACER})
	message(STATUS "Adding components:")

	# Look for folders in 'components' subdirectory and get their names.
	file(
		GLOB _COMPONENTS
		RELATIVE ${_CAMEL_CMAKE_TOP_LEVEL_DIR}
			"${_CAMEL_CMAKE_TOP_LEVEL_DIR}/component/*"
	)

	# add_subdirectory for all folders
	foreach(_COMPONENT ${_COMPONENTS})
		add_subdirectory(${_COMPONENT})
	endforeach()

	unset(_COMPONENTS)
endmacro()

#
# Scan directory for tool anmes create options to enable/disable them.
#
macro(scan_tools)
	message(STATUS ${_CAMEL_CMAKE_SPACER})
	message(STATUS "Scanning tools:")

	# Look for folders in 'tool' subdirectory and get their names.
	file(
		GLOB _TOOLS
		RELATIVE ${_CAMEL_CMAKE_TOP_LEVEL_DIR}
			"${_CAMEL_CMAKE_TOP_LEVEL_DIR}/tool/*"
	)

	# Get name of each folder
	foreach(_TOOL ${_TOOLS})
		get_filename_component(_TOOL ${_TOOL} NAME)

		# sanitise name
		string(REPLACE " " "_" _TOOL ${_TOOL})
		string(REPLACE "-" "_" _TOOL ${_TOOL})
		string(TOUPPER ${_TOOL} _TOOL)

		# create option - set to enabled if CAMEL_BUILD_ALL_TOOLS is true
		set(_VAR_NAME "${_CAMEL_ENABLE_PREFIX}${_TOOL}")
		lib_option(${_VAR_NAME} "Enable tool: ${_TOOL}" ${CAMEL_BUILD_ALL_TOOLS})
	endforeach()

	unset(_TOOLS)
	unset(_VAR_NAME)
endmacro()

#
# Add each subproject inside the `tool` subdirectory.
#
macro(add_tools)
	message(STATUS ${_CAMEL_CMAKE_SPACER})
	message(STATUS "Adding tools:")

	message(STATUS ${_CAMEL_CMAKE_SPACER})
	message(STATUS "Scanning tools:")

	# Look for folders in 'tool' subdirectory and get their names.
	file(
		GLOB _TOOLS
		RELATIVE ${_CAMEL_CMAKE_TOP_LEVEL_DIR}
			"${_CAMEL_CMAKE_TOP_LEVEL_DIR}/tool/*"
	)

	# add_subdirectory for all folders
	foreach(_TOOL ${_TOOLS})
		add_subdirectory(${_TOOL})
	endforeach()

	unset(_TOOLS)
endmacro()

#
# Add tests found for each module - this is called by [module].cmake internally
#
macro(add_tests)
	# if we don't enable tests then skip
	if (NOT CAMEL_BUILD_ALL_TESTS)
		return()
	endif()

	set(_PROJECT_NAME_TEST ${PROJECT_NAME}_test)

	# Look for test files
	file(
		GLOB_RECURSE _TEST_FILES
		RELATIVE ${CMAKE_CURRENT_LIST_DIR}
			"${CMAKE_CURRENT_LIST_DIR}/test/**.c*"
	)

	add_executable(${_PROJECT_NAME_TEST} ${_TEST_FILES})
	target_link_libraries(${_PROJECT_NAME_TEST} PRIVATE Catch2::Catch2WithMain ${PROJECT_NAME})

	# Register with catch2
	catch_discover_tests(${_PROJECT_NAME_TEST})

	unset(_TEST_FILES)
	unset(_PROJECT_NAME_TEST)
endmacro()

#
# Add install configuration - this is called by [module].cmake internally
#
macro(add_installs)
	# if we don't allow install then skip it
	if (NOT CAMEL_INSTALL)
		return()
	endif()

	# Look for public include files
	file(
		GLOB_RECURSE _INCLUDE_FILES
		RELATIVE ${CMAKE_CURRENT_LIST_DIR}
			"${CMAKE_CURRENT_LIST_DIR}/include/**.c*"
	)

	# Register include files as header set
	target_sources(
		${PROJECT_NAME}
		PUBLIC
			FILE_SET public_headers
			TYPE HEADERS
			BASE_DIRS
				include
			FILES
				${_INCLUDE_FILES}
	)

	# Setup install
	install(
		TARGETS
			${PROJECT_NAME}
		LIBRARY
		ARCHIVE
		FILE_SET
			public_headers
		OPTIONAL
	)

	unset(_INCLUDE_FILES)
endmacro()