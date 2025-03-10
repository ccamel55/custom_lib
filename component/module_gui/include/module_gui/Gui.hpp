#pragma once

#include <module_input/Input.hpp>

#include <module_render/Render.hpp>
#include <module_render/util/FontFactory.hpp>
#include <module_render/util/FrameBuffer.hpp>
#include <module_render/util/Image.hpp>
#include <module_render/util/ShaderFactory.hpp>
#include <module_render/util/TextureBlit.hpp>
#include <module_render/util/TextureFactory.hpp>

#include <module_gui/node/ContainerNode.hpp>
#include <module_gui/node/ControlNode.hpp>
#include <module_gui/node/DecoratorNode.hpp>
#include <module_gui/node/WindowNode.hpp>

#include <module_gui/StyleProvider.hpp>

namespace lib::gui {

namespace detail {

//! Gui frame buffers we will draw to
//! \note in the future we might need to have more than one frame buffer (for 3d and off screen rendering)
enum class FrameBuffer_Id: uint32_t {
    Geometry_2d,

    // Must always be last
    Num_FrameBuffer_Id
};

//! Gui image targets that get drawn too
enum class Image_Id: uint32_t {
    Geometry_2d_ColorTarget,

    // Must always be last
    Num_Image_Id
};

using FrameBuffer   = render::FrameBuffer<FrameBuffer_Id, static_cast<size_t>(FrameBuffer_Id::Num_FrameBuffer_Id)>;
using Image         = render::Image<Image_Id, static_cast<size_t>(Image_Id::Num_Image_Id)>;

}

//! Set global styler
//! \param styler styler instance
void SetStyler(const std::shared_ptr<StyleProvider>& styler);

//! Gui context, manages render resources and implements basic window manager
class Gui final : public render::RenderPass, public input::InputPass {
public:
    Gui(
        const nvrhi::DeviceHandle& device,
        const std::shared_ptr<render::FontFactory>& font_factory,
        const std::shared_ptr<render::ShaderFactory>& shader_factory,
        const std::shared_ptr<render::TextureFactory>& texture_factory
    );

    //
    // Base class implementations
    //

    void update_input(bitflag input_type, const input::InputObserver& input) override;
    void update_frame(const render::FrameInterval& interval) override;

    void render(nvrhi::IFramebuffer* frame_buffer) override;

    void back_buffer_resizing() override;
    void back_buffer_resized(const point2Di& size) override;

    //
    // Gui implementations
    //

    //! Add a window to be managed
    //! \param window window to add
    void AddWindow(const std::shared_ptr<WindowNode>& window);

private:

    //
    // Render resources
    //

    render::Geometry_2D _geometry_2d;
    render::TextureBlit _blit;
    detail::Image _image;
    detail::FrameBuffer _frame_buffer;
    nvrhi::CommandListHandle _command_list;

    //
    // Gui resources
    //

    std::vector<std::shared_ptr<WindowNode>> m_windows;

};

}