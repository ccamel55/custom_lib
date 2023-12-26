#include <lib_rendering/lib_rendering.hpp>
#include <lib_rendering/render_api.hpp>

#include <core_sdk/logger.hpp>
#include <core_sdk/types/vector/vector2D.hpp>

#include <cassert>

using namespace lib::rendering;

namespace
{
// sometimes if this is too small we might end up with some weird artifacts
constexpr uint8_t opaque_texture_width = 2;
constexpr uint8_t opaque_texture_height = 2;

constexpr int render_api_texture_id = 0;
}  // namespace


renderer::~renderer()
{
	if (_render_api)
	{
		// we don't need to explicitly do this but for sake of readability
		_render_api.reset();
	}
}

void renderer::bind_api(void* api_context)
{
	(void)api_context;

	if (_render_api)
	{
		lib_log_w("renderer: tried to register new api context with existing context");
		return;
	}

#ifdef DEF_LIB_RENDERING_gl3
	if (gladLoadGL() == 0)
	{
		lib_log_e("renderer: could not load opengl");
		assert(false);
	}

	GLint major = 0;
	GLint minor = 0;

	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	lib_log_d("renderer:: opengl version {}.{}", major, minor);
#endif

	auto opaque_texture_data = std::array<uint8_t, opaque_texture_width * opaque_texture_height * 4>{};
	std::fill(opaque_texture_data.begin(), opaque_texture_data.end(), 0xff);

	_render_api = std::make_unique<render_api>();
	_opaque_texture_id = _atlas_generator.add_texture(
		opaque_texture_data.data(),
		opaque_texture_width,
		opaque_texture_height
		);
}

void renderer::unbind_api()
{
	if (!_render_api)
	{
		return;
	}

	// call destructor
	_render_api.reset();
	_render_command.reset();
	_atlas_generator.reset();
}

texture_id renderer::add_image(const std::filesystem::path& image)
{
	const auto loader = image_loader(image);
	const auto id = _atlas_generator.add_texture(
		loader.get_byte_buffer(),
		loader.get_width(), loader.
		get_height()
		);

	return id;
}

font_id renderer::add_font(const uint8_t* font_data, float height)
{
	auto& font_properties= _font_properties.emplace_back();

	const auto id = static_cast<font_id>(_font_properties.size() - 1);
	const auto font = font_loader(font_properties, font_data, height);

	// for each printable character add to our texture atlas
	for (uint8_t c = 32; c < 127; c++)
	{
		const auto& font_internal_property = font.get_font_internal_property(c);
		auto& font_property = font_properties.at(c - 32);

		if (font_internal_property.size._x == 0 || font_internal_property.size._y == 0)
		{
			// dont really know what to do here, rather than crash map a white square
			font_property.id = _opaque_texture_id;
			continue;
		}

		const auto texture_id = _atlas_generator.add_texture(
			font_internal_property.data,
			font_internal_property.size._x,
			font_internal_property.size._y
			);

		font_property.id = texture_id;
	}

	return id;
}

void renderer::build_texture()
{
	if (!_atlas_generator.build_atlas())
	{
		lib_log_w("renderer: did not build texture atlas");
		return;
	}

	// add to render API as one giant texture,
	// note: render_api_texture_id != texture_id
	_render_api->bind_atlas(
		_atlas_generator.get_byte_buffer(),
		_atlas_generator.get_width(),
		_atlas_generator.get_height());
}

void renderer::draw_frame()
{
	// get our render api to draw our vertices
	_render_api->draw_render_command(_render_command);

	// flush command, reset states back to default
	_clipped_area = {0, 0, _window_size._x, _window_size._y};
	_render_command.reset();
}

void renderer::set_frame_time(float frame_time)
{
	_frame_time = frame_time;
}

float renderer::get_frame_time() const
{
	return _frame_time;
}

void renderer::set_window_size(const lib::point2Di& window_size)
{
	_window_size = window_size;
	_clipped_area = {0, 0, window_size._x, window_size._y};

	_render_api->update_screen_size(window_size);
}

const lib::point2Di& renderer::get_window_size() const
{
	return _window_size;
}

void renderer::draw_image(const lib::point2Di& pos,
                          const lib::point2Di& size,
                          const lib::color& color,
                          texture_id texture_id)
{
	const auto vertex_index = _render_command.prepare_batch(_clipped_area, shader_type::normal);
	const auto vertex_iterator = _render_command.insert_vertices(4);

	vertex_iterator[0].position = {pos._x, pos._y};
	vertex_iterator[1].position = {pos._x + size._x, pos._y};
	vertex_iterator[2].position = {pos._x + size._x, pos._y + size._y};
	vertex_iterator[3].position = {pos._x, pos._y + size._y};

	const auto& texture_properties = _atlas_generator.get_texture_properties(texture_id);

	vertex_iterator[0].texture_position = texture_properties.start_normalised;
	vertex_iterator[2].texture_position = texture_properties.end_normalised;

	vertex_iterator[1].texture_position = {
		texture_properties.end_normalised._x,
		texture_properties.start_normalised._y
	};

	vertex_iterator[3].texture_position = {
		texture_properties.start_normalised._x,
		texture_properties.end_normalised._y
	};

	vertex_iterator[0].color = color;
	vertex_iterator[1].color = color;
	vertex_iterator[2].color = color;
	vertex_iterator[3].color = color;

	const auto index_iterator = _render_command.insert_indices(6);

	index_iterator[0] = vertex_index;
	index_iterator[1] = vertex_index + 1;
	index_iterator[2] = vertex_index + 2;

	index_iterator[3] = vertex_index;
	index_iterator[4] = vertex_index + 2;
	index_iterator[5] = vertex_index + 3;
}

void renderer::draw_line(const lib::point2Di& p1, const lib::point2Di& p2, const lib::color& color, float thickness)
{
	// get direction of line and scale to thickness
	auto dir = lib::vector2D(
		static_cast<float>(p2._x - p1._x),
		static_cast<float>(p2._y - p1._y)).normalised();

	dir *= (thickness * 0.5f);

	const auto vertex_index = _render_command.prepare_batch(_clipped_area, shader_type::normal);
	const auto vertex_iterator = _render_command.insert_vertices(4);

	vertex_iterator[0].position = {static_cast<float>(p1._x) + dir._y, static_cast<float>(p1._y) - dir._x};
	vertex_iterator[1].position = {static_cast<float>(p2._x) + dir._y, static_cast<float>(p2._y) - dir._x};
	vertex_iterator[2].position = {static_cast<float>(p2._x) - dir._y, static_cast<float>(p2._y) + dir._x};
	vertex_iterator[3].position = {static_cast<float>(p1._x) - dir._y, static_cast<float>(p1._y) + dir._x};

	const auto& texture_properties = _atlas_generator.get_texture_properties(_opaque_texture_id);

	const auto texture_diff = texture_properties.end_normalised - texture_properties.start_normalised;
	const auto texture_center = texture_properties.start_normalised + (texture_diff * 0.5f);

	vertex_iterator[0].texture_position = texture_center;
	vertex_iterator[1].texture_position = texture_center;
	vertex_iterator[2].texture_position = texture_center;
	vertex_iterator[3].texture_position = texture_center;

	vertex_iterator[0].color = color;
	vertex_iterator[1].color = color;
	vertex_iterator[2].color = color;
	vertex_iterator[3].color = color;

	const auto index_iterator = _render_command.insert_indices(6);

	index_iterator[0] = vertex_index;
	index_iterator[1] = vertex_index + 1;
	index_iterator[2] = vertex_index + 2;

	index_iterator[3] = vertex_index;
	index_iterator[4] = vertex_index + 2;
	index_iterator[5] = vertex_index + 3;
}

void renderer::draw_triangle(const lib::point2Di& p1,
                             const lib::point2Di& p2,
                             const lib::point2Di& p3,
                             const lib::color& color,
                             float thickness)
{
	draw_line(p1, p2, color, thickness);
	draw_line(p2, p3, color, thickness);
	draw_line(p3, p1, color, thickness);
}

void renderer::draw_triangle_filled(const lib::point2Di& p1,
                                    const lib::point2Di& p2,
                                    const lib::point2Di& p3,
                                    const lib::color& color)
{
	const auto vertex_index = _render_command.prepare_batch(_clipped_area, shader_type::normal);
	const auto vertex_iterator = _render_command.insert_vertices(3);

	vertex_iterator[0].position = {static_cast<float>(p1._x), static_cast<float>(p1._y)};
	vertex_iterator[1].position = {static_cast<float>(p2._x), static_cast<float>(p2._y)};
	vertex_iterator[2].position = {static_cast<float>(p3._x), static_cast<float>(p3._y)};

	const auto& texture_properties = _atlas_generator.get_texture_properties(_opaque_texture_id);

	const auto texture_diff = texture_properties.end_normalised - texture_properties.start_normalised;
	const auto texture_center = texture_properties.start_normalised + (texture_diff * 0.5f);

	vertex_iterator[0].texture_position = texture_center;
	vertex_iterator[1].texture_position = texture_center;
	vertex_iterator[2].texture_position = texture_center;

	vertex_iterator[0].color = color;
	vertex_iterator[1].color = color;
	vertex_iterator[2].color = color;

	const auto index_iterator = _render_command.insert_indices(3);

	index_iterator[0] = vertex_index;
	index_iterator[1] = vertex_index + 1;
	index_iterator[2] = vertex_index + 2;
}

void renderer::draw_rectangle(const lib::point2Di& pos,
                              const lib::point2Di& size,
                              const lib::color& color,
                              float thickness)
{
	draw_line({pos._x, pos._y}, {pos._x + size._x, pos._y}, color, thickness);
	draw_line({pos._x + size._x, pos._y}, {pos._x + size._x, pos._y + size._y}, color, thickness);
	draw_line({pos._x + size._x, pos._y + size._y}, {pos._x, pos._y + size._y}, color, thickness);
	draw_line({pos._x, pos._y + size._y}, {pos._x, pos._y}, color, thickness);
}

void renderer::draw_rectangle_filled(const lib::point2Di& pos, const lib::point2Di& size, const lib::color& color)
{
	draw_rect_gradient_filled(pos, size, color, color, color, color);
}

void renderer::draw_rect_gradient_h_filled(const lib::point2Di& pos,
                                           const lib::point2Di& size,
                                           const lib::color& c1, const
                                           lib::color& c2)
{
	draw_rect_gradient_filled(pos, size, c1, c2, c2, c1);
}

void renderer::draw_rect_gradient_v_filled(const lib::point2Di& pos,
                                           const lib::point2Di& size,
                                           const lib::color& c1,
                                           const lib::color& c2)
{
	draw_rect_gradient_filled(pos, size, c1, c1, c2, c2);
}

void renderer::draw_rect_gradient_filled(const lib::point2Di& pos,
                                         const lib::point2Di& size,
                                         const lib::color& c1,
                                         const lib::color& c2,
                                         const lib::color& c3,
                                         const lib::color& c4)
{
	const auto vertex_index = _render_command.prepare_batch(_clipped_area, shader_type::normal);
	const auto vertex_iterator = _render_command.insert_vertices(4);

	vertex_iterator[0].position = {pos._x, pos._y};
	vertex_iterator[1].position = {pos._x + size._x, pos._y};
	vertex_iterator[2].position = {pos._x + size._x, pos._y + size._y};
	vertex_iterator[3].position = {pos._x, pos._y + size._y};

	const auto& texture_properties = _atlas_generator.get_texture_properties(_opaque_texture_id);

	const auto texture_diff = texture_properties.end_normalised - texture_properties.start_normalised;
	const auto texture_center = texture_properties.start_normalised + (texture_diff * 0.5f);

	vertex_iterator[0].texture_position = texture_center;
	vertex_iterator[1].texture_position = texture_center;
	vertex_iterator[2].texture_position = texture_center;
	vertex_iterator[3].texture_position = texture_center;

	vertex_iterator[0].color = c1;
	vertex_iterator[1].color = c2;
	vertex_iterator[2].color = c3;
	vertex_iterator[3].color = c4;

	const auto index_iterator = _render_command.insert_indices(6);

	index_iterator[0] = vertex_index;
	index_iterator[1] = vertex_index + 1;
	index_iterator[2] = vertex_index + 2;

	index_iterator[3] = vertex_index;
	index_iterator[4] = vertex_index + 2;
	index_iterator[5] = vertex_index + 3;
}

void renderer::draw_font(const lib::point2Di& pos,
						 const lib::color& color,
						 font_id font_id,
						 const std::string& text,
						 uint16_t flags)
{
	auto current_pos = pos;
	const auto& font_properties = _font_properties.at(font_id);

	const auto get_text_height = [&]() -> int
	{
		return font_properties.at(' ' - 32).spacing._y;
	};

	const auto get_text_width = [&]() -> int
	{
		int w = 0;

		for (const auto& c : text)
		{
			w += font_properties.at(c - 32).spacing._x;
		}

		return w;
	};

	if (flags & right_aligned)
	{
		current_pos._x -= get_text_width();
	}
	else if (flags & centered_x)
	{
		current_pos._x -= get_text_width() / 2;
	}

	if (flags & centered_y)
	{
		current_pos._y += get_text_height() / 2;
	}

	shader_type shader;

	if (flags & outline)
	{
		shader = shader_type::sdf_outline;
	}
	else
	{
		shader = shader_type::sdf;
	}

	for (const auto& c : text)
	{
		const auto& font_property = font_properties.at(c - 32);

		if (c != ' ')
		{
			// pretty much draw image but inlined :)
			const auto vertex_index = _render_command.prepare_batch(_clipped_area, shader);
			const auto vertex_iterator = _render_command.insert_vertices(4);

			const auto& texture_property = _atlas_generator.get_texture_properties(font_property.id);

			const auto char_pos = current_pos + font_property.offset;
			const auto size = texture_property.size_pixel;

			vertex_iterator[0].position = {char_pos._x, char_pos._y};
			vertex_iterator[1].position = {char_pos._x + size._x, char_pos._y};
			vertex_iterator[2].position = {char_pos._x + size._x, char_pos._y + size._y};
			vertex_iterator[3].position = {char_pos._x, char_pos._y + size._y};

			vertex_iterator[0].texture_position = texture_property.start_normalised;
			vertex_iterator[2].texture_position = texture_property.end_normalised;

			vertex_iterator[1].texture_position = {
				texture_property.end_normalised._x,
				texture_property.start_normalised._y
			};

			vertex_iterator[3].texture_position = {
				texture_property.start_normalised._x,
				texture_property.end_normalised._y
			};

			vertex_iterator[0].color = color;
			vertex_iterator[1].color = color;
			vertex_iterator[2].color = color;
			vertex_iterator[3].color = color;

			const auto index_iterator = _render_command.insert_indices(6);

			index_iterator[0] = vertex_index;
			index_iterator[1] = vertex_index + 1;
			index_iterator[2] = vertex_index + 2;

			index_iterator[3] = vertex_index;
			index_iterator[4] = vertex_index + 2;
			index_iterator[5] = vertex_index + 3;
		}

		current_pos._x += font_property.spacing._x;
	}
}

void renderer::update_clipped_area(const lib::point4Di& clipped_area)
{
	_clipped_area = clipped_area;
}
