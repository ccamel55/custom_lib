#include <backend/render/opengl3/utils/vertex_buffer_object.hpp>

using namespace lib::backend::opengl3;

// objects in opengl store the state of something, therefore this object stores the state of the vertex buffer
vertex_buffer_object::vertex_buffer_object(const void* data, GLuint size) : _vertex_buffer_object(0)
{
	glGenBuffers(1, &_vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer_object);

	// dynamic draw because we will likely be writing data to it in the future
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

vertex_buffer_object::~vertex_buffer_object()
{
	glDeleteBuffers(1, &_vertex_buffer_object);
}

void vertex_buffer_object::bind() const
{
	// specify the buffer type then the id of the buffer
	// when we bind the buffer we say hey, save the state from now onwards to the VBO id
	glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer_object);
}

void vertex_buffer_object::unbind()
{
	// unbind buffer object, when we unbind we say hey, ok were done, this is the state
	// for this VBO id, now restore the state before binding VBO id
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}