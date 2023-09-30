#pragma once
#include <glad/glad.h>

namespace lib::backend::opengl3
{
struct shader_t
{
	shader_t(const char* vertex_shader, const char* fragment_shader);
	~shader_t();

	GLuint shader_program_id = 0;
};
}  // namespace lib::backend::opengl3
