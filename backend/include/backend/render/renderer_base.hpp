#pragma once

#include <common/hash/fnv1a.hpp>
#include <common/logger.hpp>
#include <common/types/bitflag.hpp>
#include <common/types/color.hpp>
#include <common/types/point/point2D.hpp>
#include <common/types/point/point4D.hpp>

#include <array>
#include <cmath>
#include <string>

namespace lib::backend
{
constexpr uint8_t CIRCLE_CACHE_SEGMENTS = 64;

enum render_flags : common::bitflag_t
{
	render_flags_none = 0 << 0,
	render_flags_outline = 1 << 0,
	render_flags_center_x = 1 << 1,
	render_flags_center_y = 1 << 2,
	render_flags_gradient_horizontal = 1 << 3,
	render_flags_gradient_vertical = 1 << 4,
};

struct circle_cache_t
{
	float sin = 0.f;
	float cos = 0.f;
};

class renderer_base
{
public:
	renderer_base()
	{
		// cache circle segments
		for (size_t i = 0; i <= CIRCLE_CACHE_SEGMENTS; i++)
		{
			_circle_cache.at(i).sin =
				std::sin(6.283185f * (static_cast<float>(i) / static_cast<float>(CIRCLE_CACHE_SEGMENTS)));

			_circle_cache.at(i).cos =
				std::cos(6.283185f * (static_cast<float>(i) / static_cast<float>(CIRCLE_CACHE_SEGMENTS)));
		}
	}

	//! returns the cos/sin value of a particular circle segment
	[[nodiscard]] const circle_cache_t& get_circle_cache(uint8_t segment) const
	{
		return _circle_cache.at(segment);
	}

	//! returns the time between frames
	void set_frame_time(float frame_time)
	{
		_frame_time = frame_time;
	}

	//! returns the time between frames
	[[nodiscard]] float get_frame_time() const
	{
		return _frame_time;
	}

	void set_window_size(const common::point2Di& window_size)
	{
		_window_size = window_size;
		lib_log_d(fmt::format("renderer: updated window size, width {} height {}", window_size._x, window_size._y));
	}

	//! returns the size of the window the renderer is running in
	[[nodiscard]] const common::point2Di& get_window_size() const
	{
		return _window_size;
	}

public:
	//! create an instance of the rendering context
	virtual void init_instance() = 0;

	//! destroys current render instance
	virtual void destroy_instance() = 0;

	//! bind the render context to the renderer, used when not calling \a init_instance
	virtual void bind_context(void* context) = 0;

	//! removes the render context, used when not calling \a destroy_context
	virtual void remove_context() = 0;

	//! resets the render context
	virtual void reset() = 0;

	//! called before starting drawing in draw loop
	virtual void render_start() = 0;

	//! called at the end of drawing in draw loop
	virtual void render_finish() = 0;

	//! create a new font with the given param, \param font_hash is the name of the font hashed to fn1v
	virtual void add_font(common::fnv1a_t font_hash, const std::string& font_name, size_t height, size_t weight) = 0;

	//! draw a string
	virtual void draw_string(
		common::fnv1a_t font_hash,
		const common::point2Di& pos,
		const common::color& color,
		const std::string& string,
		render_flags flags) = 0;

	//! draw a rectangle
	virtual void draw_rect(const common::point4Di& area, const common::color& color, render_flags flags) = 0;

	//! draw a gradient rectangle
	virtual void draw_rect_gradient(
		const common::point4Di& area, const common::color& color1, const common::color& color2, render_flags flags) = 0;

	//! draw a filled rectangle
	virtual void draw_rect_filled(const common::point4Di& area, const common::color& color, render_flags flags) = 0;

	//! draw a filled gradient rectangle
	virtual void draw_rect_filled_gradient(
		const common::point4Di& area, const common::color& color1, const common::color& color2, render_flags flags) = 0;

	//! draw a circle
	virtual void draw_circle(
		const common::point2Di& pos, float radius, const common::color& color, render_flags flags) = 0;

	//! draw a filled circle
	virtual void draw_circle_filled(
		const common::point2Di& pos, float radius, const common::color& color, render_flags flags) = 0;

	//! draw a filled gradient circle
	virtual void draw_circle_filled_gradient(
		const common::point2Di& pos,
		float radius,
		const common::color& color1,
		const common::color& color2,
		render_flags flags) = 0;

	//! draw a triangle
	virtual void draw_triangle(
		const common::point2Di& pos1,
		const common::point2Di& pos2,
		const common::point2Di& pos3,
		const common::color& color,
		render_flags flags) = 0;

	//! draw a filled triangle
	virtual void draw_triangle_filled(
		const common::point2Di& pos1,
		const common::point2Di& pos2,
		const common::point2Di& pos3,
		const common::color& color,
		render_flags flags) = 0;

protected:
	float _frame_time = 0.f;
	bool _created_instance = false;

	common::point2Di _window_size = {};
	std::array<circle_cache_t, CIRCLE_CACHE_SEGMENTS + 1> _circle_cache = {};
};
}  // namespace lib::backend