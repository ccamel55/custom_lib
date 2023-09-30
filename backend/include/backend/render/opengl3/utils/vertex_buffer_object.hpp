#pragma once
#include <glad/glad.h>

namespace lib::backend::opengl3
{
class vertex_buffer_object
{
public:
	vertex_buffer_object(const void* data, GLuint size);
	~vertex_buffer_object();

	void bind() const;
	static void unbind();

private:
	GLuint _vertex_buffer_object;
};
}  // namespace lib::backend::opengl3
