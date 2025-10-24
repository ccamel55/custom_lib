#pragma once

#include <module_core/type/point/point4D.hpp>

#include <module_core/type/bitflag.hpp>

#include <module_render/backend/Device_Common.hpp>

#include <module_render/geometry/Geometry_2D_Base.hpp>
#include <module_render/geometry/types/draw_list.hpp>

#include <module_render/types/buffer_object.hpp>

#include <module_render/util/ShaderFactory.hpp>
#include <module_render/util/TextureFactory.hpp>

namespace lib::render {

class Geometry_2D;
using Geometry_2D_Observer = Geometry_2D_Base<Geometry_2D>;

class Geometry_2D final : public Geometry_2D_Observer {
public:
    explicit Geometry_2D(
        const nvrhi::DeviceHandle& device,
        const std::shared_ptr<FontFactory>& font_factory,
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

    //! Create a new font
    //! \param path relative path from font folder
    //! \param height font height in pixels
    //! \returns font id used to render the font
    std::expected<geometry::Font_Id, std::string> add_font(const std::filesystem::path& path, float height);

    //! Explicitly remove a font
    //! \param id font id
    void remove_font(const geometry::Font_Id& id);

public:
    //! Draw a texture in full
    //! \param pos position in world space
    //! \param size size in world unit length
    //! \param texture texture to draw
    //! \param alpha opacity of texture
    void d_texture(
        const point2Df& pos,
        const point2Df& size,
        const geometry::Texture_Id& texture,
        uint8_t alpha = 255
    );

    //! Draw line
    //! \param pos_1 starting point in world space
    //! \param pos_2 end point in world space
    //! \param color_1 line gradient color start
    //! \param color_2 line gradient color end
    void d_line(
        const point2Df& pos_1,
        const point2Df& pos_2,
        const color& color_1,
        const color& color_2
    );

    //! Draw filled triangle
    //! \param vert_1 vertex 1
    //! \param vert_2 vertex 2
    //! \param vert_3 vertex 3
    //! \param color_1 vertex 1 color
    //! \param color_2 vertex 2 color
    //! \param color_3 vertex 3 color
    void d_triangle_filled(
        const point2Df& vert_1,
        const point2Df& vert_2,
        const point2Df& vert_3,
        const color& color_1,
        const color& color_2,
        const color& color_3
    );

    //! Draw filled box
    //! \param pos_tl position of top left corner
    //! \param pos_tr position of top right corner
    //! \param pos_br position of bottom right corner
    //! \param pos_bl position of bottom left corner
    //! \param color_tl color of top left corner
    //! \param color_tr color of top right corner
    //! \param color_br color of bottom right corner
    //! \param color_bl color of bottom left corner
    void d_box_fill(
        const point2Df& pos_tl,
        const point2Df& pos_tr,
        const point2Df& pos_br,
        const point2Df& pos_bl,
        const color& color_tl,
        const color& color_tr,
        const color& color_br,
        const color& color_bl
    );

    //! Draw text
    //! \param pos position in world space
    //! \param color color of font
    //! \param id font id
    //! \param text string to draw
    //! \param flags draw flags
    void d_text(
        const point2Df& pos,
        const color& color,
        const geometry::Font_Id& id,
        const std::string& text,
        bitflag flags = font_flags::None
    );

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

    geometry::FontList _font;

    bool _update_constant_buffer    = false;
    bool _update_pipeline           = false;

    std::shared_ptr<FontFactory> _font_factory;
    std::shared_ptr<ShaderFactory> _shader_factory;
    std::shared_ptr<TextureFactory> _texture_factory;
};

}
