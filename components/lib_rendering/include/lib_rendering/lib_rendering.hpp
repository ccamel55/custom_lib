#pragma once

#include <lib_rendering/common/font_loader.hpp>
#include <lib_rendering/common/image_loader.hpp>
#include <lib_rendering/render_api_base.hpp>

#include <array>
#include <filesystem>
#include <memory>
#include <unordered_map>

namespace lib::rendering
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
	texture_id add_image(const std::filesystem::path& image);

	//! send render commands to render API and then get render API to draw them
	void draw_frame();

	//! update current clipped area for draws
	void update_clipped_area(const lib::point4Di& clipped_area);

	void set_frame_time(float frame_time);
	[[nodiscard]] float get_frame_time() const;

	void set_window_size(const lib::point2Di& window_size);
	[[nodiscard]] const lib::point2Di& get_window_size() const;

public:
	void draw_image(
		const lib::point2Di& pos, const lib::point2Di& size, const lib::color& color, texture_id texture_id);

	void draw_line(const lib::point2Di& p1, const lib::point2Di& p2, const lib::color& color, float thickness = 1);

	void draw_triangle(
		const lib::point2Di& p1,
		const lib::point2Di& p2,
		const lib::point2Di& p3,
		const lib::color& color,
		float thickness = 1);

	void draw_triangle_filled(
		const lib::point2Di& p1, const lib::point2Di& p2, const lib::point2Di& p3, const lib::color& color);

	void draw_rectangle(
		const lib::point2Di& pos, const lib::point2Di& size, const lib::color& color, float thickness = 1);

	void draw_rectangle_filled(const lib::point2Di& pos, const lib::point2Di& size, const lib::color& color);

	void draw_rect_gradient_h_filled(
		const lib::point2Di& pos, const lib::point2Di& size, const lib::color& c1, const lib::color& c2);

	void draw_rect_gradient_v_filled(
		const lib::point2Di& pos, const lib::point2Di& size, const lib::color& c1, const lib::color& c2);

	void draw_rect_gradient_filled(
		const lib::point2Di& pos,
		const lib::point2Di& size,
		const lib::color& c1,
		const lib::color& c2,
		const lib::color& c3,
		const lib::color& c4);

private:
	//! used to draw color
	texture_id _opaque_texture_id = 0;

	//! contains texture properties for respective texture id, texture_id indexes into array
	std::vector<texture_properties_t> _texture_properties = {};

	//! frame time given in milliseconds
	float _frame_time = 0.1f;

	//! size of our window given in pixels, width x height
	point2Di _window_size = {};

	//! current clipped area we are drawing
	point4Di _clipped_area = {};

	//! render command is what we will pass into our render api
	render_command _render_command = {};

	//! pointer to our render api, this is what we will use to actually render our primitives
	std::unique_ptr<render_api_base> _render_api = nullptr;
};
}  // namespace lib::rendering