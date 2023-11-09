#pragma once

#include "glad/glad.hpp"
#include <string>

namespace lib::backend::render::gl3
{
class shaders
{
public:
	shaders(const char* vertex_shader, const char* fragment_shader);
	~shaders();

	void bind() const;

	GLint get_attribute_location(const char* attribute) const;

private:
	GLuint _shader_program_id;
};
}  // namespace lib::backend::render::gl3