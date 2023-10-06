#pragma once
#include <backend/render/opengl3/utils/vertex_layout.hpp>
#include <memory>

namespace lib::backend::opengl3
{
class vertex_array_object
{
public:
	vertex_array_object(const vertex_layout& vertex_layout, GLuint size);
	~vertex_array_object();

	void bind() const;
	static void unbind();

private:
	GLuint _vertex_array_object;
	GLuint _vertex_buffer_object;
};
}  // namespace lib::backend::opengl3
