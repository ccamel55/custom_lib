#pragma once

#include <core_sdk/types/bitflag.hpp>

#include <lib_rendering/render_api_base.hpp>
#include <lib_rendering/render_api.hpp>

#include <lib_rendering/render_callback_handler.hpp>
#include <lib_rendering/common/atlas_generator.hpp>

#include <array>
#include <filesystem>
#include <memory>
#include <functional>
#include <chrono>

namespace lib::rendering
{
//! Renderer interface, this will be used to send commands to our render API implementation
class renderer final : render_callback_handler
{
public:
	explicit renderer(const std::weak_ptr<render_api_data_t>& render_api_data, bool flush_buffers);
	~renderer() override;

	//! build our texture atlas. must be called before any drawing can be done
	void build_texture();

	//! add an image to the texture atlas to be drawn later
	[[nodiscard]] texture_id add_image(const std::filesystem::path& image);

	//! add font to texture atlas to be drawn later
	[[nodiscard]] font_id add_font(const uint8_t* font_data, float height);

	//! send render commands to render API and then get render API to draw them
	void draw_frame();

	//! register a callback to populate the render command.
	void register_callback(std::function<void(render_callback_handler&)>&& callback);

	//! set the size of the rendering window, this will also update the renderer
	void set_window_size(const lib::point2Di& window_size);
	[[nodiscard]] const lib::point2Di& get_window_size() const;

	//! Get the time between renderer frame updates.
	[[nodiscard]] float get_frame_time_ms() const override;

protected:
	void update_clipped_area(const lib::point4Di& clipped_area) override;

	void draw_image(
		const lib::point2Di& pos,
		const lib::point2Di& size,
        const lib::color& color,
        texture_id texture_id) override;

	void draw_line(
		const lib::point2Di& p1,
		const lib::point2Di& p2,
		const lib::color& color,
		float thickness) override;

	void draw_triangle(
		const lib::point2Di& p1,
		const lib::point2Di& p2,
		const lib::point2Di& p3,
		const lib::color& color,
		float thickness) override;

	void draw_triangle_filled(
		const lib::point2Di& p1,
        const lib::point2Di& p2,
        const lib::point2Di& p3,
        const lib::color& color) override;

	void draw_rect(
		const lib::point2Di& pos,
		const lib::point2Di& size,
		const lib::color& color,
		float thickness) override;

	void draw_rect_filled(
		const lib::point2Di& pos,
		const lib::point2Di& size,
		const lib::color& color) override;

	void draw_rect_gradient_h_filled(
		const lib::point2Di& pos,
		const lib::point2Di& size,
		const lib::color& c1,
		const lib::color& c2) override;

	void draw_rect_gradient_v_filled(
		const lib::point2Di& pos,
		const lib::point2Di& size,
		const lib::color& c1,
		const lib::color& c2) override;

	void draw_rect_gradient_filled(
		const lib::point2Di& pos,
        const lib::point2Di& size,
        const lib::color& c1,
        const lib::color& c2,
        const lib::color& c3,
        const lib::color& c4) override;

	void draw_font(
		const lib::point2Di& pos,
		const lib::color& color,
		font_id font_id,
		const std::string& text,
		bitflag flags) override;

private:
	//! atlas generator is used to generate a texture atlas
	atlas_generator _atlas_generator = {};

	//! font id wraps a texture id, this is very messy and dumb but it fast and works
	std::vector<font_properties_t> _font_properties = {};

	//! used to draw color
	texture_id _opaque_texture_id = 0;

	//! frame time given in milliseconds
	float _frame_time = 0.f;

	std::chrono::system_clock::time_point _last_frame_time = std::chrono::system_clock::now();

	//! size of our window given in pixels, width x height
	point2Di _window_size = {};

	//! current clipped area we are drawing
	point4Di _clipped_area = {};

	//! render command is what we will pass into our render api
	render_command _render_command = {};

	//! pointer to our render api, this is what we will use to actually render our primitives
	std::unique_ptr<render_api_base> _render_api = nullptr;

	//! Callbacks that populate the render command
	std::vector<std::function<void(render_callback_handler&)>> _render_callbacks = {};
};
}  // namespace lib::rendering