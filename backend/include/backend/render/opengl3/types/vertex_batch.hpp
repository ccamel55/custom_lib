#pragma once
#include <glad/glad.h>

namespace lib::backend::opengl3
{
struct vertex_batch_t
{
	GLenum primitive;
	GLuint texture_id;
	uint32_t vertex_count;
};
}  // namespace lib::backend::opengl3
