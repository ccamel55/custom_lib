#include <backend/render/opengl3/utils/vertex_array_object.hpp>

using namespace lib::backend::opengl3;

vertex_array_object::vertex_array_object(
	std::unique_ptr<vertex_buffer_object> vertex_buffer_object, const vertex_layout& vertex_layout) :
	_vertex_array_object(0),
	_vertex_buffer_object(std::move(vertex_buffer_object))
{
	glGenVertexArrays(1, &_vertex_array_object);

	// map our VBO to the VAO, so we only need to apply the VAO to draw buffer
	glBindVertexArray(_vertex_array_object);
	_vertex_buffer_object->bind();

	// apply layout for the buffer
	uint32_t offset = 0;
	const auto& layout = vertex_layout.get_layout();

	for (size_t i = 0; i < layout.size(); i++)
	{
		const auto& curParam = layout.at(i);

		glEnableVertexAttribArray(i);
		glVertexAttribPointer(
			i,
			curParam.count,
			curParam.type,
			curParam.normalized,
			vertex_layout.get_stride(),
			reinterpret_cast<void*>(offset));

		offset += curParam.count * vertex_layout::size_of_gl_type(curParam.type);
	}

	glBindVertexArray(0);
}

vertex_array_object::~vertex_array_object()
{
	glDeleteVertexArrays(1, &_vertex_array_object);
	_vertex_buffer_object.reset();
}

void vertex_array_object::bind() const
{
	// start to save the state to VAO
	glBindVertexArray(_vertex_array_object);
}

void vertex_array_object::unbind()
{
	glBindVertexArray(0);
}