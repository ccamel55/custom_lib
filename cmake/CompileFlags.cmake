if (
	NOT (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang") AND
	NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
)
	message(SEND_ERROR "Current compiler is not supported, please use GCC or Clang.")
endif()

message(STATUS "------------------------------------------")

# clang/gcc supported options that we want to be enabled for all builds
list(APPEND BUILD_OPTIONS
	-Wno-unused-function # disable unused functions check
	-march=native # tell compiler what architecture we are building for, allows for vectorisation
)

if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
	message(STATUS "Compile flags: building with MSVC Clang")

	# msvc front end compile options
	list(APPEND BUILD_OPTIONS
		-W3
		-GR # disable RTTI
		-fp:fast # fast floating point arithmetic
	)

	if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR
		CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo"
	)
		list(APPEND BUILD_OPTIONS
			-Od # disable optimisations, should make debugging easier
		)
	else()
		list(APPEND BUILD_OPTIONS
			-flto # enable link time optimization
			-Oi # allow intrinsics
			-O2 # favour speed over size
			-Oy # commit frame pointers
			-Gw # place global data in COMDAT sections
			-Gy # enable function level linking
		)
	endif()

else()
	message(STATUS "Compile flags: building with GNU Clang/gcc")

	# options are valid for clang and gcc on GNU but if using msvc front ends we need to use msvc options
	list(APPEND BUILD_OPTIONS
		-Wall # all warnings
		-fno-rtti # disable rtti
		-ffast-math # enable fast math functions that dont align with IEEE/ANSI standard fully.
		-ftree-vectorize # enabled by -O3 but in case we dont compile in release, enable vectorisation
	)

	if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR
		CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo"
	)
		list(APPEND BUILD_OPTIONS
			-Og # compile with debugging optimizations
		)
	else()
		list(APPEND BUILD_OPTIONS
			-flto # enable link time optimization
			-Ofast # Ofast is O3 with further flags
			-fomit-frame-pointer # since we should never be debugging a release build, remove frame pointers
		)
	endif()
endif()

# Append to C and C++ Build options then unset
set(BUILD_OPTIONS_STRING)

# we need to loop through all options to remove the ; from lists
foreach (option ${BUILD_OPTIONS})
	set(BUILD_OPTIONS_STRING "${BUILD_OPTIONS_STRING} ${option}")
endforeach ()

# set compile options for C, and C++
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${BUILD_OPTIONS_STRING}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${BUILD_OPTIONS_STRING}")

unset(BUILD_OPTIONS_STRING)
unset(BUILD_OPTIONS)