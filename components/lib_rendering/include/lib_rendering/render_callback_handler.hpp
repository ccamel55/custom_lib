#pragma once

namespace lib::rendering
{
enum font_flags: bitflag_t
{
	none = 0,
	left_aligned = 0 << 0,
	right_aligned = 1 << 0,
	centered_x = 1 << 1,
	centered_y = 1 << 2,
	centered_xy = centered_x | centered_y,
	outline = 1 << 3,
};

//! Interface for interacting with the render handler.
class render_callback_handler
{
public:
    virtual ~render_callback_handler() = default;

	[[nodiscard]] virtual float get_frame_time_ms() const = 0;
	virtual void update_clipped_area(const lib::point4Di& clipped_area) = 0;

	virtual void draw_image(
		const lib::point2Di& pos,
		const lib::point2Di& size,
		const lib::color& color,
		texture_id texture_id) = 0;

	virtual void draw_line(
		const lib::point2Di& p1,
		const lib::point2Di& p2,
		const lib::color& color,
		float thickness) = 0;

	virtual void draw_triangle(
		const lib::point2Di& p1,
		const lib::point2Di& p2,
		const lib::point2Di& p3,
		const lib::color& color,
		float thickness) = 0;

	virtual void draw_triangle_filled(
		const lib::point2Di& p1,
		const lib::point2Di& p2,
		const lib::point2Di& p3,
		const lib::color& color) = 0;

	virtual void draw_rect(
		const lib::point2Di& pos,
		const lib::point2Di& size,
		const lib::color& color,
		float thickness) = 0;

	virtual void draw_rect_filled(
		const lib::point2Di& pos,
		const lib::point2Di& size,
		const lib::color& color) = 0;

	virtual void draw_rect_gradient_h_filled(
		const lib::point2Di& pos,
		const lib::point2Di& size,
		const lib::color& c1,
		const lib::color& c2) = 0;

	virtual void draw_rect_gradient_v_filled(
		const lib::point2Di& pos,
		const lib::point2Di& size,
		const lib::color& c1,
		const lib::color& c2) = 0;

	virtual void draw_rect_gradient_filled(
		const lib::point2Di& pos,
		const lib::point2Di& size,
		const lib::color& c1,
		const lib::color& c2,
		const lib::color& c3,
		const lib::color& c4) = 0;

	virtual void draw_font(
		const lib::point2Di& pos,
		const lib::color& color,
		font_id font_id,
		const std::string& text,
		bitflag flags) = 0;
};
}