#include <lib_rendering/render_state.hpp>

using namespace lib::rendering::gl3;

void render_state::capture()
{
	glGetIntegerv(GL_ACTIVE_TEXTURE, reinterpret_cast<GLint*>(&_last_active_texture));
	glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>(&_last_program));
	glGetIntegerv(GL_TEXTURE_BINDING_2D, reinterpret_cast<GLint*>(&_last_texture));
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, reinterpret_cast<GLint*>(&_last_array_buffer));
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, reinterpret_cast<GLint*>(&_last_index_buffer));
	glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, reinterpret_cast<GLint*>(&_last_uniform_buffer));
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, reinterpret_cast<GLint*>(&_last_vertex_array_object));
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, reinterpret_cast<GLint*>(&_last_frame_buffer));

	glGetIntegerv(GL_VIEWPORT, _last_viewport);
	glGetIntegerv(GL_SCISSOR_BOX, _last_scissor_box);

	glGetIntegerv(GL_BLEND_SRC_RGB, reinterpret_cast<GLint*>(&_last_blend_src_rgb));
	glGetIntegerv(GL_BLEND_DST_RGB, reinterpret_cast<GLint*>(&_last_blend_dst_rgb));
	glGetIntegerv(GL_BLEND_SRC_ALPHA, reinterpret_cast<GLint*>(&_last_blend_src_alpha));
	glGetIntegerv(GL_BLEND_DST_ALPHA, reinterpret_cast<GLint*>(&_last_blend_dst_alpha));
	glGetIntegerv(GL_BLEND_EQUATION_RGB, reinterpret_cast<GLint*>(&_last_blend_equation_rgb));
	glGetIntegerv(GL_BLEND_EQUATION_ALPHA, reinterpret_cast<GLint*>(&_last_blend_equation_alpha));

	_last_enable_blend = glIsEnabled(GL_BLEND);
	_last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
	_last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
	_last_enable_stencil_test = glIsEnabled(GL_STENCIL_TEST);
	_last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
}

void render_state::restore() const
{
	glUseProgram(_last_program);

	glBindTexture(GL_TEXTURE_2D, _last_texture);
	glActiveTexture(_last_active_texture);

	glBindVertexArray(_last_vertex_array_object);
	glBindBuffer(GL_ARRAY_BUFFER, _last_array_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _last_index_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, _last_uniform_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, _last_frame_buffer);

	glBlendEquationSeparate(_last_blend_equation_rgb, _last_blend_equation_alpha);
	glBlendFuncSeparate(_last_blend_src_rgb, _last_blend_dst_rgb, _last_blend_src_alpha, _last_blend_dst_alpha);

	if (_last_enable_blend)
	{
		glEnable(GL_BLEND);
	}
	else
	{
		glDisable(GL_BLEND);
	}

	if (_last_enable_cull_face)
	{
		glEnable(GL_CULL_FACE);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}

	if (_last_enable_depth_test)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	if (_last_enable_stencil_test)
	{
		glEnable(GL_STENCIL_TEST);
	}
	else
	{
		glDisable(GL_STENCIL_TEST);
	}

	if (_last_enable_scissor_test)
	{
		glEnable(GL_SCISSOR_TEST);
	}
	else
	{
		glDisable(GL_SCISSOR_TEST);
	}

	glViewport(
		_last_viewport[0],
		_last_viewport[1],
		static_cast<GLsizei>(_last_viewport[2]),
		static_cast<GLsizei>(_last_viewport[3]));

	glScissor(
		_last_scissor_box[0],
		_last_scissor_box[1],
		static_cast<GLsizei>(_last_scissor_box[2]),
		static_cast<GLsizei>(_last_scissor_box[3]));
}