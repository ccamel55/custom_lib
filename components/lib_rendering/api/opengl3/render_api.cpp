#include <lib_rendering/render_api.hpp>
#include <cassert>

using namespace lib::rendering;

namespace
{
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

render_api::render_api()
	: _normal_shader(vertex_shader, normal_fragment_shader), _sdf_shader(vertex_shader, sdf_fragment_shader),
	_sdf_outline_shader(vertex_shader, sdf_outline_fragment_shader), _vertex_array(0), _vertex_buffer(0), _index_buffer(0)
{
	_render_state.capture();

	// generate buffers

	glGenBuffers(1, &_vertex_buffer);
	glGenBuffers(1, &_index_buffer);
	glGenVertexArrays(1, &_vertex_array);

	// bind attributes to our vertex array object
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

	_render_state.restore();
}

render_api::~render_api()
{
	for (auto texture : _textures)
	{
		glDeleteTextures(1, &texture);
	}

	glDeleteBuffers(1, &_vertex_buffer);
	glDeleteBuffers(1, &_index_buffer);
	glDeleteVertexArrays(1, &_vertex_array);
}

void render_api::add_texture(int id, const uint8_t* data, int width, int height)
{
	// setup how our texture will act
	auto& new_texture = _textures.emplace_back();

	// make sure IDs match
	assert(id == _textures.size() - 1);

	GLuint last_texture = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, reinterpret_cast<GLint*>(&last_texture));

	glGenTextures(1, &new_texture);
	glBindTexture(GL_TEXTURE_2D, new_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, last_texture);
}

void render_api::update_screen_size(const lib::point2Di& window_size)
{
	// save a copy of window size for us to use later
	_window_size = window_size;

	// update our projection matrix :P im lazy so we using IMGUi's matrix
	const auto w = static_cast<float>(window_size._x);
	const auto h = static_cast<float>(window_size._y);

	// orthographic projection matrix, very swag i know
	const float projection_matrix[4][4] = {
		{2.f / w, 0.f,	   0.f,	0.f},
		{0.f,	  -2.f / h, 0.f,	 0.f},
		{0.f,	  0.f,	   -1.f, 0.f},
		{-1.f,	   1.f,		0.f,	 1.f},
	};

	// bind uniforms n shit to our shader
	GLuint last_program = 0;
	glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>(&last_program));

	// update all three shaders
	{
		_normal_shader.bind();

		glUniform1i(_normal_shader.get_attribute_location("texture_sample"), 0);
		glUniformMatrix4fv(
			_normal_shader.get_attribute_location("projection_matrix"),
			1,
			GL_FALSE,
			&projection_matrix[0][0]);
	}

	{
		_sdf_shader.bind();

		glUniform1i(_sdf_shader.get_attribute_location("texture_sample"), 0);
		glUniformMatrix4fv(
			_sdf_shader.get_attribute_location("projection_matrix"),
			1,
			GL_FALSE,
			&projection_matrix[0][0]);
	}

	{
		_sdf_outline_shader.bind();

		glUniform1i(_sdf_outline_shader.get_attribute_location("texture_sample"), 0);
		glUniformMatrix4fv(
			_sdf_outline_shader.get_attribute_location("projection_matrix"),
			1,
			GL_FALSE,
			&projection_matrix[0][0]);
	}

	glUseProgram(last_program);
}

void render_api::draw_render_command(const render_command& render_command, int texture_id)
{
	glClear(GL_COLOR_BUFFER_BIT);

	// backup render state
	_render_state.capture();

	// set up our own render state
	glEnable(GL_BLEND);
	glEnable(GL_SCISSOR_TEST);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);

	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	// bind master texture
	glActiveTexture(GL_TEXTURE0);

	// use whole screen as viewport, but we scissor regions in each batch
	glViewport(0, 0, _window_size._x, _window_size._y);

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

		glBindTexture(GL_TEXTURE_2D, _textures.at(texture_id));

		glScissor(
			batch.clipped_area._x,
			batch.clipped_area._y,
			batch.clipped_area._z - batch.clipped_area._x,
			batch.clipped_area._w - batch.clipped_area._y);

		glDrawElements(
			GL_TRIANGLES,
			static_cast<int>(batch.count),
			GL_UNSIGNED_INT,
			reinterpret_cast<void*>(batch.offset * sizeof(uint32_t)));
	}

	_render_state.restore();
}
