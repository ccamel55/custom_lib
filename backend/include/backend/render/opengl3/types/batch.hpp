#pragma once
#include <array>
#include <backend/render/opengl3/types/vertex_batch.hpp>
#include <cstddef>

namespace lib::backend::opengl3
{
// todo: find optimal max vertices.
constexpr auto MAX_VERTICES = 15000;

struct vertex_t
{
	vertex_t() = default;
	vertex_t(
		const common::point2Df& position, const common::color& color, const common::point2Df& texture_coordinates) :
		position(position),
		color(color),
        texture_coordinates(texture_coordinates)
	{
	}

	common::point2Df position;
	common::color color;
	common::point2Df texture_coordinates;
};

struct batch_t
{
	// used to get index of next entry
	uint16_t vertex_count = 0;
	uint16_t batch_count = 0;

	std::array<vertex_t, MAX_VERTICES> vertices = {};
	std::array<vertex_batch_t, MAX_VERTICES / 3> vertex_batch = {};
};
}  // namespace lib::backend::opengl3
