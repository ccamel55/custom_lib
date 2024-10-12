#
# Add catch 2 from repository
#
CPMAddPackage(
	NAME Catch2
	GITHUB_REPOSITORY catchorg/Catch2
	VERSION 3.7.1
)

# Add catch 2 cmake scripts
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)

# Not actually needed since catch2 isn't top level project
set(CATCH_INSTALL_DOCS OFF)
set(CATCH_INSTALL_EXTRAS OFF)
set(CATCH_DEVELOPMENT_BUILD OFF)

include(CTest)
include(Catch)

enable_testing()