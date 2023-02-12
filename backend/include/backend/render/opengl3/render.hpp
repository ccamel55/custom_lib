#pragma once

#include <backend/render/renderer_base.hpp>

namespace lib::backend
{
	class renderer : public renderer_base
	{
	public:
		void init_instance();
		void destroy_instance();
		void bind_context(void* context);
		void remove_context();
		void reset();
		void render_start();
		void render_finish();
		void addFont(common::hash_t font_hash, const std::string& font_name, size_t height, size_t weight);
		void draw_string(common::hash_t font_hash, const common::point2Di& pos, const common::color& color, const std::string& string, render_flags flags);
		void draw_rect(const common::point4Di& area, const common::color& color, render_flags flags);
		void draw_rect_gradient(const common::point4Di& area, const common::color& color1, const common::color& color2, render_flags flags);
		void draw_rect_filled(const common::point4Di& area, const common::color& color, render_flags flags);
		void draw_rect_filled_gradient(const common::point4Di& area, const common::color& color1, const common::color& color2, render_flags flags);
		void draw_circle(const common::point2Di& pos, float radius, const common::color& color, render_flags flags);
		void draw_circle_filled(const common::point2Di& pos, float radius, const common::color& color, render_flags flags);
		void draw_circle_filled_gradient(const common::point2Di& pos, float radius, const common::color& color1, const common::color& color2, render_flags flags);
		void draw_triangle(const common::point2Di& pos1, const common::point2Di& pos2, const common::point2Di& pos3, const common::color& color, render_flags flags);
		void draw_triangle_filled(const common::point2Di& pos1, const common::point2Di& pos2, const common::point2Di& pos3, const common::color& color, render_flags flags);

	private:
	};
}
