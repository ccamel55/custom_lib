#include <backend/render/opengl3/render.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace lib::backend;

namespace
{
	// call once, set everything up
	constexpr char color_vertex_shader[] = R"(
				#version 330 core

                layout(location = 0) in vec2 vposition;
                layout(location = 1) in vec4 vcolor;

                out vec4 vert_col;
                uniform mat4 u_MVP;

                void main()
                {
                   gl_Position = u_MVP * vec4(vposition, 0, 1);
                   vert_col = vcolor;
                };
		)";

	constexpr char color_fragment_shader[] = R"(
				#version 330 core

                out vec4 out_col;
                in vec4 vert_col;

                void main()
                {
                    out_col = vert_col;
                };
		)";

	constexpr char texture_vertex_shader[] = R"(
				#version 330 core

                layout(location = 0) in vec2 vposition;
                layout(location = 1) in vec4 vcolor;
                layout(location = 2) in vec2 texture_pos;

                out vec4 vert_col;
                out vec2 v_text_pos;
                uniform mat4 u_MVP;

                void main()
                {
                   gl_Position = u_MVP * vec4(vposition, 0, 1);
                   vert_col = vcolor;
                   v_text_pos = texture_pos;
                };
		)";

	constexpr char texture_fragment_shader[] = R"(
                 #version 330 core

                out vec4 out_col;
                in vec4 vert_col;
                in vec2 v_text_pos;

                uniform sampler2D u_Texture;

                void main()
                {
                    float c = texture(u_Texture, v_text_pos).r;
                    out_col = vec4(1, 1, 1, c) * vert_col;
                };
		)";
}

void renderer::init_internal()
{
	gl3::vertex_layout_manager vertex_layout = {};
	{
		// pos
		vertex_layout.add_layout(2, GL_FLOAT, false);

		// color
		vertex_layout.add_layout(4, GL_FLOAT, false);

		// texture pos
		vertex_layout.add_layout(2, GL_FLOAT, false);
	}

	_vertex_manager = gl3::vertex_manager(render::MAX_VERTEX_COUNT, sizeof(render::vertex_2d_t), vertex_layout);

	_color_shader = gl3::shader_manager(color_vertex_shader, color_fragment_shader);
	_texture_shader = gl3::shader_manager(texture_vertex_shader, texture_fragment_shader);

	// create projection matrix based on side of screen
	// todo: pass in resolution properly
	const auto projectionMatrix = glm::ortho(0.f, 1280.f, 720.f, 0.f, 0.f, 1.f);

	// apply draw matrix
	_color_shader.bind_uniform_mat4("u_MVP", projectionMatrix);
	_texture_shader.bind_uniform_mat4("u_MVP", projectionMatrix);

	// use first slot as texture sampler
	_texture_shader.bind_uniform("u_Texture", 0);
}

void renderer::init_instance()
{
	// load GLAD
	gladLoadGL();

	// set "background" color
	glClearColor(0.1f, 0.1f, 0.1f, 1.f);

	init_internal();
}

void renderer::destroy_instance()
{
}

void renderer::bind_context(void* context)
{
	init_internal();
}

void renderer::remove_context()
{
}

void renderer::reset()
{
}

void renderer::render_start()
{
	glClear(GL_COLOR_BUFFER_BIT);

	// create a new batch to draw into
	_render_batches.emplace_back();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	std::vector<render::vertex_2d_t> verts = {};

	constexpr float x = 10.f;
	constexpr float y = 10.f;

	constexpr float w = 50.f;
	constexpr float h = 50.f;

	verts.emplace_back(x, y, common::color_f(255, 255, 255, 255));
	verts.emplace_back(x + w, y, common::color_f(255, 255, 255, 255));
	verts.emplace_back(x + w, y + h, common::color_f(255, 255, 255, 255));

	verts.emplace_back(x, y, common::color_f(255, 255, 255, 255));
	verts.emplace_back(x + w, y + h, common::color_f(255, 255, 255, 255));
	verts.emplace_back(x, y + h, common::color_f(255, 255, 255, 255));

	add_to_render_list(verts, GL_TRIANGLES);
}

void renderer::render_finish()
{
	_vertex_manager.bind();

	for (const auto& lot : _render_batches)
	{
		if (lot._total_vertex_count <= 0)
		{
			continue;
		}

		// does all the grunt work for us, just memcpy the data into the buffer
		_vertex_manager.bind_buffer_data(lot._vertices);

		int vertex_offset = 0;
		for (const auto& batch : lot._batches)
		{
			if (batch._texture_id == -1)
			{
				_color_shader.bind();
			}
			else
			{
				_texture_shader.bind();
			}

			glDrawArrays(batch._primitive_type, vertex_offset, batch._vertex_count);
			vertex_offset += batch._vertex_count;
		}
	}

	_vertex_manager.unbind();
	_render_batches.clear();
}

void renderer::add_font(common::hash_t font_hash, const std::string& font_name, size_t height, size_t weight)
{
}

void renderer::draw_string(common::hash_t font_hash, const common::point2Di& pos, const common::color& color, const std::string& string, render_flags flags)
{
}

void renderer::draw_rect(const common::point4Di& area, const common::color& color, render_flags flags)
{
}

void renderer::draw_rect_gradient(const common::point4Di& area, const common::color& color1, const common::color& color2, render_flags flags)
{
}

void renderer::draw_rect_filled(const common::point4Di& area, const common::color& color, render_flags flags)
{
}

void renderer::draw_rect_filled_gradient(const common::point4Di& area, const common::color& color1, const common::color& color2, render_flags flags)
{
}

void renderer::draw_circle(const common::point2Di& pos, float radius, const common::color& color, render_flags flags)
{
}

void renderer::draw_circle_filled(const common::point2Di& pos, float radius, const common::color& color, render_flags flags)
{
}

void renderer::draw_circle_filled_gradient(const common::point2Di& pos, float radius, const common::color& color1, const common::color& color2, render_flags flags)
{
}

void renderer::draw_triangle(const common::point2Di& pos1, const common::point2Di& pos2, const common::point2Di& pos3, const common::color& color, render_flags flags)
{
}

void renderer::draw_triangle_filled(const common::point2Di& pos1, const common::point2Di& pos2, const common::point2Di& pos3, const common::color& color, render_flags flags)
{
}

void renderer::add_to_render_list(const std::vector<render::vertex_2d_t>& vertices, int primitive_type, int texture_id)
{
	// don't even try if it's larger than max count, just gonna infinite loop
	if (vertices.size() >= render::MAX_VERTEX_COUNT)
	{
		return;
	}

	// make new render batch if we have too
	if (_render_batches.back()._total_vertex_count + vertices.size() > render::MAX_VERTEX_COUNT)
	{
		_render_batches.emplace_back();
	}

	auto& curRenderBatch = _render_batches.back();

	if (curRenderBatch._batches.empty() || curRenderBatch._batches.back()._primitive_type != primitive_type || curRenderBatch._batches.back()._texture_id != texture_id)
	{
		curRenderBatch._batches.emplace_back(primitive_type, texture_id);
	}

	auto& curDrawBatch = curRenderBatch._batches.back();

	// copy everything from given vertex into main render vertex
	std::memcpy(&curRenderBatch._vertices.at(curRenderBatch._total_vertex_count), &vertices.at(0), vertices.size() * sizeof(render::vertex_2d_t));

	curRenderBatch._total_vertex_count += vertices.size();
	curDrawBatch._vertex_count += static_cast<int>(vertices.size());
}