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
        const std::shared_ptr<render::FontFactory>& fontFactory,
        const std::shared_ptr<render::ShaderFactory>& shaderFactory,
        const std::shared_ptr<render::TextureFactory>& textureFactory
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

    //! Checks if GUI is operating in exclusive mode
    //! \returns true if GUI is in exclusive mode, else false
    [[nodiscard]] bool IsExclusive() const;

    //! Toggle window exclusive mode
    void ToggleExclusive();

    //! Add a window to be managed
    //! \param window window to add
    void AddWindow(std::unique_ptr<WindowNode>&& window);

private:

    //
    // Render resources
    //

    render::Geometry_2D m_geometry2D;
    render::TextureBlit m_blit;
    detail::Image m_image;
    detail::FrameBuffer m_frameBuffer;
    nvrhi::CommandListHandle m_commandList;

    //
    // Gui resources
    //

    point4Di m_uiBounds     = {};
    bool m_exclusiveMode    = false;

    bool m_isDragging = false;
    bool m_isResizing = false;

    std::vector<std::unique_ptr<WindowNode>> m_windows;

};

}