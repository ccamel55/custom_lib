#include <backend/render/opengl3/core/vertex_layout_manager.hpp>

using namespace lib::backend::gl3;

void vertex_layout_manager::add_layout(uint32_t count, GLenum type, bool normalized)
{
	_layouts.emplace_back(count, _current_offset, type, normalized);

	switch (type)
	{
	case GL_INT:
		_current_offset += (count * sizeof(int));
		break;
	case GL_FLOAT:
		_current_offset += (count * sizeof(float));
		break;
	default:
		break;
	}
}

const std::vector<vertex_layout_t>& vertex_layout_manager::get_layouts() const
{
	return _layouts;
}