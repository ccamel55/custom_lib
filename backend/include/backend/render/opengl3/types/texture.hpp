#pragma once
#include <glad/glad.h>
#include <cstdint>

namespace lib::backend::opengl3
{
struct texture_t
{
	texture_t(const uint8_t* data, GLsizei width, GLsizei height, GLint format);
	~texture_t();

	GLuint texture_id = 0;
};
}  // namespace lib::backend::opengl3
