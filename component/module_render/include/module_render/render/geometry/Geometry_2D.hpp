#pragma once

#include <module_render/render/geometry/Geometry_Common.hpp>
#include <module_render/render/geometry/types/constant_buffer.hpp>
#include <module_render/render/geometry/types/vertex.hpp>

#include <module_render/types/buffer_object.hpp>
#include <module_render/types/draw_list.hpp>
#include <module_render/types/shader_program.hpp>

#include <module_render/util/ShaderFactory.hpp>
#include <module_render/util/TextureBlit.hpp>
#include <module_render/util/TextureFactory.hpp>

namespace lib::render {
class Geometry_2D final : Geometry_Common {
public:
    explicit Geometry_2D(
        const nvrhi::DeviceHandle& device,
        const std::unique_ptr<ShaderFactory>& shader_factory,
        const std::unique_ptr<TextureFactory>& texture_factory
    );

    void draw_geometry(nvrhi::IFramebuffer* frame_buffer) override;
    void back_buffer_resizing() override;
    void back_buffer_resized(const point2Di& size) override;

public:
    // BAD!! FUCK OFF!
    void triangle(const point2Df& pos, const size_t size, const std::array<uint8_t, 4> color = { 255, 255, 255, 255 }) {

        update_vertex   = true;

        const auto centre_pos = pos;

        size_t first_vertex_index;
        std::span<detail::vertex_t> vertices = _draw.emplace_vertices(first_vertex_index, 3);

        vertices[0] = detail::vertex_t(centre_pos.x, centre_pos.y - size / 2, 0.0, 0.5, 0.0, color[0], color[1], color[2], color[3]);
        vertices[1] = detail::vertex_t(centre_pos.x + size / 2, centre_pos.y + size / 2, 0.0, 1.0, 1.0, color[0], color[1], color[2], color[3]);
        vertices[2] = detail::vertex_t(centre_pos.x - size / 2, centre_pos.y + size / 2, 0.0, 0.0, 1.0, color[0], color[1], color[2], color[3]);

        std::span<detail::index_t> indices = _draw.emplace_indices(3);

        indices[0] = first_vertex_index + 0;
        indices[1] = first_vertex_index + 1;
        indices[2] = first_vertex_index + 2;
    }

private:
    nvrhi::CommandListHandle _command_list;
    nvrhi::CommandListHandle _command_list_blit;

    TextureBlit _blit;
    Geometry_Image _image;
    Geometry_FrameBuffer _frame_buffer;
    Geometry_Pipeline _pipeline;

    nvrhi::BindingLayoutHandle _binding_layout;
    std::unordered_map<detail::binding_set_desc_key, nvrhi::BindingSetHandle, detail::binding_set_desc_key::hash> _binding_set;

    buffer_object_t _constant_buffer;
    shader_program_t _shader;
    draw_list_t<detail::vertex_t, detail::index_t> _draw;

    // Image
    nvrhi::SamplerHandle _sampler; // TODO: cache sampler
    nvrhi::TextureHandle _texture;

    // Write states
    bool update_vertex      = false;
    bool update_constant    = false;
    size_t _vertex_count    = 0;

};

}
