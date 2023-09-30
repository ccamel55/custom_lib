#pragma once
#include <common/types/color.hpp>
#include <common/types/point/point4D.hpp>
#include <cstdint>

namespace lib::backend::opengl3
{
struct vertex_t
{
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
