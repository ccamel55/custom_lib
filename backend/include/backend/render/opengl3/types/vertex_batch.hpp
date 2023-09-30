#pragma once
#include <glad/glad.h>

namespace lib::backend::opengl3
{
struct vertex_batch_t
{
	GLenum primitive = 0;
	GLuint texture_id = 0;
	GLsizei vertex_count = 0;
	common::point4Di scissor_rect = {};
};
}  // namespace lib::backend::opengl3
