include(option/macro)

#
# Define options we can set.
#
message(STATUS ${_CAMEL_CMAKE_SPACER})
message(STATUS "Options:")

lib_option(CAMEL_INSTALL "Configure library for installation." ${PROJECT_IS_TOP_LEVEL})
lib_option(CAMEL_BUILD_ALL_TESTS "Include and build tests." ${PROJECT_IS_TOP_LEVEL})
lib_option(CAMEL_BUILD_ALL_COMPONENTS "Include and build all supported components." ${PROJECT_IS_TOP_LEVEL})
lib_option(CAMEL_BUILD_ALL_TOOLS "Include and build all supported components." ${PROJECT_IS_TOP_LEVEL})
