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

include(CTest)
include(Catch)

enable_testing()