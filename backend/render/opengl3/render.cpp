#include <backend/render/opengl3/render.hpp>
#include <common/logger.hpp>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace lib::backend;

void renderer::init_opengl()
{
	opengl3::vertex_layout vertex_layout = {};

	// pos x y
	vertex_layout.add_parameter(2, GL_FLOAT, false);

	// colours r g b a
	vertex_layout.add_parameter(4, GL_UNSIGNED_BYTE, true);

	// texture positions u v
	vertex_layout.add_parameter(2, GL_FLOAT, false);

	auto vertex_buffer_object = std::make_unique<opengl3::vertex_buffer_object>(nullptr, sizeof(opengl3::vertex_t) * opengl3::MAX_VERTICES);

	_vertex_array_object = std::make_unique<opengl3::vertex_array_object>(std::move(vertex_buffer_object), vertex_layout);

	constexpr char vertex_shader_color[] = R"(
        #version 410 core
        layout(location = 0) in vec2 vertexPosition_modelspace;
        layout(location = 1) in vec4 vertexColor;
        layout(location = 2) in vec2 vertexUV;

        out vec4 fragmentColor;
        uniform mat4 MVP;

        void main() {
            gl_Position = MVP * vec4(vertexPosition_modelspace, 0.f, 1.f);
            fragmentColor = vertexColor;
        }
    )";

	constexpr char fragment_shader_color[] = R"(
        #version 410 core
        in vec4 fragmentColor;
        out vec4 color;

        void main() {
            color = fragmentColor;
        }
    )";

	_color_shader = std::make_unique<opengl3::shader_t>(vertex_shader_color, fragment_shader_color);

	constexpr char vertex_shader_texture[] = R"(
        #version 410 core
        layout(location = 0) in vec2 vertexPosition_modelspace;
        layout(location = 1) in vec4 vertexColor;
        layout(location = 2) in vec2 vertexUV;

        out vec4 fragmentColor;
        out vec2 uv;

        uniform mat4 MVP;

        void main() {
            gl_Position = MVP * vec4(vertexPosition_modelspace, 0.f, 1.f);
            fragmentColor = vertexColor;
            uv = vertexUV;
        }
    )";

	constexpr char fragment_shader_texture[] = R"(
        #version 410 core
        in vec4 fragmentColor;
        in vec2 uv;

        out vec4 color;

        uniform sampler2D myTexture;

        void main() {
            float mipmapLevel = textureQueryLod(myTexture, uv).x;
            color = textureLod(myTexture, uv, mipmapLevel) * fragmentColor;
        }
    )";

	_texture_shader = std::make_unique<opengl3::shader_t>(vertex_shader_texture, fragment_shader_texture);

	// enable texture slot 0, which is what we will bind our fonts to
	glActiveTexture(GL_TEXTURE0);
}

void renderer::add_vertex(const opengl3::vertex_t* vertices, GLsizei num_vertices, GLenum primitive, GLuint texture_id)
{
	opengl3::batch_t* current_internal_batch = &_internal_batches.front();

	// make sure we have enough space for our vertices
	if (current_internal_batch->vertex_count + num_vertices > opengl3::MAX_VERTICES)
	{
		// if not enough space, add new internal batch
		current_internal_batch = &_internal_batches.emplace();
	}

	opengl3::vertex_batch_t* current_batch;

	// first batch, set primitive and texture then skip check
	if (current_internal_batch->batch_count == 0)
	{
		current_internal_batch->batch_count += 1;
		current_batch = &current_internal_batch->vertex_batch.at(0);

		current_batch->primitive = primitive;
		current_batch->texture_id = texture_id;
	}
	else
	{
		// make sure our batch is the same as the last
		current_batch = &current_internal_batch->vertex_batch.at(current_internal_batch->batch_count - 1);

		if (current_batch->primitive != primitive || current_batch->texture_id != texture_id)
		{
			// use a new batch if current vertex differs
			current_internal_batch->batch_count += 1;
			current_batch = &current_internal_batch->vertex_batch.at(current_internal_batch->batch_count - 1);

			current_batch->primitive = primitive;
			current_batch->texture_id = texture_id;
		}
	}

	// write vector to our vertices vector
	std::memcpy(&current_internal_batch->vertices.at(current_internal_batch->vertex_count), vertices, num_vertices * sizeof(opengl3::vertex_t));

	current_internal_batch->vertex_count += num_vertices;
	current_batch->vertex_count += num_vertices;
}

void renderer::init_instance(void* init_data)
{
	if (_created_instance)
	{
		lib_log_w("renderer: could not create new instance, instance already exists");
		return;
	}

	if (gladLoadGL() == 0)
	{
		lib_log_e("renderer: could not load opengl");
		return;
	}

	_created_instance = true;

	lib_log_d("renderer: initialised new opengl instance");
	lib_log_d("renderer: " << glGetString(GL_VERSION));

	// setup color blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// set background color
	glClearColor(0.15, 0.15, 0.15, 1);

	init_opengl();
}

void renderer::destroy_instance()
{
	if (!_created_instance)
	{
		lib_log_w("renderer: destroy instance, maybe using context?");
		return;
	}

	_created_instance = false;
	reset();
}

void renderer::bind_context(void* context)
{
	if (_created_instance)
	{
		lib_log_w("renderer: could not bind context, instance exists?");
		return;
	}

	_created_instance = true;
	init_opengl();
}

void renderer::remove_context()
{
	if (!_created_instance)
	{
		lib_log_w("renderer: could unbind context");
		return;
	}

	_created_instance = false;
	reset();
}

void renderer::reset()
{
	if (_vertex_array_object)
	{
		_vertex_array_object.reset();
		_vertex_array_object = nullptr;
	}

	_color_shader.reset();
	_texture_shader.reset();
}

void renderer::set_window_size(const common::point2Di& window_size)
{
	if (!_created_instance)
	{
		lib_log_e("renderer: updated window size without initialising renderer");
		return;
	}

	// call original function
	renderer_base::set_window_size(window_size);

	// generate new projection matrix and update projection matrix in shaders
	const auto projection_matrix = glm::ortho(0.f, static_cast<float>(window_size._x),  static_cast<float>(window_size._y), 0.f);

	// update uniform information
	glUseProgram(_color_shader->shader_program_id);
	glUniformMatrix4fv(glGetUniformLocation(_color_shader->shader_program_id, "MVP"), 1, GL_FALSE, &projection_matrix[0][0]);

	glUseProgram(_texture_shader->shader_program_id);
	glUniformMatrix4fv(glGetUniformLocation(_texture_shader->shader_program_id, "MVP"), 1, GL_FALSE, &projection_matrix[0][0]);

	glUseProgram(0);
}

void renderer::render_start()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	_internal_batches.emplace();
}

void renderer::render_finish()
{
	_vertex_array_object->bind();

	while (!_internal_batches.empty())
	{
		const auto& data = _internal_batches.front();

		if (data.vertex_count > 0)
		{
			// bind vertex array
			glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(data.vertex_count * sizeof(opengl3::vertex_t)), data.vertices.data());

			// render each batch
			GLint offset = 0;

			for (size_t i = 0; i < data.batch_count; i++)
			{
				const auto& batch = data.vertex_batch.at(i);

				if (batch.texture_id != 0)
				{
					// use texture shader and bind texture
					glUseProgram(_texture_shader->shader_program_id);
//					glBindTexture(GL_TEXTURE_2D, uTextureID);
				}
				else
				{
					// use color shader
					glUseProgram(_color_shader->shader_program_id);
				}

				glDrawArrays(batch.primitive, offset, batch.vertex_count);
				offset += batch.vertex_count;
			}
		}

		_internal_batches.pop();
	}

	_vertex_array_object->unbind();
}

void renderer::add_font(common::fnv1a_t font_hash, const std::string& font_name, size_t height, size_t weight)
{

}

void renderer::draw_string(
	lib::common::fnv1a_t font_hash,
	const lib::common::point2Di& pos,
	const lib::common::color& color,
	const std::string& string,
	render_flags flags)
{
}

void renderer::draw_rect(const lib::common::point4Di& area, const lib::common::color& color, render_flags flags)
{
	constexpr auto num_vertices = 6;
	const opengl3::vertex_t vertices[num_vertices] =
	{
			opengl3::vertex_t(static_cast<float>(area._x), static_cast<float>(area._y), color, 0.f, 0.f),
			opengl3::vertex_t(static_cast<float>(area._x + area._w), static_cast<float>(area._y), color, 1.f, 0.f),
			opengl3::vertex_t(static_cast<float>(area._x + area._w), static_cast<float>(area._y + area._z), color, 1.f, 1.f),

			opengl3::vertex_t(static_cast<float>(area._x), static_cast<float>(area._y), color, 0.f, 0.f),
			opengl3::vertex_t(static_cast<float>(area._x + area._w), static_cast<float>(area._y + area._z), color, 1.f, 1.f),
			opengl3::vertex_t(static_cast<float>(area._x), static_cast<float>(area._y + area._z), color, 0.f, 1.f),
	};

	add_vertex(vertices, num_vertices, GL_TRIANGLES, 0);
}

void renderer::draw_rect_gradient(
	const lib::common::point4Di& area,
	const lib::common::color& color1,
	const lib::common::color& color2,
	render_flags flags)
{

}

void renderer::draw_circle(
	const lib::common::point2Di& pos, float radius, const lib::common::color& color, render_flags flags)
{
}

void renderer::draw_circle_gradient(
	const lib::common::point2Di& pos,
	float radius,
	const lib::common::color& color1,
	const lib::common::color& color2,
	render_flags flags)
{

}

void renderer::draw_triangle(
	const lib::common::point2Di& pos1,
	const lib::common::point2Di& pos2,
	const lib::common::point2Di& pos3,
	const lib::common::color& color,
	render_flags flags)
{
	constexpr auto num_vertices = 3;
	const opengl3::vertex_t vertices[num_vertices] =
		{
			opengl3::vertex_t(static_cast<float>(pos1._x), static_cast<float>(pos1._y), color, 0.f, 0.f),
			opengl3::vertex_t(static_cast<float>(pos2._x), static_cast<float>(pos2._y), color, 0.f, 0.f),
			opengl3::vertex_t(static_cast<float>(pos3._x), static_cast<float>(pos3._y), color, 0.f, 0.f),
		};

	add_vertex(vertices, num_vertices, GL_TRIANGLES, 0);
}

void renderer::draw_triangle_gradient(
	const lib::common::point2Di& pos1,
	const lib::common::point2Di& pos2,
	const lib::common::point2Di& pos3,
	const lib::common::color& color1,
	const lib::common::color& color2,
	render_flags flags)
{

}
