#include <module_gui/Gui.hpp>

using namespace lib::gui;

// Global styler context
std::shared_ptr<StyleProvider> lib::gui::STYLER = nullptr;

void lib::gui::SetStyler(const std::shared_ptr<StyleProvider>& styler) {
    STYLER = styler;
}

//
// --------------------------------------------------------------------------------------------------------------
//

Gui::Gui(
    const nvrhi::DeviceHandle& device,
    const std::shared_ptr<render::FontFactory>& font_factory,
    const std::shared_ptr<render::ShaderFactory>& shader_factory,
    const std::shared_ptr<render::TextureFactory>& texture_factory
)   : RenderPass(device)
    , _geometry_2d(
        _device,
        font_factory,
        shader_factory,
        texture_factory
    )
    , _blit(device, shader_factory)
    , _image(device)
    , _frame_buffer(device)
    , _command_list(_device->createCommandList()) {

}

void Gui::update_input(const bitflag type, const input::InputObserver& input) {
    std::ignore = type;
    std::ignore = input;
}

void Gui::update_frame(const render::FrameInterval& interval) {

    // Note: For now we will Animate and Render on the same call.
    //       Future work should be done to allow animating and rendering separately.

    std::ignore = interval;
}

void Gui::render(nvrhi::IFramebuffer* frame_buffer) {

    using namespace detail;

    _command_list->open();
    {
        //
        // Write queued up draws to image
        //

        const auto geometry_fb = _frame_buffer[FrameBuffer_Id::Geometry_2d];
        nvrhi::utils::ClearColorAttachment(_command_list, geometry_fb, 0, nvrhi::Color(0));

        _geometry_2d.draw_geometry(_command_list, geometry_fb);
    }
    // ------------------------------------------------------------------------
    {
        //
        // Copy draw target image to current frame buffer image
        //

        _blit.blit(_command_list, _image[Image_Id::Geometry_2d_ColorTarget], frame_buffer);
    }
    _command_list->close();

    _device->executeCommandList(_command_list);
}

void Gui::back_buffer_resizing() {
    _geometry_2d.back_buffer_resizing();
    _blit.back_buffer_resizing();
}

void Gui::back_buffer_resized(const point2Di& size) {

    using namespace detail;

    _image.back_buffer_resized([&](auto& image) {

        //
        // Recreate textures
        //

        image[static_cast<size_t>(Image_Id::Geometry_2d_ColorTarget)] = _device->createTexture(
            nvrhi::TextureDesc()
                .setDebugName("ColorTarget")
                .setFormat(nvrhi::Format::SBGRA8_UNORM)
                .setWidth(std::max(size.x, 1))
                .setHeight(std::max(size.y, 1))
                .setIsRenderTarget(true)
                .setKeepInitialState(true)
                .setInitialState(nvrhi::ResourceStates::RenderTarget)
        );
    });

    _frame_buffer.back_buffer_resized([&](auto& frame_buffer) {

        //
        // Recreate frame buffers
        //

        frame_buffer[static_cast<size_t>(FrameBuffer_Id::Geometry_2d)] = _device->createFramebuffer(
            nvrhi::FramebufferDesc()
                .addColorAttachment(_image[Image_Id::Geometry_2d_ColorTarget])
        );
    });
}







