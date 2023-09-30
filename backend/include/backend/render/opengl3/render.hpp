#pragma once
#include <backend/render/opengl3/types/batch.hpp>
#include <backend/render/opengl3/types/shader.hpp>
#include <backend/render/opengl3/types/texture.hpp>
#include <backend/render/opengl3/utils/state_manager.hpp>
#include <backend/render/opengl3/utils/vertex_array_object.hpp>
#include <backend/render/opengl3/utils/vertex_buffer_object.hpp>
#include <backend/render/opengl3/utils/vertex_layout.hpp>
#include <backend/render/renderer_base.hpp>
#include <queue>
#include <unordered_map>

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
	void set_window_size(const common::point2Di& window_size) override;
	void render_start() override;
	void render_finish() override;
	void set_scissor_rect(const common::point4Di& rect) override;
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

private:
	void init_opengl();
	void add_vertex(const opengl3::vertex_t* vertices, GLsizei num_vertices, GLenum primitive, GLuint texture_id = 0);

private:
	std::unique_ptr<opengl3::vertex_array_object> _vertex_array_object = nullptr;
	std::unique_ptr<opengl3::texture_t> _color_texture = nullptr;
	std::unique_ptr<opengl3::shader_t> _shader = nullptr;

	std::queue<opengl3::batch_t> _internal_batches = {};
	opengl3::state_manager _render_state = {};

	opengl3::vertex_batch_t* _current_batch = nullptr;
};
}  // namespace lib::backend
