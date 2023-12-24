#pragma once

#include <lib_rendering/render_api_base.hpp>
#include <lib_rendering/render_state.hpp>
#include <lib_rendering/shaders.hpp>

#include <vector>

namespace lib::rendering
{
class render_api : public render_api_base
{
public:
	render_api();
	~render_api() override;

	void add_texture(int id, const uint8_t* data, int width, int height) override;
	void update_screen_size(const lib::point2Di& window_size) override;
	void draw_render_command(const render_command& render_command, int texture_id) override;

private:
	lib::point2Di _window_size;

	gl3::render_state _render_state;
	gl3::shaders _shader;

	GLuint _vertex_array;
	GLuint _vertex_buffer;
	GLuint _index_buffer;

	// indexes by texture_id
	std::vector<GLuint> _textures = {};
};
}  // namespace lib::rendering