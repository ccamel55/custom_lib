camel_library(
	DEPENDENCIES
		dep_nvrhi
		module_core
		module_logger
)

# Note: you need to set LINUX_DXC_PATH to DXC on linux platforms.
# download: https://github.com/microsoft/DirectXShaderCompiler/releases
include(cmake/compile_hlsl.cmake)

compile_hlsl(
    ${PROJECT_NAME}
        INPUT       "${CMAKE_CURRENT_LIST_DIR}/shader"
        OUTPUT      "${CMAKE_CURRENT_LIST_DIR}/include/module_render/shader"
        NAMESPACE   "shader"
)
