#include <core_sdk/logger.hpp>
#include <lib_rendering/shaders.hpp>

#include <cassert>

using namespace lib::rendering::gl3;

namespace
{
GLuint compile_shader(const char* shader_src, GLuint shader_type)
{
	const auto shader_id = glCreateShader(shader_type);

	// try to compile shader
	glShaderSource(shader_id, 1, &shader_src, nullptr);
	glCompileShader(shader_id);

	int result = 0;
	int log_length = 0;

	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

	// write any errors that occur when compiling shaders
	if (log_length > 0)
	{
		std::string error_message = {};
		error_message.resize(log_length + 1);

		glGetShaderInfoLog(shader_id, log_length, nullptr, &error_message[0]);
		lib_log_e("opengl3: could not compile shader: {}", error_message);

		assert(false);
	}

	return shader_id;
}
}  // namespace

shaders::shaders(const char* vertex_shader, const char* fragment_shader) : _shader_program_id(0)
{
	const auto vertex_shader_id = compile_shader(vertex_shader, GL_VERTEX_SHADER);
	const auto fragment_shader_id = compile_shader(fragment_shader, GL_FRAGMENT_SHADER);

	_shader_program_id = glCreateProgram();

	// bind the shader to the program
	glAttachShader(_shader_program_id, vertex_shader_id);
	glAttachShader(_shader_program_id, fragment_shader_id);

	// link everything
	glLinkProgram(_shader_program_id);
	glValidateProgram(_shader_program_id);

	// we can delete the shader now because its already bound and loaded into the program
	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);
}

shaders::~shaders()
{
	glDeleteProgram(_shader_program_id);
}

void shaders::bind() const
{
	glUseProgram(_shader_program_id);
}

GLint shaders::get_attribute_location(const char* attribute) const
{
	return glGetUniformLocation(_shader_program_id, attribute);
}