camel_library(
	DEPENDENCIES
		dep_nvrhi
		module_core
		module_logger
		module_system
)

include(cmake/compile_shaders.cmake)
compile_shaders(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/include/module_render/shaders")
