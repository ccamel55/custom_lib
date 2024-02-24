#pragma once

#include <lib_rendering/render_api_base.hpp>
#include <lib_rendering/render_state.hpp>
#include <lib_rendering/shaders.hpp>

namespace lib::rendering
{
namespace gl3
{
struct uniform_buffer_object_t
{
	glm::mat4 projection_matrix = {1.f};
	glm::mat4 view_matrix = {1.f};
	glm::mat4 model_matrix = {1.f};
};
}

struct render_api_data_t
{
};

class render_api final : public render_api_base
{
public:
	render_api(const std::weak_ptr<render_api_data_t>& render_api_data, bool flush_buffers);
	~render_api() override;

	void bind_atlas(const uint8_t* data, int width, int height) override;
	void update_screen_size(const lib::point2Di& window_size) override;
	void draw(const render_command& render_command) override;

private:
	gl3::render_state _render_state = {};
	gl3::uniform_buffer_object_t _uniform_buffer_object = {};

	gl3::shader_module _normal_shader = {};
	gl3::shader_module _sdf_shader = {};
	gl3::shader_module _sdf_outline_shader = {};

	GLuint _vertex_array = 0;
	GLuint _vertex_buffer = 0;
	GLuint _index_buffer = 0;
	GLuint _texture_atlas = 0;
	GLuint _uniform_buffer = 0;
};
}  // namespace lib::rendering