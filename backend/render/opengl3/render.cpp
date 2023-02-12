#include <backend/render/opengl3/render.hpp>

using namespace lib::backend;

void renderer::init_instance()
{
}

void renderer::destroy_instance()
{
}

void renderer::bind_context(void* context)
{
}

void renderer::remove_context()
{
}

void renderer::reset()
{
}

void renderer::render_start()
{
}

void renderer::render_finish()
{
}

void renderer::addFont(common::hash_t font_hash, const std::string& font_name, size_t height, size_t weight)
{
}

void renderer::draw_string(common::hash_t font_hash, const common::point2Di& pos, const common::color& color, const std::string& string, render_flags flags)
{
}

void renderer::draw_rect(const common::point4Di& area, const common::color& color, render_flags flags)
{
}

void renderer::draw_rect_gradient(const common::point4Di& area, const common::color& color1, const common::color& color2, render_flags flags)
{
}

void renderer::draw_rect_filled(const common::point4Di& area, const common::color& color, render_flags flags)
{
}

void renderer::draw_rect_filled_gradient(const common::point4Di& area, const common::color& color1, const common::color& color2, render_flags flags)
{
}

void renderer::draw_circle(const common::point2Di& pos, float radius, const common::color& color, render_flags flags)
{
}

void renderer::draw_circle_filled(const common::point2Di& pos, float radius, const common::color& color, render_flags flags)
{
}

void renderer::draw_circle_filled_gradient(const common::point2Di& pos, float radius, const common::color& color1, const common::color& color2, render_flags flags)
{
}

void renderer::draw_triangle(const common::point2Di& pos1, const common::point2Di& pos2, const common::point2Di& pos3, const common::color& color, render_flags flags)
{
}

void renderer::draw_triangle_filled(const common::point2Di& pos1, const common::point2Di& pos2, const common::point2Di& pos3, const common::color& color, render_flags flags)
{
}