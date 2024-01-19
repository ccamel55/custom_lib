#pragma once

#include <lib_rendering/render_api_base.hpp>
#include <lib_rendering/render_state.hpp>
#include <lib_rendering/shaders.hpp>

namespace lib::rendering
{
class render_api final : public render_api_base
{
public:
	render_api(const void* api_context, bool flush_buffers);
	~render_api() override;

	void bind_atlas(const uint8_t* data, int width, int height) override;
	void update_screen_size(const lib::point2Di& window_size) override;
	void update_frame_buffer(const render_command& render_command) override;
	void draw_frame_buffer() override;

private:
	void create_frame_buffer_texture(const lib::point2Di& window_size);

	lib::point2Di _window_size = {};
	gl3::render_state _render_state = {};

	gl3::shaders _normal_shader = {};
	gl3::shaders _sdf_shader = {};
	gl3::shaders _sdf_outline_shader = {};

	GLuint _vertex_array = 0;
	GLuint _vertex_buffer = 0;
	GLuint _index_buffer = 0;

	GLuint _texture_atlas = 0;

	GLuint _frame_buffer = 0;
	GLuint _frame_buffer_vertex_array = 0;
	GLuint _frame_buffer_vertex_buffer = 0;
	GLuint _frame_buffer_texture = 0;
	gl3::shaders _frame_buffer_shader = {};
};
}  // namespace lib::rendering