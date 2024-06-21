# Custom defined options
option(
	LIB_ENABLE_TESTS
	"Register module and dependency tests"
	OFF
)

option(
	LIB_VULKAN_SUPPORT
	"Whether or not the platform supports Vulkan"
	OFF
)

option(
	LIB_OPENGL_SUPPORT
	"Whether or not the platform supports OpenGL"
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
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "------------------------------------------")
message(STATUS "Options:")

message(STATUS "	CMAKE_C_STANDARD: ${CMAKE_C_STANDARD}")
message(STATUS "	CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
message(STATUS "	CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

message(STATUS "	LIB_INSTALL_DEPENDENCIES: ${LIB_INSTALL_DEPENDENCIES}")
message(STATUS "	LIB_INSTALL_MODULES: ${LIB_INSTALL_MODULES}")

message(STATUS "	LIB_ENABLE_TESTS: ${LIB_ENABLE_TESTS}")
message(STATUS "	LIB_VULKAN_SUPPORT: ${LIB_VULKAN_SUPPORT}")
message(STATUS "	LIB_OPENGL_SUPPORT: ${LIB_OPENGL_SUPPORT}")
