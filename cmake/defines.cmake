#
# Common includes
#
include(CMakeParseArguments)

#
# Platform independent CMake settings
#
set(CMAKE_NETRC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#
# Common definitions
#

# Spacer used in CMake logs
set(_CAMEL_CMAKE_SPACER "==============================================================================")

# Enable options prefix
set(_CAMEL_ENABLE_PREFIX "CAMEL_ENABLE_")

# Directory of project CMakeLists.txt
set(_CAMEL_CMAKE_TOP_LEVEL_DIR "${PROJECT_SOURCE_DIR}")

# Directory of main.cmake
set(_CAMEL_CMAKE_MAIN_DIR "${CMAKE_CURRENT_LIST_DIR}")

#
# Create a define interface that we can propagate to external targets
#

add_library(camel_defines INTERFACE)