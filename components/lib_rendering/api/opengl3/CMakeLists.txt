add_subdirectory(dependencies/glad)

lib_sub_component(
    PARENT
        lib_rendering
    SOURCES
        render_api.cpp
        render_state.cpp
        shaders.cpp
    INCLUDES
        include
    LINK_LIBS
        glad
)

# setup pre build python script
add_custom_target(
	parse_shader_files ALL
	COMMAND
		${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/python/parse_text_file.py
		-i ${CMAKE_CURRENT_LIST_DIR}/shaders
		-o ${CMAKE_CURRENT_LIST_DIR}/include/lib_rendering/shaders
		-n lib::rendering::gl3::shaders
	COMMENT
		"Parsing OpenGL shaders"
)

add_dependencies(lib_rendering parse_shader_files)