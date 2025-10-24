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
# Todo: allow configure of top level stuff (helpful: https://stackoverflow.com/a/67568372)
#

# Spacer used in CMake logs
set(_CAMEL_CMAKE_SPACER "==============================================================================")

# Enable options prefix
set(_CAMEL_ENABLE_PREFIX "CAMEL_ENABLE_")

# Directory of main.cmake
set(_CAMEL_CMAKE_MAIN_DIR "${CMAKE_CURRENT_LIST_DIR}")

# Directory of top level CMakeLists.txt in tree
set(_CAMEL_CMAKE_TOP_LEVEL_DIR "${CMAKE_SOURCE_DIR}")

# Directory of custom_lib CMakeLists.txt in tree
cmake_path(GET _CAMEL_CMAKE_MAIN_DIR PARENT_PATH _CAMEL_CMAKE_DIR)


#
# Create a define interface that we can propagate to external targets
#

add_library(camel_defines INTERFACE)

#
# Do some hacky stuff to help with editors that rely on .clangd
#

# Make a copy of .clangd and put in top level directory
configure_file(
    "${_CAMEL_CMAKE_MAIN_DIR}/template/.clangd.in"
    "${_CAMEL_CMAKE_TOP_LEVEL_DIR}/.clangd"
    @ONLY
)
