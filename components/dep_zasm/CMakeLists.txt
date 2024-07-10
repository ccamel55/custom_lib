# zasm is x86 only :(
if (NOT CURRENT_ARCH STREQUAL "x86")
	return()
endif ()

# todo: disable library install, need to make a .diff patch since zydis cmake doesn't have
# an option for that yetset(ZASM_BUILD_TESTS OFF)
set(ZASM_BUILD_BENCHMARKS OFF)
set(ZASM_BUILD_EXAMPLES OFF)

add_dependency(
	zasm
	GITHUB_REPOSITORY
		zyantific/zasm
	GIT_TAG
		a18fa34251b161cc18e4c0fc385dee7cea104cfd
	DEPENDENCIES
		zasm
)

# U suck zasm so many warnings :(
set(ZASM_COMPILE_FLAGS "-Wno-sign-compare")

set_target_properties(zasm PROPERTIES
	COMPILE_FLAGS ${ZASM_COMPILE_FLAGS}
)

set_target_properties(zasm PROPERTIES
	COMPILE_FLAGS ${ZASM_COMPILE_FLAGS}
)

