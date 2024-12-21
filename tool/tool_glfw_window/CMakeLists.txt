camel_executable(
	DEPENDENCIES
		dep_glfw
		module_core
		module_input
		module_logger
		module_render
		module_system
)

# Get artifact output directory based on the binary build directory
# Runtime path will be <exe path>/shaders
set(COMPILED_SHADER_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders)
copy_shaders(${PROJECT_NAME} ${COMPILED_SHADER_DIR})