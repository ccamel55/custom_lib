#include <backend/render/api/opengl3/render_api.hpp>

using namespace lib::backend::render;

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

constexpr char fragment_shader[] = R"(
        #version 410 core

		uniform sampler2D texture;

        in vec4 fragment_color;
		in vec2 fragment_uv;

		layout (location = 0) out vec4 out_color;

        void main()
		{
            out_color = fragment_color;
        }
    )";
}  // namespace

render_api::render_api() :
	_window_size(), _render_state(), _shader(vertex_shader, fragment_shader), _vertex_array(0), _vertex_buffer(0),
	_index_buffer(0)
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
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(
			0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), reinterpret_cast<void*>(offsetof(vertex_t, position)));

		glVertexAttribPointer(
			1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(vertex_t), reinterpret_cast<void*>(offsetof(vertex_t, color)));

		glVertexAttribPointer(
			2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), reinterpret_cast<void*>(offsetof(vertex_t, texture_position)));
	}

	_render_state.restore();
}

render_api::~render_api()
{
	glDeleteBuffers(1, &_vertex_buffer);
	glDeleteBuffers(1, &_index_buffer);

	glDeleteVertexArrays(1, &_vertex_array);
}

texture_id render_api::add_texture()
{
	return 0;
}

void render_api::update_screen_size(const common::point2Di& window_size)
{
	// save a copy of window size for us to use later
	_window_size = window_size;

	// update our projection matrix :P im lazy so we using IMGUi's matrix
	constexpr float L = 0.f;
	constexpr float T = 0.f;

	const auto R = static_cast<float>(window_size._x);
	const auto B = static_cast<float>(window_size._y);

	const float projection_matrix[4][4] = {
		{2.0f / (R - L),	 0.0f,			   0.0f,	 0.0f},
		{0.0f,			   2.0f / (T - B),	   0.0f,	 0.0f},
		{0.0f,			   0.0f,				 -1.0f, 0.0f},
		{(R + L) / (L - R), (T + B) / (B - T), 0.0f,	 1.0f},
	};

	// bind uniforms n shit to our shader
	GLuint last_program;
	glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>(&last_program));

	_shader.bind();

	glUniform1i(_shader.get_attribute_location("texture"), 0);
	glUniformMatrix4fv(_shader.get_attribute_location("projection_matrix"), 1, GL_FALSE, &projection_matrix[0][0]);

	glUseProgram(last_program);
}

void render_api::draw_render_command(const render_command& render_command)
{
	// backup render state
	_render_state.capture();

	_shader.bind();

	// set up our own render state
	glEnable(GL_BLEND);
	glEnable(GL_SCISSOR_TEST);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);

	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

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
