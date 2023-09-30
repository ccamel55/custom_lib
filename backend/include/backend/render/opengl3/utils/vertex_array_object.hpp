#pragma once
#include <backend/render/opengl3/utils/vertex_buffer_object.hpp>
#include <backend/render/opengl3/utils/vertex_layout.hpp>
#include <memory>

namespace lib::backend::opengl3
{
class vertex_array_object
{
public:
	vertex_array_object(std::unique_ptr<vertex_buffer_object> vertex_buffer_object, const vertex_layout& vertex_layout);
	~vertex_array_object();

	void bind() const;
	static void unbind();

private:
	GLuint _vertex_array_object;
	std::unique_ptr<vertex_buffer_object> _vertex_buffer_object = nullptr;
};
}  // namespace lib::backend::opengl3
