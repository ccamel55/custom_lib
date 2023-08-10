#include <backend/render/opengl3/core/shader_manager.hpp>

using namespace lib::backend::gl3;

shader_manager::shader_manager() :
	_shader_id(0)
{
}

shader_manager::shader_manager(const std::string& vertex_shader, const std::string& fragment_shader)
{
	const auto vertex_shader_id = gen_shader(vertex_shader, GL_VERTEX_SHADER);
	const auto fragment_shader_id = gen_shader(fragment_shader, GL_FRAGMENT_SHADER);

	_shader_id = glCreateProgram();

	// bind shaders to our current shader
	glAttachShader(_shader_id, vertex_shader_id);
	glAttachShader(_shader_id, fragment_shader_id);

	glLinkProgram(_shader_id);
	glValidateProgram(_shader_id);

	// we are done so nuke them
	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);
}

shader_manager::~shader_manager()
{
	glDeleteProgram(_shader_id);
}

void shader_manager::bind() const
{
	glUseProgram(_shader_id);
}

void shader_manager::unbind() const
{
	(void)_shader_id;
	glUseProgram(0);
}

void shader_manager::bind_uniform_mat4(const std::string& uniform_name, const glm::mat4& matrix) const
{
	glUseProgram(_shader_id);

	const auto uniform_id = glGetUniformLocation(_shader_id, uniform_name.c_str());
	glUniformMatrix4fv(uniform_id, 1, GL_FALSE, &matrix[0][0]);

	glUseProgram(0);
}

void shader_manager::bind_uniform(const std::string& uniform_name, int v0) const
{
	glUseProgram(_shader_id);

	const auto uniform_id = glGetUniformLocation(_shader_id, uniform_name.c_str());
	glUniform1i(uniform_id, v0);

	glUseProgram(0);
}

uint32_t shader_manager::gen_shader(const std::string& shader, uint32_t type)
{
	const auto shader_char_str = shader.c_str();
	const auto current_id = glCreateShader(type);

	glShaderSource(current_id, 1, &shader_char_str, nullptr);
	glCompileShader(current_id);

	int result = 0;
	glGetShaderiv(current_id, GL_COMPILE_STATUS, &result);

	if (result != 1)
	{
		// throw your error here
		glDeleteShader(current_id);
		return 0;
	}

	return current_id;
}