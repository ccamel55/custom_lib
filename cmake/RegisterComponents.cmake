if (LIB_ENABLE_TESTS)
	# Add Catch2 if we are using tests
	message(STATUS "------------------------------------------")
	CPMAddPackage(
		NAME Catch2
		GITHUB_REPOSITORY catchorg/Catch2
		VERSION 3.5.4
	)

	list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)

	include(CTest)
	include(Catch)

	enable_testing()
endif()

message(STATUS "------------------------------------------")
message(STATUS "Registering components: searching")
message(STATUS "------------------------------------------")

set(COMPONENT_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/components")

# Find all folders within the components directory
file(
	GLOB
	COMPONENTS
	LIST_DIRECTORIES true
	RELATIVE ${COMPONENT_FOLDER}
	${COMPONENT_FOLDER}/*
)

foreach (COMPONENT ${COMPONENTS})
	# Only look at directories
	set(CURRENT_COMPONENT_DIR "${COMPONENT_FOLDER}/${COMPONENT}")
	if (NOT IS_DIRECTORY ${CURRENT_COMPONENT_DIR})
		message(FATAL_ERROR "Found unexpected item in components directory: ${CURRENT_COMPONENT_DIR}")
		return()
	endif ()

	# Add as subdirectory
	add_subdirectory(${CURRENT_COMPONENT_DIR})
endforeach()

message(STATUS "------------------------------------------")
message(STATUS "Registering components: finished")

unset(CURRENT_COMPONENT_DIR)
unset(COMPONENTS)
unset(COMPONENT_FOLDER)