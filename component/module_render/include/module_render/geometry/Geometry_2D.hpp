#pragma once

#include <module_core/NoCopy.hpp>

#include <module_render/backend/Device_Common.hpp>

#include <module_render/geometry/types/alias.hpp>
#include <module_render/geometry/types/draw_list.hpp>
#include <module_render/geometry/types/vertex.hpp>

#include <module_render/types/buffer_object.hpp>

#include <module_render/util/ShaderFactory.hpp>
#include <module_render/util/TextureFactory.hpp>

namespace lib::render {

class Geometry_2D final : public NoCopy {
public:
    explicit Geometry_2D(
        const nvrhi::DeviceHandle& device,
        const std::shared_ptr<ShaderFactory>& shader_factory,
        const std::shared_ptr<TextureFactory>& texture_factory
    );

    void draw_geometry(const nvrhi::CommandListHandle& command_list, nvrhi::IFramebuffer* frame_buffer);
    void back_buffer_resizing();

    //! Create a new renderable image from disk
    //! \param path relative path from texture folder
    //! \returns texture ID used to render texture
    std::expected<geometry::Texture_Id, std::string> add_texture(const std::filesystem::path& path);

    //! Create a new renderable image from memory
    //! \param data_ptr pointer to image data
    //! \param size size of image (width x height)
    //! \returns texture ID used to render texture
    std::expected<geometry::Texture_Id, std::string> add_texture(const uint8_t* data_ptr, const point2Di& size);

    //! Explicitly remove an image from image list
    //! \param id texture id
    void remove_texture(const geometry::Texture_Id& id);

public:
    void triangle(const point2Df& pos, const size_t size, const std::array<uint8_t, 4> color = { 255, 255, 255, 255 }) {

        const auto centre_pos = pos;

        _draw.prepare_draw(_texture_default, geometry::Pipeline_Id::Geometry_Texture);

        size_t first_vertex_index;
        std::span<geometry::vertex_t> vertices = _draw.emplace_vertices(first_vertex_index, 3);

        vertices[0] = geometry::vertex_t(centre_pos.x, centre_pos.y - size / 2, 0.0, 0.5, 0.0, color[0], color[1], color[2], color[3]);
        vertices[1] = geometry::vertex_t(centre_pos.x + size / 2, centre_pos.y + size / 2, 0.0, 1.0, 1.0, color[0], color[1], color[2], color[3]);
        vertices[2] = geometry::vertex_t(centre_pos.x - size / 2, centre_pos.y + size / 2, 0.0, 0.0, 1.0, color[0], color[1], color[2], color[3]);

        std::span<geometry::index_t> indices = _draw.emplace_indices(3);

        indices[0] = first_vertex_index + 0;
        indices[1] = first_vertex_index + 1;
        indices[2] = first_vertex_index + 2;
    }

private:
    nvrhi::DeviceHandle _device;
    nvrhi::BindingLayoutHandle _binding_layout;

    geometry::Pipeline _pipeline;
    geometry::BindingSet _binding_set;

    buffer_object_t _vertex_buffer;
    buffer_object_t _index_buffer;
    buffer_object_t _constant_buffer;

    nvrhi::ShaderHandle _vertex_shader;
    nvrhi::ShaderHandle _pixel_shader;
    nvrhi::InputLayoutHandle _vertex_layout;

    geometry::draw_list_t _draw;
    nvrhi::SamplerHandle _sampler; // TODO: cache sampler

    geometry::TextureList _texture;
    geometry::Texture_Id _texture_default;

    bool _update_constant_buffer    = false;
    bool _update_pipeline           = false;

    std::shared_ptr<ShaderFactory> _shader_factory;
    std::shared_ptr<TextureFactory> _texture_factory;
};

}
