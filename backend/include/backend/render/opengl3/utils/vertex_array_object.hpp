#pragma once
#include <glad/glad.h>

namespace lib::backend::opengl3
{
class vertex_array_object
{
public:
	explicit vertex_array_object(GLuint size);
	~vertex_array_object();

	[[nodiscard]] GLuint get_vertex_buffer() const;

	void bind() const;
	static void unbind();

private:
	GLuint _vertex_array_object;
	GLuint _vertex_buffer;
};
}  // namespace lib::backend::opengl3
