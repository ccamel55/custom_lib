#pragma once
#include <cstdint>

namespace lib::backend::opengl3
{
struct vertex_t
{
	// position
	float x;
	float y;

	// color
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;

	// tex-coords
	float u;
	float v;
};
}  // namespace lib::backend::opengl3
