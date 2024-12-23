#pragma once

#include <module_render/render/geometry/Geometry_Common.hpp>
#include <module_render/render/geometry/types/constant_buffer.hpp>
#include <module_render/render/geometry/types/vertex.hpp>

#include <module_render/types/buffer_object.hpp>

#include <module_render/util/ShaderFactory.hpp>
#include <module_render/util/TextureFactory.hpp>


namespace lib::render {

class Geometry_2D final : Geometry_Common {
public:
    explicit Geometry_2D(
        const nvrhi::DeviceHandle& device,
        const std::filesystem::path& shader_folder,
        const std::filesystem::path& texture_folder,
        const std::unique_ptr<ShaderFactory>& shader_factory,
        const std::unique_ptr<TextureFactory>& texture_factory
    );

    void draw_geometry(nvrhi::IFramebuffer* frame_buffer) override;
    void back_buffer_resizing() override;

public:
    // BAD!! FUCK OFF!
    void triangle(const point2Df& pos, const size_t size, const std::array<uint8_t, 4> color = { 255, 255, 255, 255 }) {

        _num_vertices +=3;

        const auto centre_pos = pos;

        _vertices[_num_vertices - 3] = detail::vertex_t(centre_pos.x, centre_pos.y - size / 2, 0.0, 0.5, 0.0, color[0], color[1], color[2], color[3]);
        _vertices[_num_vertices - 2] = detail::vertex_t(centre_pos.x + size / 2, centre_pos.y + size / 2, 0.0, 1.0, 1.0, color[0], color[1], color[2], color[3]);
        _vertices[_num_vertices - 1] = detail::vertex_t(centre_pos.x - size / 2, centre_pos.y + size / 2, 0.0, 0.0, 1.0, color[0], color[1], color[2], color[3]);

        _num_indices +=3;

        _indices[_num_indices - 3] = _num_indices - 3;
        _indices[_num_indices - 2] = _num_indices - 2;
        _indices[_num_indices - 1] = _num_indices - 1;
    }

private:
    nvrhi::ShaderHandle _vertex_shader;
    nvrhi::ShaderHandle _pixel_shader;

    nvrhi::InputLayoutHandle _input_layout;

    buffer_object_t _vertex_buffer;
    buffer_object_t _index_buffer;
    buffer_object_t _constant_buffer;

    nvrhi::SamplerHandle _sampler;
    nvrhi::TextureHandle _texture;

    nvrhi::BindingLayoutHandle _binding_layout;
    nvrhi::BindingSetHandle _binding_set;

    nvrhi::CommandListHandle _command_list;
    nvrhi::GraphicsPipelineHandle _pipeline;

    // Our stuff:

    size_t _num_vertices    = 0;
    size_t _num_indices     = 0;

    detail::vertex_array_t _vertices;
    detail::index_array_t _indices;
};

}
