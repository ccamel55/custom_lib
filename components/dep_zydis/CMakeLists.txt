# zydis is x86 only :(
if (NOT CURRENT_ARCH STREQUAL "x86")
	return()
endif ()

# todo: disable library install, need to make a .diff patch since zydis cmake doesn't have
# an option for that yet
CPMAddPackage(
	NAME
		zydis
	GITHUB_REPOSITORY
		zyantific/zydis
	GIT_TAG
		v4.1.0
	OPTIONS
		"ZYDIS_BUILD_SHARED_LIB OFF"
		"ZYDIS_BUILD_EXAMPLES OFF"
		"ZYDIS_BUILD_TOOLS OFF"
		"ZYDIS_BUILD_MAN OFF"
		"ZYDIS_BUILD_DOXYGEN OFF"
		"ZYDIS_BUILD_TESTS OFF"
)

add_dependency(
	zydis
	DEPENDENCIES
		Zydis
)

