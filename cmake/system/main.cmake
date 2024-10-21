#
# Define and set system type and architecture then apply appropriate compiler flags.
# Note: this is the only module that MIGHT be shared with another top level project.
#
message(STATUS ${_CAMEL_CMAKE_SPACER})
message(STATUS "System:")

include(system/resolve_platform)
include(system/resolve_compiler)

# Add appropriate platform cmake module
# Note: name of cmake module MUST match CAMEL_PLATFORM_LOWER
include(system/platform/common)
include(system/platform/${CAMEL_PLATFORM_LOWER})

# Add appropriate compiler flags
# Note: name of cmake module MUST match CAMEL_COMPILER_FRONTEND_LOWER
include(system/frontend/${CAMEL_COMPILER_FRONTEND_LOWER})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}" CACHE INTERNAL "")