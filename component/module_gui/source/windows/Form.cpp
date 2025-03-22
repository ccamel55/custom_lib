#include <glm/gtx/easing.hpp>
#include <module_gui/windows/Form.hpp>

using namespace lib::gui;

Form::Form(
    std::string title,
    const lib::point2Di& size,
    const lib::point2Di& pos
)
    : m_title(std::move(title)) {

    point4Di& area = GetNodeProperties().area;

    area.x = pos.x;
    area.y = pos.y;

    area.z = size.x;
    area.w = size.y;
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

    const point4Di& area    = GetNodeProperties().area;
    const bool focused      = WindowState().has(WindowState_Focused | WindowState_Pinned);

    const point4Di dragArea = GetDragArea().value();
    const point4Di resizeArea = GetResizeArea().value();

    geometry_2d.d_box_fill(area, color(50, 50, 50));

    geometry_2d.d_box_fill(dragArea, color(100, 100, 100));
    geometry_2d.d_triangle_filled(
        point2Df(resizeArea.x + resizeArea.z, resizeArea.y),
        point2Df(resizeArea.x + resizeArea.z, resizeArea.y + resizeArea.w),
        point2Df(resizeArea.x, resizeArea.y + resizeArea.w),
        color(40, 40, 40)
    );

    geometry_2d.d_box(area, focused ? color(150, 150, 150) : color(30, 30, 30) );

    //
    // Render contents
    //

    if (m_content == nullptr) {
        return;
    }

    m_content->OnRender(geometry_2d);
}

std::optional<lib::point4Di> Form::GetDragArea() const {
    const point4Di& area = GetNodeProperties().area;
    return std::make_optional<point4Di>(area.x, area.y, area.z, defaults::DRAG_HEIGHT);
}

std::optional<lib::point4Di> Form::GetResizeArea() const {
    const point4Di& area = GetNodeProperties().area;
    const point2Di resizeSize(defaults::RESIZE_SIZE, defaults::RESIZE_SIZE);

    return std::make_optional<point4Di>(
        area.x + area.z - resizeSize.x,
        area.y + area.w - resizeSize.y,
        resizeSize.x,
        resizeSize.y
    );
}

void Form::SetContent(std::unique_ptr<UiNode>&& content) {
    m_content = std::move(content);
}



