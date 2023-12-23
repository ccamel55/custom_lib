#pragma once

#include <core_sdk/types/color.hpp>
#include <core_sdk/types/point/point2D.hpp>
#include <core_sdk/types/point/point4D.hpp>

#include <array>

namespace lib::rendering
{
//! defines the maximum number of vertices we can render at a time
constexpr uint32_t MAX_VERTICES = 419430;

//! defines the maximum number of indices we can draw at a time
constexpr uint32_t MAX_INDICES = MAX_VERTICES * 3;

//! layout of each point, we only draw using triangles
struct vertex_t
{
	vertex_t() = default;

	vertex_t(const lib::point2Df& position,
			 const lib::color& color,
			 const lib::point2Df& texture_position)
		: position(position), color(color), texture_position(texture_position)
	{
	}

	// 2 x float
	lib::point2Df position = {};

	// 4 x unsigned byte
	lib::color color = {};

	// 2 x float
	lib::point2Df texture_position = {};
};

// ensure nothing funky happens on different architectures
static_assert(sizeof(vertex_t) == 20);

//! texture ID used to identify a texture and get it's properties
using texture_id = uint8_t;

struct batch_t
{
	batch_t() = default;

	explicit batch_t(const lib::point4Di& clipped_area) : clipped_area(clipped_area), texture_id(0)
	{
	}

	// start point from index array
	uint32_t offset = 0;

	// how many indices to read from index array
	uint32_t count = 0;

	// where should we clip the drawing space to
	lib::point4Di clipped_area = {};

	// what texture we should be using
	texture_id texture_id = {};
};

struct texture_properties_t
{
	// texture start location in pixels
	lib::point2Di start_pixel = {};

	// texture size in pixels
	lib::point2Di size_pixel = {};

	// texture start normalised
	lib::point2Df start_normalised = {};

	// texture end normalised
	lib::point2Df end_normalised = {};
};

struct font_data_t
{

};
}  // namespace lib::rendering