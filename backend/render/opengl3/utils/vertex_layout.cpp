#include <backend/render/opengl3/utils/vertex_layout.hpp>

using namespace lib::backend::opengl3;

void vertex_layout::add_parameter(GLint size, GLuint type, GLuint normalized)
{
	_layout.emplace_back(size, type, normalized);
	_stride += size * size_of_gl_type(type);
}

GLint vertex_layout::get_stride() const
{
	return _stride;
}

const std::vector<vertex_layout_data_t>& vertex_layout::get_layout() const
{
	return _layout;
}

GLint vertex_layout::size_of_gl_type(GLuint type)
{
	switch (type)
	{
	case GL_INT:
		return sizeof(GLint);
	case GL_FLOAT:
		return sizeof(GLfloat);
	case GL_UNSIGNED_BYTE:
		return sizeof(GLubyte);
	default:
		break;
	}

	return 0;
}
