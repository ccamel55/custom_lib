#include <module_gui/Gui.hpp>

#include <ranges>

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
    const std::shared_ptr<render::FontFactory>& fontFactory,
    const std::shared_ptr<render::ShaderFactory>& shaderFactory,
    const std::shared_ptr<render::TextureFactory>& textureFactory
)   : RenderPass(device)
    , m_geometry2D(
        _device,
        fontFactory,
        shaderFactory,
        textureFactory
    )
    , m_blit(device, shaderFactory)
    , m_image(device)
    , m_frameBuffer(device)
    , m_commandList(_device->createCommandList()) {

}

void Gui::update_input(const bitflag input_type, const input::InputObserver& input) {

    using namespace lib::input;

    //
    // TODO: handle properly
    //

    if (m_windows.empty()) {
        return;
    }

    // First index will always be our "focused" window
    const auto& focusedWindow = m_windows.at(0);
    const auto& [focusSize, focusPos] = focusedWindow->GetNodeProperties();

    if (focusedWindow->WindowState().has(WindowState_BlockInput) ||
        input.in_rect(focusPos, focusSize)) {

        //
        // Mouse is in focus window or focus window is taking control
        //

        focusedWindow->OnInput(input_type, input);
    }
    else if (input.get_state(key::MOUSE_LEFT).has(BUTTON_STATE_PRESSED)) {

        //
        // Check if we are over any other window, then bring to focus if so
        //

        for (auto it = m_windows.begin() + 1; it != m_windows.end(); ++it) {

            const auto& window = *it;
            const auto& [windowSize, windowPos] = window->GetNodeProperties();

            if (input.in_rect(windowPos, windowSize)) {

                // move to front (index 0 AKA focused)
                std::rotate(m_windows.begin(), it, it + 1);

                break;
            }
        }
    }

    // if (m_exclusiveMode) {
    //     // TODO: handle window resizing and snapping etc.
    //     //
    //     // NEED TO IMPLEMENT
    //     // NOTE: first element in windows list should be the window in focus
    //     //
    // }
    // else {
    //
    //     // For now, we only pass keyboard inputs to nonexclusive windows. We may want to change this in the future
    //     if (input_type.has(lib::input::INPUT_TYPE_KEYBOARD)) {
    //
    //         // Update input for all windows in non-exclusive or pinned mode
    //         for (const std::unique_ptr<WindowNode>& window: m_windows) {
    //
    //             const window_properties_t& windowProperties = window->GetWindowProperties();
    //
    //             // Skip windows that don't allow for inputting or need exclusive focus to input
    //             if (!windowProperties.allowInput || windowProperties.exclusiveInput) {
    //                 return;
    //             }
    //
    //             window->OnInput(input_type, input);
    //         }
    //     }
    // }
}

void Gui::update_frame(const render::FrameInterval& interval) {

    // Note: For now we will Animate and Render on the same call.
    //       Future work should be done to allow animating and rendering separately.

    //
    // TODO: handle properly, I am just drawing for the sake of it right now
    //

    for (const std::unique_ptr<WindowNode>& window: m_windows | std::views::reverse) {
        window->OnAnimate(interval);
        window->OnRender(m_geometry2D);
    }
}

void Gui::render(nvrhi::IFramebuffer* frame_buffer) {

    using namespace detail;

    m_commandList->open();
    {
        //
        // Write queued up draws to image
        //

        const auto geometry_fb = m_frameBuffer[FrameBuffer_Id::Geometry_2d];
        nvrhi::utils::ClearColorAttachment(m_commandList, geometry_fb, 0, nvrhi::Color(0));

        m_geometry2D.draw_geometry(m_commandList, geometry_fb);
    }
    // ------------------------------------------------------------------------
    {
        //
        // Copy draw target image to current frame buffer image
        //

        m_blit.blit(m_commandList, m_image[Image_Id::Geometry_2d_ColorTarget], frame_buffer);
    }
    m_commandList->close();

    _device->executeCommandList(m_commandList);
}

void Gui::back_buffer_resizing() {
    m_geometry2D.back_buffer_resizing();
    m_blit.back_buffer_resizing();
}

void Gui::back_buffer_resized(const point2Di& size) {

    using namespace detail;

    m_image.back_buffer_resized([&](auto& image) {

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

    m_frameBuffer.back_buffer_resized([&](auto& frame_buffer) {

        //
        // Recreate frame buffers
        //

        frame_buffer[static_cast<size_t>(FrameBuffer_Id::Geometry_2d)] = _device->createFramebuffer(
            nvrhi::FramebufferDesc()
                .addColorAttachment(m_image[Image_Id::Geometry_2d_ColorTarget])
        );
    });
}

bool Gui::IsExclusive() const {
    return m_exclusiveMode;
}

void Gui::ToggleExclusive() {
    m_exclusiveMode = !m_exclusiveMode;
}

void Gui::AddWindow(std::unique_ptr<WindowNode>&& window) {
    m_windows.emplace_back(std::move(window));
}






