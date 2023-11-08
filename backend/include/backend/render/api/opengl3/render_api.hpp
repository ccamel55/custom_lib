#pragma once

#include <backend/render/api/render_api_base.hpp>

#include <backend/render/api/opengl3/render_state.hpp>
#include <backend/render/api/opengl3/shaders.hpp>

namespace lib::backend::render
{
class render_api : public render_api_base
{
public:
	render_api();
	~render_api() override;

	void add_texture(render::texture_id id, const uint8_t* data, int width, int height) override;
	void update_screen_size(const common::point2Di& window_size) override;
	void draw_render_command(const render_command& render_command) override;

private:
	common::point2Di _window_size;

	gl3::render_state _render_state;
	gl3::shaders _shader;

	GLuint _vertex_array;
	GLuint _vertex_buffer;
	GLuint _index_buffer;

	// indexes by texture_id
	std::vector<GLuint> _textures = {};
};
}  // namespace lib::backend::render