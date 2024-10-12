#
# Set platform
#
if(WIN32)
	set(CAMEL_PLATFORM "windows")
elseif(APPLE)
	set(CAMEL_PLATFORM "apple")
elseif(UNIX AND NOT APPLE)
	set(CAMEL_PLATFORM "linux")
else()
	message(FATAL_ERROR "\t\tPlatform unknown.")
endif()

#
# Set architecture
# Note: this doesn't work for all processor, but it should work well enough
#
if(CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)|(x86_64)|(x64)")
	set(CAMEL_ARCH "amd65")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "(arm64)|(aarch64)|(arm64)")
	set(CAMEL_ARCH "arm64")
else()
	message(FATAL_ERROR "\t\tArchitecture unknown (${CMAKE_SYSTEM_PROCESSOR})")
endif ()

#
# Create a new variable based on CAMEL_PLATFORM and CAMEL_ARCH, also add them as preprocessors.
#
string(TOUPPER ${CAMEL_PLATFORM} CAMEL_PLATFORM_UPPER)
string(TOLOWER ${CAMEL_PLATFORM} CAMEL_PLATFORM_LOWER)

string(TOUPPER ${CAMEL_ARCH} CAMEL_ARCH_UPPER)
string(TOLOWER ${CAMEL_ARCH} CAMEL_ARCH_LOWER)

# Make variable based on platform/arch
set(_PLATFORM_VAR CAMEL_PLATFORM_${CAMEL_PLATFORM_UPPER})
set(_ARCH_VAR CAMEL_ARCH_${CAMEL_ARCH_UPPER})

# Set and add preprocessor
set(${_PLATFORM_VAR} 1)
set(${_ARCH_VAR} 1)

add_compile_definitions(${_PLATFORM_VAR})
add_compile_definitions(${_ARCH_VAR})

message("\t\tCAMEL_PLATFORM: ${CAMEL_PLATFORM_UPPER}")
message("\t\tCAMEL_ARCH: ${CAMEL_ARCH_UPPER}")

unset(_PLATFORM_VAR)
unset(_ARCH_VAR)