camel_library(
	FORCE_DISABLE
		${CAMEL_PLATFORM_LINUX}
	DEPENDENCIES
		dep_nvrhi
		module_core
)

# Note: you need to set LINUX_DXC_PATH to DXC on linux platforms.
# download: https://github.com/microsoft/DirectXShaderCompiler/releases
include(cmake/compile_hlsl.cmake)

