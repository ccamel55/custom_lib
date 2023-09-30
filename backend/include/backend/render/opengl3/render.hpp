#pragma once
#include <backend/render/opengl3/types/batch.hpp>
#include <backend/render/renderer_base.hpp>

namespace lib::backend
{
class renderer : public renderer_base
{
public:
	void init_instance(void* init_data) override;
	void destroy_instance() override;
	void bind_context(void* bind_data) override;
	void remove_context() override;
	void reset() override;
	void render_start() override;
	void render_finish() override;
	void add_font(common::fnv1a_t font_hash, const std::string& font_name, size_t height, size_t weight) override;

public:
	void draw_string(
		common::fnv1a_t font_hash,
		const common::point2Di& pos,
		const common::color& color,
		const std::string& string,
		render_flags flags) override;

	void draw_rect(const common::point4Di& area, const common::color& color, render_flags flags) override;

	void draw_rect_gradient(
		const common::point4Di& area,
		const common::color& color1,
		const common::color& color2,
		render_flags flags) override;

	void draw_circle(
		const common::point2Di& pos, float radius, const common::color& color, render_flags flags) override;

	void draw_circle_gradient(
		const common::point2Di& pos,
		float radius,
		const common::color& color1,
		const common::color& color2,
		render_flags flags) override;

	void draw_triangle(
		const common::point2Di& pos1,
		const common::point2Di& pos2,
		const common::point2Di& pos3,
		const common::color& color,
		render_flags flags) override;

	void draw_triangle_gradient(
		const common::point2Di& pos1,
		const common::point2Di& pos2,
		const common::point2Di& pos3,
		const common::color& color1,
		const common::color& color2,
		render_flags flags) override;
};
}  // namespace lib::backend
