#include <backend/render/opengl3/render.hpp>
#include <common/logger.hpp>

#include <glad/glad.h>

using namespace lib::backend;

void renderer::init_instance(void* init_data)
{
	if (_created_instance)
	{
		lib_log_w("renderer: could not create new instance, instance already exists");
		return;
	}

	if (gladLoadGL() == 0)
	{
		lib_log_e("renderer: could not load opengl");
		return;
	}

	_created_instance = true;

	lib_log_d("renderer: initialised new opengl instance");
	lib_log_d("renderer: " << glGetString(GL_VERSION));

	// setup color blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// set background color
	glClearColor(0.15, 0.15, 0.15, 1);
}

void renderer::destroy_instance()
{
	if (!_created_instance)
	{
		lib_log_w("renderer: destroy instance, maybe using context?");
		return;
	}
}

void renderer::bind_context(void* context)
{
	if (_created_instance)
	{
		lib_log_w("renderer: could not bind context, instance exists?");
		return;
	}
}

void renderer::remove_context()
{
	if (_created_instance)
	{
		lib_log_w("renderer: could unbind context");
		return;
	}
}

void renderer::reset()
{
}

void renderer::render_start()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void renderer::render_finish()
{
}

void renderer::add_font(common::fnv1a_t font_hash, const std::string& font_name, size_t height, size_t weight)
{
}

void renderer::draw_string(
	lib::common::fnv1a_t font_hash,
	const lib::common::point2Di& pos,
	const lib::common::color& color,
	const std::string& string,
	render_flags flags)
{
}

void renderer::draw_rect(const lib::common::point4Di& area, const lib::common::color& color, render_flags flags)
{
}

void renderer::draw_rect_gradient(
	const lib::common::point4Di& area,
	const lib::common::color& color1,
	const lib::common::color& color2,
	render_flags flags)
{
}

void renderer::draw_circle(
	const lib::common::point2Di& pos, float radius, const lib::common::color& color, render_flags flags)
{
}

void renderer::draw_circle_gradient(
	const lib::common::point2Di& pos,
	float radius,
	const lib::common::color& color1,
	const lib::common::color& color2,
	render_flags flags)
{
}

void renderer::draw_triangle(
	const lib::common::point2Di& pos1,
	const lib::common::point2Di& pos2,
	const lib::common::point2Di& pos3,
	const lib::common::color& color,
	render_flags flags)
{
}

void renderer::draw_triangle_gradient(
	const lib::common::point2Di& pos1,
	const lib::common::point2Di& pos2,
	const lib::common::point2Di& pos3,
	const lib::common::color& color1,
	const lib::common::color& color2,
	render_flags flags)
{
}
