#pragma once
#include <glad/glad.h>

namespace lib::backend::opengl3
{
class state_manager
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
	GLuint _last_vertex_array_object = 0;
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
}  // namespace lib::backend::opengl3