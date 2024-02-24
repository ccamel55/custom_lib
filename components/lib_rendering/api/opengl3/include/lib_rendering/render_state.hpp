#pragma once

#include <glad/glad.hpp>

namespace lib::rendering::gl3
{
class render_state
{
public:
	void capture();
	void restore() const;

private:
	// inshallah imgui for providing me with states to keep and restore.
	GLenum _last_active_texture = 0;
	GLuint _last_program = 0;
	GLuint _last_texture = 0;
	GLuint _last_array_buffer = 0;
	GLuint _last_index_buffer = 0;
	GLuint _last_uniform_buffer = 0;
	GLuint _last_vertex_array_object = 0;
	GLuint _last_frame_buffer = 0;
	GLuint _last_cull_face = 0;
	GLuint _last_front_face = 0;
	GLint _last_viewport[4] = {};
	GLint _last_scissor_box[4] = {};
	GLenum _last_blend_src_rgb = 0;
	GLenum _last_blend_dst_rgb = 0;
	GLenum _last_blend_src_alpha = 0;
	GLenum _last_blend_dst_alpha = 0;
	GLenum _last_blend_equation_rgb = 0;
	GLenum _last_blend_equation_alpha = 0;
	GLboolean _last_enable_blend = 0;
	GLboolean _last_enable_cull_face = 0;
	GLboolean _last_enable_depth_test = 0;
	GLboolean _last_enable_stencil_test = 0;
	GLboolean _last_enable_scissor_test = 0;
};
}  // namespace lib::rendering::gl3