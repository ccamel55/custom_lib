camel_library(
	DEPENDENCIES
		dep_nvrhi
		module_core
		module_logger
		module_system
)

# Note: you need to set LINUX_DXC_PATH to DXC on linux platforms.
# download: https://github.com/microsoft/DirectXShaderCompiler/releases
include(cmake/compile_hlsl.cmake)
compile_shaders(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/shader")
