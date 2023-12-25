#pragma once

#include <lib_rendering/common/font_loader.hpp>
#include <lib_rendering/common/image_loader.hpp>
#include <lib_rendering/common/atlas_generator.hpp>
#include <lib_rendering/render_api_base.hpp>

#include <array>
#include <filesystem>
#include <memory>
#include <unordered_map>

namespace lib::rendering
{
enum font_flags: uint8_t
{
	none = 0,
	left_aligned = 0 << 0,
	right_aligned = 1 << 0,
	centered_x = 1 << 1,
	centered_y = 1 << 2,
	centered_xy = centered_x | centered_y,
};

//! Renderer interface, this will be used to send commands to our render API implementation
class renderer
{
public:
	~renderer();

	//! bind our render API to an existing render context
	void bind_api(void* api_context);

	//! remove our render API from an existing context and reset the renderer
	void unbind_api();

	//! build our texture atlas. must be called before any drawing can be done
	void build_texture();

	//! add an image to the texture atlas to be drawn later
	[[nodiscard]] texture_id add_image(const std::filesystem::path& image);

	//! add font to texture atlas to be drawn later
	[[nodiscard]] font_id add_font(const uint8_t* font_data, float height);

	//! send render commands to render API and then get render API to draw them
	void draw_frame();

	//! update current clipped area for draws
	void update_clipped_area(const lib::point4Di& clipped_area);

	void set_frame_time(float frame_time);
	[[nodiscard]] float get_frame_time() const;

	void set_window_size(const lib::point2Di& window_size);
	[[nodiscard]] const lib::point2Di& get_window_size() const;

public:
	void draw_image(const lib::point2Di& pos,
                    const lib::point2Di& size,
                    const lib::color& color,
                    texture_id texture_id);

	void draw_line(const lib::point2Di& p1, const lib::point2Di& p2, const lib::color& color, float thickness = 1);

	void draw_triangle(
		const lib::point2Di& p1,
		const lib::point2Di& p2,
		const lib::point2Di& p3,
		const lib::color& color,
		float thickness = 1);

	void draw_triangle_filled(const lib::point2Di& p1,
                              const lib::point2Di& p2,
                              const lib::point2Di& p3,
                              const lib::color& color);

	void draw_rectangle(const lib::point2Di& pos,
                        const lib::point2Di& size,
                        const lib::color& color,
                        float thickness = 1);

	void draw_rectangle_filled(const lib::point2Di& pos, const lib::point2Di& size, const lib::color& color);

	void draw_rect_gradient_h_filled(const lib::point2Di& pos,
                                     const lib::point2Di& size,
                                     const lib::color& c1,
                                     const lib::color& c2);

	void draw_rect_gradient_v_filled(const lib::point2Di& pos,
                                     const lib::point2Di& size,
                                     const lib::color& c1,
                                     const lib::color& c2);

	void draw_rect_gradient_filled(const lib::point2Di& pos,
                                   const lib::point2Di& size,
                                   const lib::color& c1,
                                   const lib::color& c2,
                                   const lib::color& c3,
                                   const lib::color& c4);

	void draw_font(const lib::point2Di& pos,
				   const lib::color& color,
				   font_id font_id,
				   const std::string& text,
				   font_flags flags = font_flags::none);

#if DEF_LIB_RENDERING_EXPERIMENTAL_on
	void draw_font_outlined(const lib::point2Di& pos,
							const lib::color& color,
							const lib::color& outline_color,
							font_id font_id,
							const std::string& text,
							font_flags flags = font_flags::none);
#endif

private:
	//! atlas generator is used to generate a texture atlas
	atlas_generator _atlas_generator = {};

	//! font id wraps a texture id, this is very messy and dumb but it fast and works
	std::vector<font_properties_t> _font_properties = {};

	//! used to draw color
	texture_id _opaque_texture_id = 0;

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