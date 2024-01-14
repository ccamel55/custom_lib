#pragma once

#include <lib_rendering/render_api_base.hpp>
#include <lib_rendering/render_state.hpp>
#include <lib_rendering/shaders.hpp>

namespace lib::rendering
{
class render_api : public render_api_base
{
public:
	render_api();
	~render_api() override;

	void bind_atlas(const uint8_t* data, int width, int height) override;
	void update_screen_size(const lib::point2Di& window_size) override;
	void update_frame_buffer(const render_command& render_command) override;
	void draw_frame_buffer() override;

private:
	void create_frame_buffer_texture(const lib::point2Di& window_size);

	lib::point2Di _window_size;
	gl3::render_state _render_state;

	gl3::shaders _normal_shader;
	gl3::shaders _sdf_shader;
	gl3::shaders _sdf_outline_shader;

	GLuint _vertex_array;
	GLuint _vertex_buffer;
	GLuint _index_buffer;

	GLuint _texture_atlas;

	GLuint _frame_buffer;
	GLuint _frame_buffer_vertex_array;
	GLuint _frame_buffer_vertex_buffer;
	GLuint _frame_buffer_texture;
	gl3::shaders _frame_buffer_shader;
};
}  // namespace lib::rendering