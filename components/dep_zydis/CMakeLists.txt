# zydis is x86 only :(
if (NOT CURRENT_ARCH STREQUAL "x86")
	return()
endif ()

# todo: disable library install, need to make a .diff patch since zydis cmake doesn't have
# an option for that yet
set(ZYDIS_BUILD_SHARED_LIB OFF)
set(ZYDIS_BUILD_EXAMPLES OFF)
set(ZYDIS_BUILD_TOOLS OFF)
set(ZYDIS_BUILD_MAN OFF)
set(ZYDIS_BUILD_DOXYGEN OFF)
set(ZYDIS_BUILD_TESTS OFF)

add_dependency(
	zydis
	GITHUB_REPOSITORY
		zyantific/zydis
	GIT_TAG
		v4.1.0
	DEPENDENCIES
		Zydis
)

