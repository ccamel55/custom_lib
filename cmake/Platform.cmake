set(CURRENT_ARCH "")
set(CURRENT_PLATFORM "")

# Note: this doesn't work for all processor, but it should work well enough
if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)|(x86_64)|(x64)")
	set(CURRENT_ARCH "x86")
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "(arm64)|(aarch64)|(arm64)")
	set(CURRENT_ARCH "arm64")
else ()
	set(CURRENT_ARCH "")
endif ()

if (WIN32)
	set(CURRENT_PLATFORM "win32")
elseif (APPLE)
	set(CURRENT_PLATFORM "darwin")
elseif (UNIX AND NOT APPLE)
	set(CURRENT_PLATFORM "linux")
else ()
	set(CURRENT_PLATFORM "")
endif()

# Make defines available to everything
add_definitions(
	-DCURRENT_ARCH_${CURRENT_ARCH}
	-DCURRENT_ARCH_${CURRENT_PLATFORM}
)