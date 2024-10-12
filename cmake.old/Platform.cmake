# Note: this doesn't work for all processor, but it should work well enough
if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)|(x86_64)|(x64)")
	set(_CURRENT_ARCH "x86")
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "(arm64)|(aarch64)|(arm64)")
	set(_CURRENT_ARCH "arm64")
endif ()

if (WIN32)
	set(_CURRENT_PLATFORM "win32")
elseif (APPLE)
	set(_CURRENT_PLATFORM "darwin")
elseif (UNIX AND NOT APPLE)
	set(_CURRENT_PLATFORM "linux")
endif()

set(CURRENT_ARCH ${_CURRENT_ARCH} CACHE INTERNAL "")
set(CURRENT_PLATFORM ${_CURRENT_PLATFORM} CACHE INTERNAL "")