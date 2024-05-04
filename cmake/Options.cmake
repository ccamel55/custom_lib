# Custom defined options
option(
	LIB_ENABLE_TESTS
	"Register module and dependency tests"
	OFF
)

# Apply C++ standard options
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "------------------------------------------")
message(STATUS "Options:")

message(STATUS "	CMAKE_C_STANDARD: ${CMAKE_C_STANDARD}")
message(STATUS "	CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
message(STATUS "	CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "	LIB_ENABLE_TESTS: ${LIB_ENABLE_TESTS}")
