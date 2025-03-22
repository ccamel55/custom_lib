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

    if (m_windows.empty()) {
        return;
    }

    // First index will always be our "focused" window
    const auto& focusedWindow = m_windows.at(0);
    const point4Di& focusedArea = focusedWindow->GetNodeProperties().area;

    if (m_isDragging || m_isResizing) {

        //
        // Handle drag or resize
        //

        if (input.get_state(key::MOUSE_LEFT).has(BUTTON_STATE_DOWN)) {

            const point2Di& mouseDelta = input.cursor_delta();

            if (m_isDragging) {
                const point2Di newPos(focusedArea.x + mouseDelta.x, focusedArea.y + mouseDelta.y);
                focusedWindow->SetWindowPosChecked(newPos, m_uiBounds);
            }
            else { // m_isResizing
                const point2Di newSize(focusedArea.z + mouseDelta.x, focusedArea.w + mouseDelta.y);
                focusedWindow->SetWindowSizeChecked(newSize, m_uiBounds);
            }
        }
        else {
            m_isResizing = false;
            m_isDragging = false;
        }
    }
    else if (input.in_rect(focusedArea)) {

        //
        // Mouse is in focus window
        //

        const auto resizeArea = focusedWindow->GetResizeArea();
        const auto dragArea = focusedWindow->GetDragArea();

        if (resizeArea.has_value() && input.in_rect(resizeArea.value())) {
            m_isResizing = true;
            return;
        }

        if (dragArea.has_value() && input.in_rect(dragArea.value())) {
            m_isDragging = true;
            return;
        }

        focusedWindow->OnInput(input_type, input);
    }
    else if (input.get_state(key::MOUSE_LEFT).has(BUTTON_STATE_PRESSED)) {

        //
        // Check if we are over any other window, then bring to focus if so
        //

        for (auto it = m_windows.begin() + 1; it != m_windows.end(); ++it) {

            const auto& window = *it;
            const point4Di& windowArea = window->GetNodeProperties().area;

            if (input.in_rect(windowArea)) {

                // reset focus of current "focused" window
                m_windows.at(0)->WindowState().remove(WindowState_Focused);

                // move to front (index 0 AKA focused)
                std::rotate(m_windows.begin(), it, it + 1);

                // add focus to new "focused" window
                m_windows.at(0)->WindowState().add(WindowState_Focused);

                break;
            }
        }
    }
}

void Gui::update_frame(const render::FrameInterval& interval) {

    // Note: For now we will Animate and Render on the same call.
    //       Future work should be done to allow animating and rendering separately.

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

    m_uiBounds = { 0, 0, size.x, size.y };
}

bool Gui::IsExclusive() const {
    return m_exclusiveMode;
}

void Gui::ToggleExclusive() {
    m_exclusiveMode = !m_exclusiveMode;
}

void Gui::AddWindow(std::unique_ptr<WindowNode>&& window) {
    m_windows.emplace_back(std::move(window));

    // First window - set as "focused"
    if (m_windows.size() == 1) {
        m_windows.at(0)->WindowState().add(WindowState_Focused);
    }
}






