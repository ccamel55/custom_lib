#pragma once
#include <glad/glad.h>
#include <vector>

namespace lib::backend::opengl3
{
struct vertex_layout_data_t
{
	GLint count;
	GLuint type;
	GLuint normalized;
};

class vertex_layout
{
public:
	void add_parameter(GLint size, GLuint type, GLuint normalized);
	static GLint size_of_gl_type(GLuint type);

	[[nodiscard]] GLint get_stride() const;
	[[nodiscard]] const std::vector<vertex_layout_data_t>& get_layout() const;

private:
	GLint _stride = 0;
	std::vector<vertex_layout_data_t> _layout = {};

};

}  // namespace lib::backend::opengl3
