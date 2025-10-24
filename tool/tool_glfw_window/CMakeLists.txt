camel_executable(
	DEPENDENCIES
		dep_glfw
		module_core
		module_gui
		module_input
		module_logger
		module_render
)

target_compile_definitions(${PROJECT_NAME} PRIVATE "_DEBUG=$<IF:$<CONFIG:Debug>,1,0>")

# Get artifact output directory based on the binary build directory
# Runtime path will be <exe path>/shaders
set(COMPILED_SHADER_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders)
set(COMPILED_TEXTURE_DIR ${CMAKE_CURRENT_BINARY_DIR}/textures)

copy_shaders(${PROJECT_NAME} ${COMPILED_SHADER_DIR})
copy_textures(${PROJECT_NAME} ${COMPILED_TEXTURE_DIR})