#pragma once

#include <backend/render/api/render_api_base.hpp>

#include <array>
#include <filesystem>
#include <memory>

namespace lib::backend
{
//! Renderer interface, this will be used to send commands to our render API implementation
class renderer
{
public:
	~renderer();

	//! bind our render API to an existing render context
	void bind_api(void* api_context);

	//! remove our render API from an existing context and reset the renderer
	void unbind_api();

	//! add an image to the texture atlas to be drawn later
	void add_image(const std::filesystem::path& image);

	//! send render commands to render API and then get render API to draw them
	void draw_frame();

	//! update current clipped area for draws
	void update_clipped_area(const common::point4Di& clipped_area);

	void set_frame_time(float frame_time);
	[[nodiscard]] float get_frame_time() const;

	void set_window_size(const common::point2Di& window_size);
	[[nodiscard]] const common::point2Di& get_window_size() const;

public:
	void draw_image(
		const common::point2Di& pos,
		const common::point2Di& size,
		const common::color& color,
		render::texture_id texture_id);

	void draw_line(
		const common::point2Di& p1, const common::point2Di& p2, const common::color& color, float thickness = 1);

	void draw_triangle(
		const common::point2Di& p1,
		const common::point2Di& p2,
		const common::point2Di& p3,
		const common::color& color,
		float thickness = 1);

	void draw_triangle_filled(
		const common::point2Di& p1, const common::point2Di& p2, const common::point2Di& p3, const common::color& color);

	void draw_rectangle(
		const common::point2Di& pos, const common::point2Di& size, const common::color& color, float thickness = 1);

	void draw_rectangle_filled(const common::point2Di& pos, const common::point2Di& size, const common::color& color);

	void draw_rect_gradient_h_filled(
		const common::point2Di& pos, const common::point2Di& size, const common::color& c1, const common::color& c2);

	void draw_rect_gradient_v_filled(
		const common::point2Di& pos, const common::point2Di& size, const common::color& c1, const common::color& c2);

	void draw_rect_gradient_filled(
		const common::point2Di& pos,
		const common::point2Di& size,
		const common::color& c1,
		const common::color& c2,
		const common::color& c3,
		const common::color& c4);

private:
	common::point2Df opaque_texture_uv = {};

	//! frame time given in milliseconds
	float _frame_time = 0.1f;

	//! size of our window given in pixels, width x height
	common::point2Di _window_size = {};

	//! current clipped area we are drawing
	common::point4Di _clipped_area = {};

	//! render command is what we will pass into our render api
	render::render_command _render_command = {};

	//! pointer to our render api, this is what we will use to actually render our primitives
	std::unique_ptr<render::render_api_base> _render_api = nullptr;
};
}  // namespace lib::backend