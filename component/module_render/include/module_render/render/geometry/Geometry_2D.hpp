#pragma once

#include <module_render/render/geometry/Geometry_Common.hpp>
#include <module_render/render/geometry/types/constant_buffer.hpp>
#include <module_render/render/geometry/types/vertex.hpp>

#include <module_render/types/bindings.hpp>
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
        update_constant = true;

        const auto centre_pos = pos;

        _draw.backing_vertices.emplace_back(centre_pos.x, centre_pos.y - size / 2, 0.0, 0.5, 0.0, color[0], color[1], color[2], color[3]);
        _draw.backing_vertices.emplace_back(centre_pos.x + size / 2, centre_pos.y + size / 2, 0.0, 1.0, 1.0, color[0], color[1], color[2], color[3]);
        _draw.backing_vertices.emplace_back(centre_pos.x - size / 2, centre_pos.y + size / 2, 0.0, 0.0, 1.0, color[0], color[1], color[2], color[3]);

        _draw.backing_indices.emplace_back(_draw.backing_indices.size());
        _draw.backing_indices.emplace_back(_draw.backing_indices.size());
        _draw.backing_indices.emplace_back(_draw.backing_indices.size());
    }

private:
    nvrhi::CommandListHandle _command_list;
    nvrhi::CommandListHandle _command_list_blit;

    nvrhi::GraphicsPipelineHandle _pipeline;

    nvrhi::TextureHandle _color_buffer;
    nvrhi::FramebufferHandle _color_frame_buffer; // TODO: Create framebuffer object

    TextureBlit _blit;

    buffer_object_t _constant_buffer;
    shader_program_t _shader;
    bindings_t _binding;
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
