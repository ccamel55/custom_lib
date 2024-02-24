#pragma once

#include <glad/glad.hpp>

namespace lib::rendering::gl3
{
class shader_module
{
public:
	~shader_module();

	void create(const char* vertex_shader, const char* fragment_shader);
	void bind() const;

	GLint get_attribute_location(const char* attribute) const;

private:
	GLuint _shader_program_id = 0;
};
}  // namespace lib::rendering::gl3