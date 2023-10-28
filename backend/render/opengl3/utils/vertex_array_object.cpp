#include <backend/render/opengl3/types/batch.hpp>
#include <backend/render/opengl3/utils/vertex_array_object.hpp>
#include <common/types/color.hpp>
#include <common/types/point/point2D.hpp>

using namespace lib::backend::opengl3;

vertex_array_object::vertex_array_object(GLuint size) :
	_vertex_array_object(0), _vertex_buffer(0)
{
	// create vertex array object
	{
		glGenVertexArrays(1, &_vertex_array_object);
		glBindVertexArray(_vertex_array_object);
	}

	// create vertex buffers and bind layout to VAO
	{
		// vertex buffer
		glGenBuffers(1, &_vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(size * sizeof(common::point2Df)), nullptr, GL_DYNAMIC_DRAW);

		// position
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(opengl3::vertex_t), reinterpret_cast<void*>(0));

		// color
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
		glVertexAttribPointer(
			1,
			4,
			GL_UNSIGNED_BYTE,
			GL_TRUE,
			sizeof(opengl3::vertex_t),
			reinterpret_cast<void*>(sizeof(common::point2Df)));

		// tex coord
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
		glVertexAttribPointer(
			2,
			2,
			GL_FLOAT,
			GL_FALSE,
			sizeof(opengl3::vertex_t),
			reinterpret_cast<void*>(sizeof(common::point2Df) + sizeof(common::color)));

		glBindVertexArray(0);
	}
}

vertex_array_object::~vertex_array_object()
{
	glDeleteVertexArrays(1, &_vertex_array_object);
	glDeleteBuffers(1, &_vertex_buffer);
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

GLuint vertex_array_object::get_vertex_buffer() const
{
	return _vertex_buffer;
}