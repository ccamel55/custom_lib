#pragma once
#include <common/types/color.hpp>
#include <common/types/point/point2D.hpp>
#include <common/types/point/point4D.hpp>
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
