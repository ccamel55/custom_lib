#include <backend/render/renderer.hpp>

#include <common/logger.hpp>
#include <common/types/vector/vector2D.hpp>

#include <cassert>

#if DEF_LIB_BACKEND_RENDER == gl3
#include <backend/render/api/opengl3/render_api.hpp>
#endif

using namespace lib::backend;

namespace
{
constexpr uint8_t opaque_texture_width = 1;
constexpr uint8_t opaque_texture_height = 1;

constexpr uint8_t opaque_texture_data[] = {
	// r g b a
	0xff,
	0xff,
	0xff,
	0xff};
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

#if DEF_LIB_BACKEND_RENDER == gl3
	// opengl should already be loaded, we just need to map it to our functions
	if (gladLoadGL() == 0)
	{
		lib_log_e("renderer: could not load opengl");
		assert(false);
	}

	GLint major = 0;
	GLint minor = 0;

	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	lib_log_i("renderer:: opengl version {}.{}", major, minor);

	_render_api = std::make_unique<backend::render::render_api>();
#endif

	// create opaque texture for us to draw into
	_texture_properties.emplace_back();
	_opaque_texture_id = 0;

	_render_api->add_texture(_opaque_texture_id, opaque_texture_data, opaque_texture_width, opaque_texture_height);
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

	_texture_properties.clear();
}

render::texture_id renderer::add_image(const std::filesystem::path& image)
{
	// create opaque texture for us to draw into
	auto& properties = _texture_properties.emplace_back();
	const auto id = static_cast<render::texture_id>(_texture_properties.size() - 1);

	render::image_loader loader(image);
	const auto data = loader.generate_byte_array();

	properties.size_pixel = {loader.get_width(), loader.get_height()};
	properties.end_normalised = {1.f, 1.f};

	_render_api->add_texture(id, data, loader.get_width(), loader.get_height());

	loader.free_byte_array();
	return id;
}

void renderer::draw_frame()
{
	// get our render api to draw our vertices
	_render_api->draw_render_command(_render_command);

	// flush command, reset states back to default
	_render_command.reset();
	_clipped_area = {0, 0, _window_size._x, _window_size._y};
}

void renderer::set_frame_time(float frame_time)
{
	_frame_time = frame_time;
}

float renderer::get_frame_time() const
{
	return _frame_time;
}

void renderer::set_window_size(const lib::common::point2Di& window_size)
{
	_window_size = window_size;
	_clipped_area = {0, 0, window_size._x, window_size._y};

	_render_api->update_screen_size(window_size);
}

const lib::common::point2Di& renderer::get_window_size() const
{
	return _window_size;
}

void renderer::draw_image(
	const lib::common::point2Di& pos,
	const lib::common::point2Di& size,
	const lib::common::color& color,
	render::texture_id texture_id)
{
	const auto vertex_index = _render_command.prepare_batch(_clipped_area, texture_id);
	const auto vertex_iterator = _render_command.insert_vertices(4);

	vertex_iterator[0].position = {pos._x, pos._y};
	vertex_iterator[1].position = {pos._x + size._x, pos._y};
	vertex_iterator[2].position = {pos._x + size._x, pos._y + size._y};
	vertex_iterator[3].position = {pos._x, pos._y + size._y};

	const auto& texture_properties = _texture_properties.at(texture_id);

	vertex_iterator[0].texture_position = texture_properties.start_normalised;
	vertex_iterator[2].texture_position = texture_properties.end_normalised;

	vertex_iterator[1].texture_position = {
		texture_properties.end_normalised._x, texture_properties.start_normalised._y};
	vertex_iterator[3].texture_position = {
		texture_properties.start_normalised._x, texture_properties.end_normalised._y};

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

void renderer::draw_line(
	const lib::common::point2Di& p1, const lib::common::point2Di& p2, const lib::common::color& color, float thickness)
{
	// get direction of line and scale to thickness
	auto dir = common::vector2D(p2._x - p1._x, p2._y - p1._y).normalised();
	dir *= (thickness * 0.5f);

	const auto vertex_index = _render_command.prepare_batch(_clipped_area, _opaque_texture_id);
	const auto vertex_iterator = _render_command.insert_vertices(4);

	vertex_iterator[0].position = {static_cast<float>(p1._x) + dir._y, static_cast<float>(p1._y) - dir._x};
	vertex_iterator[1].position = {static_cast<float>(p2._x) + dir._y, static_cast<float>(p2._y) - dir._x};
	vertex_iterator[2].position = {static_cast<float>(p2._x) - dir._y, static_cast<float>(p2._y) + dir._x};
	vertex_iterator[3].position = {static_cast<float>(p1._x) - dir._y, static_cast<float>(p1._y) + dir._x};

	const auto& texture_properties = _texture_properties.at(_opaque_texture_id);

	vertex_iterator[0].texture_position = texture_properties.start_normalised;
	vertex_iterator[1].texture_position = texture_properties.start_normalised;
	vertex_iterator[2].texture_position = texture_properties.start_normalised;
	vertex_iterator[3].texture_position = texture_properties.start_normalised;

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

void renderer::draw_triangle(
	const lib::common::point2Di& p1,
	const lib::common::point2Di& p2,
	const lib::common::point2Di& p3,
	const lib::common::color& color,
	float thickness)
{
	draw_line(p1, p2, color, thickness);
	draw_line(p2, p3, color, thickness);
	draw_line(p3, p1, color, thickness);
}

void renderer::draw_triangle_filled(
	const lib::common::point2Di& p1,
	const lib::common::point2Di& p2,
	const lib::common::point2Di& p3,
	const lib::common::color& color)
{
	const auto vertex_index = _render_command.prepare_batch(_clipped_area, _opaque_texture_id);
	const auto vertex_iterator = _render_command.insert_vertices(3);

	vertex_iterator[0].position = {static_cast<float>(p1._x), static_cast<float>(p1._y)};
	vertex_iterator[1].position = {static_cast<float>(p2._x), static_cast<float>(p2._y)};
	vertex_iterator[2].position = {static_cast<float>(p3._x), static_cast<float>(p3._y)};

	const auto& texture_properties = _texture_properties.at(_opaque_texture_id);

	vertex_iterator[0].texture_position = texture_properties.start_normalised;
	vertex_iterator[1].texture_position = texture_properties.start_normalised;
	vertex_iterator[2].texture_position = texture_properties.start_normalised;

	vertex_iterator[0].color = color;
	vertex_iterator[1].color = color;
	vertex_iterator[2].color = color;

	const auto index_iterator = _render_command.insert_indices(3);

	index_iterator[0] = vertex_index;
	index_iterator[1] = vertex_index + 1;
	index_iterator[2] = vertex_index + 2;
}

void renderer::draw_rectangle(
	const lib::common::point2Di& pos,
	const lib::common::point2Di& size,
	const lib::common::color& color,
	float thickness)
{
	draw_line({pos._x, pos._y}, {pos._x + size._x, pos._y}, color, thickness);
	draw_line({pos._x + size._x, pos._y}, {pos._x + size._x, pos._y + size._y}, color, thickness);
	draw_line({pos._x + size._x, pos._y + size._y}, {pos._x, pos._y + size._y}, color, thickness);
	draw_line({pos._x, pos._y + size._y}, {pos._x, pos._y}, color, thickness);
}

void renderer::draw_rectangle_filled(
	const lib::common::point2Di& pos, const lib::common::point2Di& size, const lib::common::color& color)
{
	draw_rect_gradient_filled(pos, size, color, color, color, color);
}

void renderer::draw_rect_gradient_h_filled(
	const lib::common::point2Di& pos,
	const lib::common::point2Di& size,
	const lib::common::color& c1,
	const lib::common::color& c2)
{
	draw_rect_gradient_filled(pos, size, c1, c2, c2, c1);
}

void renderer::draw_rect_gradient_v_filled(
	const lib::common::point2Di& pos,
	const lib::common::point2Di& size,
	const lib::common::color& c1,
	const lib::common::color& c2)
{
	draw_rect_gradient_filled(pos, size, c1, c1, c2, c2);
}

void renderer::draw_rect_gradient_filled(
	const lib::common::point2Di& pos,
	const lib::common::point2Di& size,
	const lib::common::color& c1,
	const lib::common::color& c2,
	const lib::common::color& c3,
	const lib::common::color& c4)
{
	const auto vertex_index = _render_command.prepare_batch(_clipped_area, _opaque_texture_id);
	const auto vertex_iterator = _render_command.insert_vertices(4);

	vertex_iterator[0].position = {pos._x, pos._y};
	vertex_iterator[1].position = {pos._x + size._x, pos._y};
	vertex_iterator[2].position = {pos._x + size._x, pos._y + size._y};
	vertex_iterator[3].position = {pos._x, pos._y + size._y};

	const auto& texture_properties = _texture_properties.at(_opaque_texture_id);

	vertex_iterator[0].texture_position = texture_properties.start_normalised;
	vertex_iterator[1].texture_position = texture_properties.start_normalised;
	vertex_iterator[2].texture_position = texture_properties.start_normalised;
	vertex_iterator[3].texture_position = texture_properties.start_normalised;

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

void renderer::update_clipped_area(const lib::common::point4Di& clipped_area)
{
	_clipped_area = clipped_area;
}
