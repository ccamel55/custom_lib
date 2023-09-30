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
private:
	GLuint _stride = 0;
	std::vector<vertex_layout_data_t> _layout = {};
};

}  // namespace lib::backend::opengl3
