# Custom defined options
option(
	LIB_ENABLE_TESTS
	"Register module and dependency tests"
	OFF
)

option(
	LIB_INSTALL_DEPENDENCIES
	"Allow installation of dependencies"
	OFF
)

option(
	LIB_INSTALL_MODULES
	"Allow installation of modules"
	OFF
)

# Apply C++ standard options
set(CMAKE_C_STANDARD 17 CACHE INTERNAL "")
set(CMAKE_CXX_STANDARD 23 CACHE INTERNAL "")
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE INTERNAL "")
set(CMAKE_CXX_EXTENSIONS OFF CACHE INTERNAL "")

include(Platform)

message(STATUS "------------------------------------------")
message(STATUS "Properties:")
message(STATUS "	CMAKE_C_STANDARD: ${CMAKE_C_STANDARD}")
message(STATUS "	CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
message(STATUS "	CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "	CURRENT_ARCH: ${CURRENT_ARCH}")
message(STATUS "	CURRENT_PLATFORM: ${CURRENT_PLATFORM}")
message(STATUS "Options:")
message(STATUS "	LIB_INSTALL_DEPENDENCIES: ${LIB_INSTALL_DEPENDENCIES}")
message(STATUS "	LIB_INSTALL_MODULES: ${LIB_INSTALL_MODULES}")
message(STATUS "	LIB_ENABLE_TESTS: ${LIB_ENABLE_TESTS}")

