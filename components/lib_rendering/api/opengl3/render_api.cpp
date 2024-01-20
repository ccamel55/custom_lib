#include <lib_rendering/render_api.hpp>
#include <cassert>

// this file is a fucking mess, but it works and it works efficiently so eat poo
using namespace lib::rendering;

namespace
{
constexpr char frame_buffer_vertex_shader[] = R"(
        #version 410 core

        layout(location = 0) in vec2 position;
        layout(location = 2) in vec2 uv;

		out vec2 fragment_uv;

        void main()
		{
			fragment_uv = uv;
			gl_Position = vec4(position.xy, 0, 1);
        }
    )";

constexpr char frame_buffer_fragment_shader[] = R"(
		#version 410 core

		uniform sampler2D texture_sample;
		in vec2 fragment_uv;

		layout (location = 0) out vec4 out_color;

        void main()
		{
	        out_color = texture(texture_sample, fragment_uv.st);
        }
    )";

constexpr char vertex_shader[] = R"(
        #version 410 core

		uniform mat4 projection_matrix;

        layout(location = 0) in vec2 position;
        layout(location = 1) in vec4 color;
        layout(location = 2) in vec2 uv;

        out vec4 fragment_color;
		out vec2 fragment_uv;

        void main()
		{
            fragment_color = color;
			fragment_uv = uv;

			gl_Position = projection_matrix * vec4(position.xy, 0, 1);
        }
    )";

constexpr char normal_fragment_shader[] = R"(
        #version 410 core

		uniform sampler2D texture_sample;

        in vec4 fragment_color;
		in vec2 fragment_uv;

		layout (location = 0) out vec4 out_color;

        void main()
		{
			vec4 sampled_texture = texture(texture_sample, fragment_uv.st);
	        out_color = sampled_texture * fragment_color;
        }
    )";

constexpr char sdf_fragment_shader[] = R"(
        #version 410 core

		// best sharpness = 0.25 / (spread * scale)
		// = 0.25 / (4 * 1)
		const float smoothing = 1.0 / 16.0;

		uniform sampler2D texture_sample;

        in vec4 fragment_color;
		in vec2 fragment_uv;

		layout (location = 0) out vec4 out_color;

        void main()
		{
			vec4 sampled_texture = texture(texture_sample, fragment_uv.st);
			float outline_factor = smoothstep(0.5 - smoothing, 0.5 + smoothing, sampled_texture.a);

			vec4 sdf_texture = vec4(sampled_texture.rgb, outline_factor);
	        out_color = sdf_texture * fragment_color;
        }
    )";

constexpr char sdf_outline_fragment_shader[] = R"(
        #version 410 core

		// best sharpness = 0.25 / (spread * scale)
		// = 0.25 / (4 * 1)
		const float smoothing = 1.0 / 16.0;

		// Between 0 and 0.5, 0 = thick outline, 0.5 = no outline
		const float outline_distance = 0.4;

		// outline will always be black for now, can change later
		const vec4 outline_color = vec4(0.0, 0.0, 0.0, 1.0);

		uniform sampler2D texture_sample;

        in vec4 fragment_color;
		in vec2 fragment_uv;

		layout (location = 0) out vec4 out_color;

        void main()
		{
			vec4 sampled_texture = texture(texture_sample, fragment_uv.st);

			float distance = sampled_texture.a;
			float outline_factor = smoothstep(0.5 - smoothing, 0.5 + smoothing, distance);

			vec4 color = mix(outline_color, sampled_texture, outline_factor);
			float alpha = smoothstep(outline_distance - smoothing, outline_distance + smoothing, distance);

			vec4 sdf_texture = vec4(color.rgb, alpha);
	        out_color = sdf_texture * fragment_color;
        }
    )";
}  // namespace

render_api::render_api(const void* api_context, bool flush_buffers)
	: render_api_base(api_context, flush_buffers)
{
	(void)api_context;

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
	_normal_shader.create(vertex_shader, normal_fragment_shader);
	_sdf_shader.create(vertex_shader, sdf_fragment_shader);
	_sdf_outline_shader.create(vertex_shader, sdf_outline_fragment_shader);
	_frame_buffer_shader.create(frame_buffer_vertex_shader, frame_buffer_fragment_shader);

	_render_state.capture();

	// generate buffers
	glGenBuffers(1, &_vertex_buffer);
	glGenBuffers(1, &_index_buffer);

	// bind attributes to our vertex array object
	glGenVertexArrays(1, &_vertex_array);
	glBindVertexArray(_vertex_array);
	{
		glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer);

		glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * sizeof(vertex_t), nullptr, GL_DYNAMIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_INDICES * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);

		// position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), reinterpret_cast<void*>(offsetof(vertex_t, position)));

		// color
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(
				1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(vertex_t), reinterpret_cast<void*>(offsetof(vertex_t, color)));

		// texture position
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(
			2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), reinterpret_cast<void*>(offsetof(vertex_t, texture_position)));
	}

	// bind texture sampler
	{
		_normal_shader.bind();
		glUniform1i(_normal_shader.get_attribute_location("texture_sample"), 0);

		_sdf_shader.bind();
		glUniform1i(_sdf_shader.get_attribute_location("texture_sample"), 0);

		_sdf_outline_shader.bind();
		glUniform1i(_sdf_outline_shader.get_attribute_location("texture_sample"), 0);
	}

	// create frame buffer and it's respective draw codes
	{
		glGenFramebuffers(1, &_frame_buffer);
		glGenBuffers(1, &_frame_buffer_vertex_buffer);

		glGenVertexArrays(1, &_frame_buffer_vertex_array);
		glBindVertexArray(_frame_buffer_vertex_array);

		glBindBuffer(GL_ARRAY_BUFFER, _frame_buffer_vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(frame_buffer_vertices),
			frame_buffer_vertices,
			GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), reinterpret_cast<void*>(offsetof(vertex_t, position)));

		// texture position
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(
			2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), reinterpret_cast<void*>(offsetof(vertex_t, texture_position)));

		_frame_buffer_shader.bind();
		glUniform1i(_frame_buffer_shader.get_attribute_location("texture_sample"), 0);
	}

	_render_state.restore();

	// call once on init with default parameters, this way we guarentee we have something setup
	update_screen_size(default_window_size);
}

render_api::~render_api()
{
	glDeleteFramebuffers(1, &_frame_buffer);

	glDeleteTextures(1, &_texture_atlas);
	glDeleteTextures(1, &_frame_buffer_texture);

	glDeleteBuffers(1, &_vertex_buffer);
	glDeleteBuffers(1, &_frame_buffer_vertex_buffer);

	glDeleteBuffers(1, &_index_buffer);

	glDeleteVertexArrays(1, &_vertex_array);
	glDeleteVertexArrays(1, &_frame_buffer_vertex_array);
}

void render_api::bind_atlas(const uint8_t* data, int width, int height)
{
	// only use 1 atlas ever
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

	// update our projection matrix :P im lazy so we using IMGUi's matrix
	const auto w = static_cast<float>(window_size.x);
	const auto h = static_cast<float>(window_size.y);

	// orthographic projection matrix, very swag i know
	const float projection_matrix[4][4] = {
		{2.f / w,	0.f,		0.f,	0.f},
		{0.f,		-2.f / h,	0.f,	0.f},
		{0.f,		0.f,		-1.f,	0.f},
		{-1.f,	1.f,		0.f,	1.f},
	};

	// bind uniforms n shit to our shader
	GLuint last_program = 0;
	glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>(&last_program));

	// update all three shaders
	{
		_normal_shader.bind();
		glUniformMatrix4fv(
			_normal_shader.get_attribute_location("projection_matrix"),
			1,
			GL_FALSE,
			&projection_matrix[0][0]);
	}

	{
		_sdf_shader.bind();
		glUniformMatrix4fv(
			_sdf_shader.get_attribute_location("projection_matrix"),
			1,
			GL_FALSE,
			&projection_matrix[0][0]);
	}

	{
		_sdf_outline_shader.bind();
		glUniformMatrix4fv(
			_sdf_outline_shader.get_attribute_location("projection_matrix"),
			1,
			GL_FALSE,
			&projection_matrix[0][0]);
	}

	glUseProgram(last_program);

	// update frame buffer texture
	create_frame_buffer_texture(window_size);
}

void render_api::update_frame_buffer(const render_command& render_command)
{
	// backup render state
	_render_state.capture();

	// bind our frame buffer, this means everything we draw will now render to the frame buffer
	// we also need to clear the buffer since its a new render target
	glBindFramebuffer(GL_FRAMEBUFFER, _frame_buffer);
	glClear(GL_COLOR_BUFFER_BIT);

	// set up our own render state
	glEnable(GL_BLEND);
	glEnable(GL_SCISSOR_TEST);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);

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

	// bind our atlas, every texture should live on the atlas
	glBindTexture(GL_TEXTURE_2D, _texture_atlas);

	for (uint32_t i = 0; i < render_command.batch_count; i++)
	{
		const auto& batch = render_command.batches.at(i);

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

void render_api::draw_frame_buffer()
{
	if (_flush_buffers)
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	// backup render state
	_render_state.capture();

	// set up our own render state
	glEnable(GL_BLEND);

	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);

	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(
		GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA,
		GL_ONE,
		GL_ONE_MINUS_SRC_ALPHA);

	// use whole screen as viewport, but we scissor regions in each batch
	glViewport(0, 0, _window_size.x, _window_size.y);

	// bind master texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _frame_buffer_texture);

	_frame_buffer_shader.bind();

	glBindVertexArray(_frame_buffer_vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, _frame_buffer_vertex_buffer);

	glDrawArrays(GL_TRIANGLES, 0, frame_buffer_vertex_count);

	// this will restore the old frame buffer if it was set
	_render_state.restore();
}

void render_api::create_frame_buffer_texture(const lib::point2Di& window_size)
{
	// save previous state
	GLuint last_texture = 0;
	GLuint last_frame_buffer = 0;

	glGetIntegerv(GL_ACTIVE_TEXTURE, reinterpret_cast<GLint*>(&last_texture));
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, reinterpret_cast<GLint*>(&last_frame_buffer));

	if (_frame_buffer_texture)
	{
		glDeleteTextures(1, &_frame_buffer_texture);
	}

	glGenTextures(1, &_frame_buffer_texture);
	glBindTexture(GL_TEXTURE_2D, _frame_buffer_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		window_size.x,
		window_size.y,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		nullptr);

	glBindFramebuffer(GL_FRAMEBUFFER, _frame_buffer);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		_frame_buffer_texture,
		0);

	if (const auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER); status != GL_FRAMEBUFFER_COMPLETE)
	{
		lib_log_d("render_api: failed to update frame buffer, status: ", status);
		assert(false);
	}

	// restore previous state
	glBindFramebuffer(GL_FRAMEBUFFER, last_frame_buffer);
	glBindTexture(GL_TEXTURE_2D, last_texture);
}