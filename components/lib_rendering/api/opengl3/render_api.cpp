#include <lib_rendering/render_api.hpp>

#include <lib_rendering/shaders/basic_shader_vert.hpp>
#include <lib_rendering/shaders/normal_shader_frag.hpp>
#include <lib_rendering/shaders/sdf_shader_frag.hpp>
#include <lib_rendering/shaders/outline_shader_frag.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cassert>

// this file is a fucking mess, but it works and it works efficiently so eat poo
using namespace lib::rendering;

render_api::render_api(const std::weak_ptr<render_api_data_t>& render_api_data, bool flush_buffers)
	: render_api_base(flush_buffers)
{
	(void)render_api_data;

	// load opengl
	if (gladLoadGL() == 0)
	{
		lib_log_e("renderer: could not load opengl");
		assert(false);
	}

	GLint major = 0;
	GLint minor = 0;

	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	lib_log_d("renderer:: opengl version {}.{}", major, minor);

	// setup shaders
	_normal_shader.create(
		gl3::shaders::basic_shader_vert,
		gl3::shaders::normal_shader_frag);

	_sdf_shader.create(
		gl3::shaders::basic_shader_vert,
		gl3::shaders::sdf_shader_frag);

	_sdf_outline_shader.create(
		gl3::shaders::basic_shader_vert,
		gl3::shaders::outline_shader_frag);

	_render_state.capture();

	// generate buffers
	glGenBuffers(1, &_vertex_buffer);
	glGenBuffers(1, &_index_buffer);
	glGenBuffers(1, &_uniform_buffer);

	// bind attributes to our vertex array object
	glGenVertexArrays(1, &_vertex_array);
	glBindVertexArray(_vertex_array);
	{
		glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer);
		glBindBuffer(GL_UNIFORM_BUFFER, _uniform_buffer);

		glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * sizeof(vertex_t), nullptr, GL_DYNAMIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_INDICES * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(gl3::uniform_buffer_object_t), nullptr, GL_DYNAMIC_DRAW);

		// position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(vertex_t),
			reinterpret_cast<void*>(offsetof(vertex_t, position)));

		// color
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(
				1,
				4,
				GL_UNSIGNED_BYTE,
				GL_TRUE,
				sizeof(vertex_t),
				reinterpret_cast<void*>(offsetof(vertex_t, color)));

		// texture position
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(
			2,
			2,
			GL_FLOAT,
			GL_FALSE,
			sizeof(vertex_t),
			reinterpret_cast<void*>(offsetof(vertex_t, texture_position)));
	}

	// bind texture sampler
	{
		_normal_shader.bind();
		_normal_shader.bind_uniform_block(0, "uniform_buffer_object_t");
		glUniform1i(_normal_shader.get_attribute_location("texture_sample"), 0);

		_sdf_shader.bind();
		_sdf_shader.bind_uniform_block(0, "uniform_buffer_object_t");
		glUniform1i(_sdf_shader.get_attribute_location("texture_sample"), 0);

		_sdf_outline_shader.bind();
		_sdf_outline_shader.bind_uniform_block(0, "uniform_buffer_object_t");
		glUniform1i(_sdf_outline_shader.get_attribute_location("texture_sample"), 0);
	}

	_render_state.restore();

	// call once on init with default parameters, this way we guarentee we have something setup
	update_screen_size(default_window_size);
}

render_api::~render_api()
{
	glDeleteTextures(1, &_texture_atlas);

	glDeleteBuffers(1, &_vertex_buffer);
	glDeleteBuffers(1, &_index_buffer);
	glDeleteBuffers(1, &_uniform_buffer);

	glDeleteVertexArrays(1, &_vertex_array);
}

void render_api::bind_atlas(const uint8_t* data, int width, int height)
{
	// only use 1 atlas ever
	// todo: destroy current texture and re init if texture exists
	assert(_texture_atlas == 0);

	// create texture setup how our texture will be sampled
	GLuint last_texture = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, reinterpret_cast<GLint*>(&last_texture));

	glGenTextures(1, &_texture_atlas);
	glBindTexture(GL_TEXTURE_2D, _texture_atlas);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, last_texture);
}

void render_api::update_screen_size(const lib::point2Di& window_size)
{
	// save a copy of window size for us to use later
	_window_size = window_size;

	// update projection and view matrix, model matrix can be instance speicifc
	_uniform_buffer_object.projection_matrix = glm::ortho(
		0.f,
		static_cast<float>(window_size.x),
		static_cast<float>(window_size.y),
		0.f);

	// load identity matrix for now, we can fuck with this later
	_uniform_buffer_object.view_matrix = glm::mat4(1.f);
}

void render_api::draw(const render_command& render_command)
{
	if (_flush_buffers)
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	// backup render state
	_render_state.capture();
	glClear(GL_COLOR_BUFFER_BIT);

	// set up our own render state
	glEnable(GL_BLEND);
	glEnable(GL_SCISSOR_TEST);
	glEnable(GL_CULL_FACE);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);

	// only show front, front faces are represented by clock wise rotation,
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(
		GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA,
		GL_ONE,
		GL_ONE_MINUS_SRC_ALPHA);

	// bind master texture
	glActiveTexture(GL_TEXTURE0);

	// use whole screen as viewport, but we scissor regions in each batch
	glViewport(0, 0, _window_size.x, _window_size.y);

	// bind relevant buffers and start drawing
	glBindVertexArray(_vertex_array);

	glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, _uniform_buffer);

	glBufferSubData(
		GL_ARRAY_BUFFER,
		0,
		static_cast<GLsizeiptr>(render_command.vertex_count * sizeof(vertex_t)),
		render_command.vertices.data());

	glBufferSubData(
		GL_ELEMENT_ARRAY_BUFFER,
		0,
		static_cast<GLsizeiptr>(render_command.index_count * sizeof(uint32_t)),
		render_command.indices.data());

	glBindBufferRange(
		GL_UNIFORM_BUFFER,
		0,
		_uniform_buffer,
		0,
		sizeof(gl3::uniform_buffer_object_t));

	// bind our atlas, every texture should live on the atlas
	glBindTexture(GL_TEXTURE_2D, _texture_atlas);

	for (uint32_t i = 0; i < render_command.batch_count; i++)
	{
		const auto& batch = render_command.batches.at(i);
		_uniform_buffer_object.model_matrix = batch.model_matrix;

		switch (batch.shader)
		{
		case shader_type::normal:
			_normal_shader.bind();
			break;
		case shader_type::sdf:
			_sdf_shader.bind();
			break;
		case shader_type::sdf_outline:
			_sdf_outline_shader.bind();
			break;
		}

		glBufferSubData(
			GL_UNIFORM_BUFFER,
			0,
			sizeof(gl3::uniform_buffer_object_t),
			&_uniform_buffer_object);

		// x and y represent the bottom left corner, we give x and y as the top right corner
		glScissor(
			batch.clipped_area.x,
			// flip the y axis so that start point = bottom left corner
			_window_size.y - batch.clipped_area.w - batch.clipped_area.y,
			batch.clipped_area.z,
			batch.clipped_area.w);

		glDrawElements(
			GL_TRIANGLES,
			static_cast<int>(batch.count),
			GL_UNSIGNED_INT,
			reinterpret_cast<void*>(batch.offset * sizeof(uint32_t)));
	}

	// this will restore the old frame buffer if it was set
	_render_state.restore();
}