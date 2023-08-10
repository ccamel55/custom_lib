#include <backend/render/opengl3/core/vertex_manager.hpp>

using namespace lib::backend::gl3;

vertex_manager::vertex_manager() :
	_vertex_array_id(0), _vertex_buffer_id(0)
{
}

vertex_manager::vertex_manager(size_t max_vertices, uint32_t size_of_vertex, vertex_layout_manager& vertex_layout_manager)
{
	// create vertex array
	glGenVertexArrays(1, &_vertex_array_id);
	glGenBuffers(1, &_vertex_buffer_id);

	glBindVertexArray(_vertex_array_id);

	// create vertex buffer, since our array is bound, our buffer will be bound to the array
	glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer_id);

	// we will write to the buffer later, we just want to allcoate the space now
	glBufferData(GL_ARRAY_BUFFER, static_cast<uint32_t>(max_vertices) * size_of_vertex, nullptr, GL_DYNAMIC_DRAW);

	// while we are at it also bind the vertex layout to our array, so we just need to apply the vertex array on draw
	const auto& layout = vertex_layout_manager.get_layouts();

	for (unsigned int i = 0; i < layout.size(); i++)
	{
		const auto& param = layout.at(i);

		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, param._count, param._param_type, param._normalized, size_of_vertex, reinterpret_cast<void*>(param._offset));
	}

	// unbind, good practice
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

vertex_manager::~vertex_manager()
{
	glDeleteVertexArrays(1, &_vertex_array_id);
	glDeleteBuffers(1, &_vertex_buffer_id);
}

void vertex_manager::bind() const
{
	glBindVertexArray(_vertex_array_id);
}

void vertex_manager::unbind() const
{
	(void)_vertex_array_id;
	glBindVertexArray(0);
}

void vertex_manager::bind_buffer_data(const std::array<render::vertex_2d_t, render::MAX_VERTEX_COUNT>& vertices) const
{
	// memcpy the whole ass array into the vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer_id);

	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(render::vertex_2d_t), vertices.data());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}