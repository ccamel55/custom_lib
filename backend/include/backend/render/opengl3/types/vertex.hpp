#pragma once
#include <common/types/color.hpp>
#include <common/types/point/point4D.hpp>
#include <cstdint>

namespace lib::backend::opengl3
{
struct vertex_t
{
	vertex_t() = default;
	vertex_t(int x, int y, const common::color& color, float u, float v) :
		x(static_cast<float>(x)), y(static_cast<float>(y)), color(color), u(u), v(v)
	{
	}

	// position
	float x = 0.f;
	float y = 0.f;

	// color
	common::color color = {};

	// tex-coords
	float u = 0.f;
	float v = 0.f;
};
}  // namespace lib::backend::opengl3
