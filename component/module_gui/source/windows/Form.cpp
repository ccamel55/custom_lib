#include <module_gui/windows/Form.hpp>

using namespace lib::gui;

namespace {

constexpr window_properties_t PROPERTIES = {
    true,
    true,
    true,
    true
};

}

Form::Form(
    std::string title,
    const lib::point2Di& size,
    const lib::point2Di& pos
)
    : WindowNode(PROPERTIES)
    , m_title(std::move(title)) {

    auto& [curSize, curPos] = GetNodeProperties();

    curSize = size;
    curPos  = pos;
}

void Form::OnAnimate(const render::FrameInterval& interval) {

    if (m_content == nullptr) {
        return;
    }

    m_content->OnAnimate(interval);
}

void Form::OnInput(const bitflag input_type, const input::InputObserver& input) {

    using namespace lib::input;

    //
    // Update form
    //

    auto& [curSize, curPos] = GetNodeProperties();

    if (input.get_state(key::MOUSE_LEFT).has(BUTTON_STATE_PRESSED)) {

        // We will assume bottom right corner is resizeable area.
        // TODO: allow resizing from bottom, right and corner
        const lib::point2Di resizeSize  = point2Di(10, 10);
        const lib::point2Di resizePos   = curPos + curSize - resizeSize;

        if (input.in_rect(resizePos, resizeSize)) {
            m_resizing = true;
            return;
        }

        if (input.in_rect(curPos, curSize)) {
            m_dragging = true;
            return;
        }
    }
    else if (m_resizing || m_dragging) {

        if (input.get_state(key::MOUSE_LEFT).has(BUTTON_STATE_DOWN)) {

            //
            // TODO: check for min size and check for max position on screen
            //

            const point2Di& mouseDelta = input.cursor_delta();

            if (m_dragging) {
                curPos = {
                    curPos.x + mouseDelta.x,
                    curPos.y + mouseDelta.y
                };
            }
            else { // m_resizing
                curSize = {
                    curSize.x + mouseDelta.x,
                    curSize.y + mouseDelta.y
                };
            }

            // Take exclusive input control
            WindowState().add(WindowState_BlockInput);

            // Update contents size
            OnRefresh();
        }
        else {
            // Pass control back to GUI manager
            WindowState().remove(WindowState_BlockInput);

            m_resizing = false;
            m_dragging = false;
        }

        return;
    }

    //
    // Update content
    //

    if (m_content == nullptr) {
        return;
    }

    m_content->OnInput(input_type, input);
}

void Form::OnRefresh() {

    if (m_content == nullptr) {
        return;
    }

    m_content->OnRefresh();
}

void Form::OnRender(render::Geometry_2D& geometry_2d) {

    //
    // Draw form
    //

    const auto& [curSize, curPos] = GetNodeProperties();

    geometry_2d.d_box_fill(curPos, curSize, color(50, 50, 50));
    geometry_2d.d_box(curPos, curSize, color(255, 255, 255));

    //
    // Render contents
    //

    if (m_content == nullptr) {
        return;
    }

    m_content->OnRender(geometry_2d);
}

void Form::SetContent(std::unique_ptr<UiNode>&& content) {
    m_content = std::move(content);
}



