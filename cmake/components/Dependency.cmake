include(CMakeParseArguments)

macro(add_dependency name)
	project("dep_${name}")

	# Template - defined arguments
	set(_ARG_DEF

	)

	# Template - one value arguments
	set(_ARG_ONE
		GITHUB_REPOSITORY
		GIT_TAG
	)

	# Template - multi value arguments
	set (_ARG_MULTI
		REQUIRES
		DEPENDENCIES
	)

	cmake_parse_arguments(
		${PROJECT_NAME}
		"${_ARG_DEF}"
		"${_ARG_ONE}"
		"${_ARG_MULTI}"
		${ARGN}
	)

	# Make sure all requirements from REQUIRES is true
	if (${PROJECT_NAME}_REQUIRES)
		foreach (REQUIREMENT ${${PROJECT_NAME}_REQUIRES})
			if (NOT ${REQUIREMENT})
				return()
			endif ()
		endforeach ()
	endif()

	message(STATUS "Dependency - ${name}")
	message(STATUS "------------------------------------------")

	# Add package using CPM
	CPMAddPackage(
		NAME ${name}
		GITHUB_REPOSITORY ${${PROJECT_NAME}_GITHUB_REPOSITORY}
		GIT_TAG ${${PROJECT_NAME}_GIT_TAG}
	)

	message(STATUS "------------------------------------------")

	# Create new target and add files
	file(
		GLOB_RECURSE
		DEP_SOURCE_FILES
		RELATIVE ${CMAKE_CURRENT_LIST_DIR}
		${CMAKE_CURRENT_LIST_DIR}/source/**.c*
	)

	add_library(${PROJECT_NAME} STATIC ${DEP_SOURCE_FILES})
	target_include_directories(${PROJECT_NAME} PRIVATE include_private)
	target_include_directories(${PROJECT_NAME} PUBLIC include)
	target_link_libraries(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_DEPENDENCIES})

	# Create new test target and add test
	if (LIB_ENABLE_TESTS)
		set(PROJECT_NAME_TEST ${PROJECT_NAME}_test)

		file(
			GLOB_RECURSE
			DEP_TEST_FILES
			RELATIVE ${CMAKE_CURRENT_LIST_DIR}
			${CMAKE_CURRENT_LIST_DIR}/test/**.c*
		)

		add_executable(${PROJECT_NAME_TEST} ${DEP_TEST_FILES})
		target_link_libraries(${PROJECT_NAME_TEST} PRIVATE Catch2::Catch2WithMain ${PROJECT_NAME})

		add_test(ComponentTests ${PROJECT_NAME_TEST})

		unset(DEP_TEST_FILES)
		unset(PROJECT_NAME_TEST)
	endif()

	unset(DEP_SOURCE_FILES)
	unset(_ARG_DEF)
	unset(_ARG_ONE)
	unset(_ARG_MULTI)

endmacro()