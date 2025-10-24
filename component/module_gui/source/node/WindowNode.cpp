#include <module_gui/node/WindowNode.hpp>

using namespace lib::gui;

namespace {

    // TODO: make this configurable
    const lib::point2Di MIN_WINDOW_SIZE = { 100, 50 };

}

void WindowNode::SetWindowPosChecked(const point2Di& pos, const point4Di& bounds) {

    point4Di& area = GetNodeProperties().area;

    area.x = std::clamp<int>(pos.x, bounds.x, bounds.z - area.z);
    area.y = std::clamp<int>(pos.y, bounds.y, bounds.w - area.w);

    OnRefresh(m_styler);
}

void WindowNode::SetWindowSizeChecked(const point2Di& size, const point4Di& bounds) {

    point4Di& area = GetNodeProperties().area;

    area.z = std::clamp<int>(size.x, MIN_WINDOW_SIZE.x, bounds.z - area.x);
    area.w = std::clamp<int>(size.y, MIN_WINDOW_SIZE.y, bounds.w - area.y);

    OnRefresh(m_styler);
}

std::optional<lib::point4Di> WindowNode::GetDragArea() const {
    return std::nullopt;
}

std::optional<lib::point4Di> WindowNode::GetResizeArea() const {
    return std::nullopt;
}

lib::bitflag& WindowNode::WindowState() {
    return m_windowState;
}
