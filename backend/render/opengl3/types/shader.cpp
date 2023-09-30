#include <backend/render/opengl3/types/shader.hpp>
#include <common/logger.hpp>
#include <cassert>
#include <string>

using namespace lib::backend::opengl3;

namespace
{
GLuint compile_shader(const char* shader_src, GLuint shader_type)
{
	const auto shader_id = glCreateShader(shader_type);

	// try to compile shader
	glShaderSource(shader_id, 1, &shader_src, nullptr);
	glCompileShader(shader_id);

	int result = 0;
	int logLength = 0;

	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &logLength);

	// write any errors that occur when compiling shaders
	if (logLength > 0)
	{
		std::string error_message = {};
		error_message.resize(logLength + 1);

		glGetShaderInfoLog(shader_id, logLength, nullptr, &error_message[0]);
		std::cout << &error_message[0] << std::endl;

		lib_log_e("opengl3: could not compile shader: " + error_message);
		assert(false);
	}

	return shader_id;
}
}

shader_t::shader_t(const char* vertex_shader, const char* fragment_shader)
{
	const auto vertex_shader_id = compile_shader(vertex_shader, GL_VERTEX_SHADER);
	const auto fragment_shader_id = compile_shader(fragment_shader, GL_FRAGMENT_SHADER);

	shader_program_id = glCreateProgram();

	// bind the shader to the program
	glAttachShader(shader_program_id, vertex_shader_id);
	glAttachShader(shader_program_id, fragment_shader_id);

	// link everything
	glLinkProgram(shader_program_id);
	glValidateProgram(shader_program_id);

	// we can delete the shader now because its already bound and loaded into the program
	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);
}

shader_t::~shader_t()
{
	glDeleteProgram(shader_program_id);
}