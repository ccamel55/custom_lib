#include <backend/render/opengl3/render.hpp>
#include <common/logger.hpp>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace lib::backend;

namespace
{
    constexpr char vertex_shader[] = R"(
        #version 410 core
        layout(location = 0) in vec2 position;
        layout(location = 1) in vec4 color;
        layout(location = 2) in vec2 uv;

		uniform mat4 projection_matrix;
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
        in vec4 fragment_color;
		in vec2 fragment_uv;

		uniform sampler2D Texture;
		layout (location = 0) out vec4 out_color;

        void main()
		{
            out_color = fragment_color * texture(Texture, fragment_uv.st);
        }
    )";

    constexpr uint8_t white_bitmap[] = {0xff, 0xff, 0xff, 0xff};
}  // namespace

void renderer::init_opengl()
{
    // setup opengl states
    glEnable(GL_BLEND);
    glEnable(GL_SCISSOR_TEST);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

    glBlendEquation(GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    _vertex_array_object = std::make_unique<opengl3::vertex_array_object>(opengl3::MAX_VERTICES);

    _color_texture = std::make_unique<opengl3::texture_t>(white_bitmap, 1, 1, GL_RGBA);
    _shader = std::make_unique<opengl3::shader_t>(vertex_shader, fragment_shader);

    GLenum last_shader = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>(&last_shader));

    // bind texture to slot 0
    glUseProgram(_shader->shader_program_id);
    glUniform1i(glGetUniformLocation(_shader->shader_program_id, "Texture"), 0);

    glUseProgram(last_shader);
}

void renderer::add_batch_break()
{
    // add emtpy batch and update current batch, new batch will be skipped because we check for vertex count
    // in the render function
    _current_internal_batch->batch_count += 1;
    _current_batch = &_current_internal_batch->vertex_batch.at(_current_internal_batch->batch_count - 1);
}

void renderer::draw_vertices(
        const opengl3::vertex_t* vertices,
        uint16_t num_vertices,
        GLenum primitive,
        GLuint texture_id)
{
    if (_internal_batches.empty())
    {
        _current_internal_batch = &_internal_batches.emplace();
        _current_batch = nullptr;
    }
    else
    {
        _current_internal_batch = &_internal_batches.front();

        // make sure we have enough space for our vertices
        if (_current_internal_batch->vertex_count + num_vertices > opengl3::MAX_VERTICES)
        {
            // if not enough space, add new internal batch
            _current_internal_batch = &_internal_batches.emplace();
            _current_batch = nullptr;
        }
    }

    if (_current_batch == nullptr || _current_batch->primitive != primitive ||
        _current_batch->texture_id != texture_id || _current_batch->scissor_rect != _current_scissor_rect)
    {
        // increment render batch count
        add_batch_break();

        // set new batch data to current data
        _current_batch->primitive = primitive;
        _current_batch->texture_id = texture_id;
        _current_batch->scissor_rect = _current_scissor_rect;
    }

    // copy vertices
    std::memcpy(
            &_current_internal_batch->vertices.at(_current_internal_batch->vertex_count),
            vertices,
            num_vertices * sizeof(opengl3::vertex_t));

    _current_internal_batch->vertex_count += num_vertices;
    _current_batch->vertex_count += num_vertices;
}

uint16_t renderer::get_num_vertices() const
{
    if (!_current_internal_batch)
    {
        return 0;
    }

    return _current_internal_batch->vertex_count;
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
    lib_log_d("renderer: {}", *glGetString(GL_VERSION));

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

    if (_shader)
    {
        _shader.reset();
        _shader = nullptr;
    }

    if (_color_texture)
    {
        _color_texture.reset();
        _color_texture = nullptr;
    }
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
    const auto projection_matrix =
            glm::ortho(0.f, static_cast<float>(window_size._x), static_cast<float>(window_size._y), 0.f);

    GLenum last_shader = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>(&last_shader));

    // update uniform
    glUseProgram(_shader->shader_program_id);
    glUniformMatrix4fv(
            glGetUniformLocation(_shader->shader_program_id, "projection_matrix"), 1, GL_FALSE, &projection_matrix[0][0]);

    // restore previous shader
    glUseProgram(last_shader);
}

void renderer::render_start()
{
    // save current render state before rending anything, so we can restore states as to not screw anything up
    _render_state.capture();

    _current_batch = nullptr;
    _current_scissor_rect = common::point4Di(0, 0, _window_size._x, _window_size._y);

    // set viewport to the whole screen in case it has been changed
    glViewport(0, 0, _window_size._x, _window_size._y);

    // enable texture slot 0, which is what we will bind our fonts to
    glActiveTexture(GL_TEXTURE0);
}

void renderer::render_finish()
{
    _vertex_array_object->bind();

    // use our own shader to render our shapes and stuff
    glUseProgram(_shader->shader_program_id);

    while (!_internal_batches.empty())
    {
        const auto& data = _internal_batches.front();

        // bind vertex positions, color and tex coord
        glBindBuffer(GL_ARRAY_BUFFER, _vertex_array_object->get_vertex_buffer());
        glBufferSubData(
                GL_ARRAY_BUFFER,
                0,
                static_cast<GLsizeiptr>(data.vertex_count * sizeof(opengl3::vertex_t)),
                data.vertices.data());

        // render each batch
        GLint offset = 0;

        for (size_t i = 0; i < data.batch_count; i++)
        {
            const auto& batch = data.vertex_batch.at(i);

            if (batch.vertex_count == 0)
            {
                continue;
            }

            // apply scissor
            glScissor(
                    batch.scissor_rect._x,
                    batch.scissor_rect._y,
                    batch.scissor_rect._z - batch.scissor_rect._x,
                    batch.scissor_rect._w - batch.scissor_rect._y);

            // apply texture
            if (batch.texture_id != 0)
            {
                // only apply custom texture when texture id is not 0, else apply "color" texture
                glBindTexture(GL_TEXTURE_2D, batch.texture_id);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, _color_texture->texture_id);
            }

            glDrawArrays(batch.primitive, offset, batch.vertex_count);
            offset += batch.vertex_count;
        }

        _internal_batches.pop();
    }

    _vertex_array_object->unbind();
    _render_state.restore();
}

void renderer::set_scissor_rect(const lib::common::point4Di& rect)
{
    _current_scissor_rect = rect;
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
    opengl3::vertex_t vertices[num_vertices] = {};

    vertices[0] = {{area._x, area._y}, color, {0.f, 0.f}};
    vertices[1] = {{area._x + area._z, area._y + area._w}, color, {1.f, 1.f}};
    vertices[2] = {{area._x + area._z, area._y}, color, {1.f, 0.0}};

    vertices[3] = {{area._x, area._y}, color, {0.f, 1.f}};
    vertices[4] = {{area._x + area._z, area._y + area._w}, color, {1.f, 1.f}};
    vertices[5] = {{area._x, area._y + area._w}, color, {0.f, 1.f}};

    draw_vertices(vertices, num_vertices, GL_TRIANGLES, 0);
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
    constexpr auto num_vertices = CIRCLE_CACHE_SEGMENTS + 1;
    opengl3::vertex_t vertices[num_vertices] = {};

    for (size_t i = 0; i <= CIRCLE_CACHE_SEGMENTS; i++)
    {
        const auto& lookup = _circle_cache.at(i);
        vertices[i] = {
                {pos._x + static_cast<int>(radius * lookup.cos),pos._y + static_cast<int>(radius * lookup.sin)},
                color,
                {0.f,0.f}};
    }

    draw_vertices(vertices, num_vertices, GL_TRIANGLE_FAN, 0);
    add_batch_break();
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
    opengl3::vertex_t vertices[num_vertices] = {};

    vertices[0] = {{pos1._x, pos1._y}, color, {0, 0}};
    vertices[1] = {{pos2._x, pos2._y}, color, {0, 0}};
    vertices[2] = {{pos3._x, pos3._y}, color, {0, 0}};

    draw_vertices(vertices, num_vertices, GL_TRIANGLES, 0);
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
