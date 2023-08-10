#pragma once

#include <backend/render/renderer_base.hpp>
#include <backend/render/common/render_batch.hpp>

#include <backend/render/opengl3/core/shader_manager.hpp>
#include <backend/render/opengl3/core/vertex_layout_manager.hpp>
#include <backend/render/opengl3/core/vertex_manager.hpp>

namespace lib::backend
{
	class renderer : public renderer_base
	{
	public:
		void init_instance() override;
		void destroy_instance() override;
		void bind_context(void* context) override;
		void remove_context() override;
		void reset() override;
		void render_start() override;
		void render_finish() override;
		void add_font(common::hash_t font_hash, const std::string& font_name, size_t height, size_t weight) override;
		void draw_string(common::hash_t font_hash, const common::point2Di& pos, const common::color& color, const std::string& string, render_flags flags) override;
		void draw_rect(const common::point4Di& area, const common::color& color, render_flags flags) override;
		void draw_rect_gradient(const common::point4Di& area, const common::color& color1, const common::color& color2, render_flags flags) override;
		void draw_rect_filled(const common::point4Di& area, const common::color& color, render_flags flags) override;
		void draw_rect_filled_gradient(const common::point4Di& area, const common::color& color1, const common::color& color2, render_flags flags) override;
		void draw_circle(const common::point2Di& pos, float radius, const common::color& color, render_flags flags) override;
		void draw_circle_filled(const common::point2Di& pos, float radius, const common::color& color, render_flags flags) override;
		void draw_circle_filled_gradient(const common::point2Di& pos, float radius, const common::color& color1, const common::color& color2, render_flags flags) override;
		void draw_triangle(const common::point2Di& pos1, const common::point2Di& pos2, const common::point2Di& pos3, const common::color& color, render_flags flags) override;
		void draw_triangle_filled(const common::point2Di& pos1, const common::point2Di& pos2, const common::point2Di& pos3, const common::color& color, render_flags flags) override;

	private:
		void init_internal();
		void add_to_render_list(const std::vector<render::vertex_2d_t>& vertices, int primitive_type, int texture_id = -1);

	private:
		std::vector<render::render_batch_t> _render_batches = {};

		gl3::shader_manager _color_shader = {};
		gl3::shader_manager _texture_shader = {};
		gl3::vertex_manager _vertex_manager = {};
	};
}
