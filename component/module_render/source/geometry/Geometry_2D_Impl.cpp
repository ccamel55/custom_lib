#include <module_render/geometry/Geometry_2D.hpp>

using namespace lib::render;

//
// Geometry implementations
//

void Geometry_2D::d_texture(
    const point2Df& pos,
    const point2Df& size,
    const geometry::Texture_Id& texture,
    const uint8_t alpha
) {
    _draw.prepare_draw(texture, geometry::Pipeline_Id::Geometry_Texture);

    size_t first_vertex_index;
    std::span<geometry::vertex_t> vertices = _draw.emplace_vertices(first_vertex_index, 4);

    vertices[0] = geometry::vertex_t(pos.x, pos.y, 0.f, 0.f, 0.f, 255, 255, 255, alpha);
    vertices[1] = geometry::vertex_t(pos.x + size.x, pos.y, 0.f, 1.f, 0.f, 255, 255, 255, alpha);
    vertices[2] = geometry::vertex_t(pos.x + size.x, pos.y + size.y, 0.f, 1.f, 1.f, 255, 255, 255, alpha);
    vertices[3] = geometry::vertex_t(pos.x, pos.y + size.y, 0.f, 0.f, 1.f, 255, 255, 255, alpha);

    std::span<geometry::index_t> indices = _draw.emplace_indices(6);

    indices[0] = first_vertex_index + 0;
    indices[1] = first_vertex_index + 1;
    indices[2] = first_vertex_index + 2;

    indices[3] = first_vertex_index + 0;
    indices[4] = first_vertex_index + 2;
    indices[5] = first_vertex_index + 3;
}

void Geometry_2D::d_line(
    const point2Df& pos_1,
    const point2Df& pos_2,
    const color& color_1,
    const color& color_2
) {
    constexpr float thickness   = 1.f;
    const glm::f32vec2 dir      = glm::normalize(pos_2 - pos_1) * (thickness * 0.5f);

    _draw.prepare_draw(_texture_default, geometry::Pipeline_Id::Geometry_Texture);

    size_t first_vertex_index;
    std::span<geometry::vertex_t> vertices = _draw.emplace_vertices(first_vertex_index, 4);

    vertices[0] = geometry::vertex_t(pos_1.x + dir.y, pos_1.y - dir.x, 0.f, 0.f, 0.f, color_1.r, color_1.g, color_1.b, color_1.a);
    vertices[1] = geometry::vertex_t(pos_2.x + dir.y, pos_2.y - dir.x, 0.f, 0.f, 0.f, color_2.r, color_2.g, color_2.b, color_2.a);
    vertices[2] = geometry::vertex_t(pos_2.x - dir.y, pos_2.y + dir.x, 0.f, 0.f, 0.f, color_2.r, color_2.g, color_2.b, color_2.a);
    vertices[3] = geometry::vertex_t(pos_1.x - dir.y, pos_1.y + dir.x, 0.f, 0.f, 0.f, color_1.r, color_1.g, color_1.b, color_1.a);

    std::span<geometry::index_t> indices = _draw.emplace_indices(6);

    indices[0] = first_vertex_index + 0;
    indices[1] = first_vertex_index + 1;
    indices[2] = first_vertex_index + 2;

    indices[3] = first_vertex_index + 0;
    indices[4] = first_vertex_index + 2;
    indices[5] = first_vertex_index + 3;
}

void Geometry_2D::d_box(
    const point2Df& pos_tl,
    const point2Df& pos_tr,
    const point2Df& pos_br,
    const point2Df& pos_bl,
    const color& color_tl,
    const color& color_tr,
    const color& color_br,
    const color& color_bl
) {
    // Lazy man's way, just call `d_line` 4 times lol
    d_line(pos_tl, pos_tr, color_tl, color_tr);
    d_line(pos_tr, pos_br, color_tr, color_br);
    d_line(pos_br, pos_bl, color_br, color_bl);
    d_line(pos_bl, pos_tl, color_bl, color_tl);
}

void Geometry_2D::d_box_fill(
    const point2Df& pos_tl,
    const point2Df& pos_tr,
    const point2Df& pos_br,
    const point2Df& pos_bl,
    const color& color_tl,
    const color& color_tr,
    const color& color_br,
    const color& color_bl
) {
    _draw.prepare_draw(_texture_default, geometry::Pipeline_Id::Geometry_Texture);

    size_t first_vertex_index;
    std::span<geometry::vertex_t> vertices = _draw.emplace_vertices(first_vertex_index, 4);

    vertices[0] = geometry::vertex_t(pos_tl.x, pos_tl.y, 0.f, 0.f, 0.f, color_tl.r, color_tl.g, color_tl.b, color_tl.a);
    vertices[1] = geometry::vertex_t(pos_tr.x, pos_tr.y, 0.f, 0.f, 0.f, color_tr.r, color_tr.g, color_tr.b, color_tr.a);
    vertices[2] = geometry::vertex_t(pos_br.x, pos_br.y, 0.f, 0.f, 0.f, color_br.r, color_br.g, color_br.b, color_br.a);
    vertices[3] = geometry::vertex_t(pos_bl.x, pos_bl.y, 0.f, 0.f, 0.f, color_bl.r, color_bl.g, color_bl.b, color_bl.a);

    std::span<geometry::index_t> indices = _draw.emplace_indices(6);

    indices[0] = first_vertex_index + 0;
    indices[1] = first_vertex_index + 1;
    indices[2] = first_vertex_index + 2;

    indices[3] = first_vertex_index + 0;
    indices[4] = first_vertex_index + 2;
    indices[5] = first_vertex_index + 3;
}

void Geometry_2D::d_text(
    const point2Df& pos,
    const color& color,
    const geometry::Font_Id& id,
    const std::string& text,
    const bitflag flags
) {
    // Fonts will always use SDF rendering, this allows us to scale the size of the font in the future
    // TODO: implement font scaling
    _draw.prepare_draw(
        id->texture,
        flags.has(font_flags::Outline)
            ? geometry::Pipeline_Id::Geometry_Texture_Sdf_Outline
            : geometry::Pipeline_Id::Geometry_Texture_Sdf
    );

    // Adjust pos based on flags
    point2Df fixed_pos = pos;

    const auto text_height = [&]() -> float {
        return static_cast<float>(id->characters.at(0).spacing.y);
    };

    const auto text_width = [&]() -> float {
        int w = 0;
        for (const auto& c : text) {
            w += id->characters.at(c - CHAR_START).spacing.x;
        }
        return static_cast<float>(w);
    };

    if (flags.has(font_flags::Align_R)) {
        fixed_pos.x -= text_width();
    }
    else if (flags.has(font_flags::Centre_X)) {
        fixed_pos.x -= text_width() / 2;
    }

    if (flags.has(font_flags::Centre_Y)) {
        fixed_pos.y += text_height() / 2;
    }

    // Add each character by its self
    for (const char c: text) {

        const auto& character = id->characters.at(c - CHAR_START);

        if (c != ' ') {

            const auto aligned_pos  = fixed_pos + point2Df(character.align);
            const auto size         = point2Df(character.size);

            size_t first_vertex_index;
            std::span<geometry::vertex_t> vertices = _draw.emplace_vertices(first_vertex_index, 4);

            vertices[0] = geometry::vertex_t(
                aligned_pos.x, aligned_pos.y, 0.f,
                character.atlas_start.x, character.atlas_start.y,
                color.r, color.g, color.b, color.a
            );

            vertices[1] = geometry::vertex_t(
                aligned_pos.x + size.x, aligned_pos.y, 0.f,
                character.atlas_end.x, character.atlas_start.y,
                color.r, color.g, color.b, color.a
            );

            vertices[2] = geometry::vertex_t(
                aligned_pos.x + size.x, aligned_pos.y + size.y, 0.f,
                character.atlas_end.x, character.atlas_end.y,
                color.r, color.g, color.b, color.a
            );

            vertices[3] = geometry::vertex_t(
                aligned_pos.x, aligned_pos.y + size.y, 0.f,
                character.atlas_start.x, character.atlas_end.y,
                color.r, color.g, color.b, color.a
            );

            std::span<geometry::index_t> indices = _draw.emplace_indices(6);

            indices[0] = first_vertex_index + 0;
            indices[1] = first_vertex_index + 1;
            indices[2] = first_vertex_index + 2;

            indices[3] = first_vertex_index + 0;
            indices[4] = first_vertex_index + 2;
            indices[5] = first_vertex_index + 3;
        }

        fixed_pos.x += static_cast<float>(character.spacing.x);
    }
}