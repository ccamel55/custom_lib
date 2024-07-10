# Min hook only supports windows
if (NOT CURRENT_PLATFORM STREQUAL "win32")
	return()
endif ()

add_dependency(
	minhook
	GITHUB_REPOSITORY
		TsudaKageyu/minhook
	GIT_TAG
		8eedf5b1b353850dd2617d6461254f5e72164555
)

# minhook doesn't have a CMakeLists.txt file so we need to add everything manually
# this is based off a newer version of minhook that isn't release yet :(

set(SOURCES_MINHOOK
	"${minhook_SOURCE_DIR}/src/buffer.c"
	"${minhook_SOURCE_DIR}/src/hook.c"
	"${minhook_SOURCE_DIR}/src/trampoline.c"
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(SOURCES_HDE "${minhook_SOURCE_DIR}/src/hde/hde64.c")
else()
	set(SOURCES_HDE "${minhook_SOURCE_DIR}/src/hde/hde32.c")
endif()

target_sources(dep_minhook PRIVATE
	${SOURCES_MINHOOK}
	${SOURCES_HDE}
)

target_include_directories(dep_minhook PUBLIC
	"${minhook_SOURCE_DIR}/include"
)

target_include_directories(dep_minhook PRIVATE "src/")
target_include_directories(dep_minhook PRIVATE "src/hde/")

