#include <lib_rendering/lib_rendering.hpp>
#include <lib_rendering/render_api.hpp>

#include <lib_rendering/common/font_loader.hpp>
#include <lib_rendering/common/image_loader.hpp>

#include <core_sdk/logger/logger.hpp>
#include <core_sdk/types/vector/vector2D.hpp>

#include <glm/ext/quaternion_geometric.hpp>

using namespace lib::rendering;

renderer::renderer(const std::weak_ptr<render_api_data_t>& render_api_data, bool flush_buffers)
{
	_render_api = std::make_unique<render_api>(render_api_data, flush_buffers);
	_opaque_texture_id = _atlas_generator.add_texture(
		opaque_texture_data,
		opaque_texture_width,
		opaque_texture_height
		);
}

renderer::~renderer()
{
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

		if (font_internal_property.size.x == 0 || font_internal_property.size.y == 0)
		{
			// dont really know what to do here, rather than crash map a white square
			font_property.id = _opaque_texture_id;
			continue;
		}

		const auto texture_id = _atlas_generator.add_texture(
			font_internal_property.data,
			font_internal_property.size.x,
			font_internal_property.size.y
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
		_atlas_generator.get_byte_buffer().data(),
		_atlas_generator.get_width(),
		_atlas_generator.get_height());
}

void renderer::draw_frame()
{
	const auto frame_start_time = std::chrono::system_clock::now();

	_frame_time = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(
		frame_start_time - _last_frame_time).count()) / 1000.f;

	_last_frame_time = frame_start_time;

	// popluate the draw command
	for (const auto& callback : _render_callbacks)
	{
		callback(*this);
	}

	// get our render api to draw our vertices
	_render_api->draw(_render_command);

	// flush command, reset states back to default
	_clipped_area = {0, 0, _window_size.x, _window_size.y};
	_render_command.reset();
}

float renderer::get_frame_time_ms() const
{
	return _frame_time;
}

void renderer::set_window_size(const lib::point2Di& window_size)
{
	_window_size = window_size;
	_clipped_area = {0, 0, window_size.x, window_size.y};

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

	vertex_iterator[0].position = {pos.x, pos.y};
	vertex_iterator[1].position = {pos.x + size.x, pos.y};
	vertex_iterator[2].position = {pos.x + size.x, pos.y + size.y};
	vertex_iterator[3].position = {pos.x, pos.y + size.y};

	const auto& texture_properties = _atlas_generator.get_texture_properties(texture_id);

	vertex_iterator[0].texture_position = texture_properties.start_normalised;
	vertex_iterator[2].texture_position = texture_properties.end_normalised;

	vertex_iterator[1].texture_position = {
		texture_properties.end_normalised.x,
		texture_properties.start_normalised.y
	};

	vertex_iterator[3].texture_position = {
		texture_properties.start_normalised.x,
		texture_properties.end_normalised.y
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
	auto dir = glm::normalize(lib::vector2D(
		static_cast<float>(p2.x - p1.x),
		static_cast<float>(p2.y - p1.y)));

	dir *= (thickness * 0.5f);

	const auto vertex_index = _render_command.prepare_batch(_clipped_area, shader_type::normal);
	const auto vertex_iterator = _render_command.insert_vertices(4);

	vertex_iterator[0].position = {static_cast<float>(p1.x) + dir.y, static_cast<float>(p1.y) - dir.x};
	vertex_iterator[1].position = {static_cast<float>(p2.x) + dir.y, static_cast<float>(p2.y) - dir.x};
	vertex_iterator[2].position = {static_cast<float>(p2.x) - dir.y, static_cast<float>(p2.y) + dir.x};
	vertex_iterator[3].position = {static_cast<float>(p1.x) - dir.y, static_cast<float>(p1.y) + dir.x};

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

	vertex_iterator[0].position = {static_cast<float>(p1.x), static_cast<float>(p1.y)};
	vertex_iterator[1].position = {static_cast<float>(p2.x), static_cast<float>(p2.y)};
	vertex_iterator[2].position = {static_cast<float>(p3.x), static_cast<float>(p3.y)};

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

void renderer::draw_rect(const lib::point2Di& pos,
                              const lib::point2Di& size,
                              const lib::color& color,
                              float thickness)
{
	draw_line({pos.x, pos.y}, {pos.x + size.x, pos.y}, color, thickness);
	draw_line({pos.x + size.x, pos.y}, {pos.x + size.x, pos.y + size.y}, color, thickness);
	draw_line({pos.x + size.x, pos.y + size.y}, {pos.x, pos.y + size.y}, color, thickness);
	draw_line({pos.x, pos.y + size.y}, {pos.x, pos.y}, color, thickness);
}

void renderer::draw_rect_filled(const lib::point2Di& pos, const lib::point2Di& size, const lib::color& color)
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

	vertex_iterator[0].position = {pos.x, pos.y};
	vertex_iterator[1].position = {pos.x + size.x, pos.y};
	vertex_iterator[2].position = {pos.x + size.x, pos.y + size.y};
	vertex_iterator[3].position = {pos.x, pos.y + size.y};

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
						 bitflag flags)
{
	auto current_pos = pos;
	const auto& font_properties = _font_properties.at(font_id);

	const auto get_text_height = [&]() -> int
	{
		return font_properties.at(' ' - 32).spacing.y;
	};

	const auto get_text_width = [&]() -> int
	{
		int w = 0;

		for (const auto& c : text)
		{
			w += font_properties.at(c - 32).spacing.x;
		}

		return w;
	};

	if (flags.has(right_aligned))
	{
		current_pos.x -= get_text_width();
	}
	else if (flags.has(centered_x))
	{
		current_pos.x -= get_text_width() / 2;
	}

	if (flags.has(centered_y))
	{
		current_pos.y += get_text_height() / 2;
	}

	shader_type shader;

	if (flags.has(outline))
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

			vertex_iterator[0].position = {char_pos.x, char_pos.y};
			vertex_iterator[1].position = {char_pos.x + size.x, char_pos.y};
			vertex_iterator[2].position = {char_pos.x + size.x, char_pos.y + size.y};
			vertex_iterator[3].position = {char_pos.x, char_pos.y + size.y};

			vertex_iterator[0].texture_position = texture_property.start_normalised;
			vertex_iterator[2].texture_position = texture_property.end_normalised;

			vertex_iterator[1].texture_position = {
				texture_property.end_normalised.x,
				texture_property.start_normalised.y
			};

			vertex_iterator[3].texture_position = {
				texture_property.start_normalised.x,
				texture_property.end_normalised.y
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

		current_pos.x += font_property.spacing.x;
	}
}

void renderer::update_clipped_area(const lib::point4Di& clipped_area)
{
	_clipped_area = clipped_area;
}

void renderer::register_callback(std::function<void(render_callback_handler&)>&& callback)
{
	_render_callbacks.emplace_back(std::move(callback));
}
