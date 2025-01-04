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
    //! Draw a texture in full
    //! \param pos position in world space
    //! \param size size in world unit length
    //! \param texture texture to draw
    //! \param alpha opacity of texture
    void d_texture(const point2Df& pos, const point2Df& size, const geometry::Texture_Id& texture, uint8_t alpha = 255);

    //! Draw line
    //! \param pos_1 starting point in world space
    //! \param pos_2 end point in world space
    //! \param color color of line
    //! \param thickness thickness of line
    void d_line(const point2Df& pos_1, const point2Df& pos_2, const color& color, float thickness = 1.0);

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
    nvrhi::ShaderHandle _pixel_shader_sdf;
    nvrhi::ShaderHandle _pixel_shader_sdf_outline;
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
