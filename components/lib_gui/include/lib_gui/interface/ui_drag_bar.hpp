#pragma once

#include <lib_gui/interface/ui_base_window.hpp>

namespace lib::gui
{
namespace drag_bar
{
    const static auto size = lib::point2Di{150, 20};

    const static auto pin_offset = lib::point2Di(10, size.y / 2);
    const static auto pin_size = lib::point2Di(6, 6);

    constexpr auto color_fill = lib::color(64, 64, 64);
    constexpr auto color_fill_gradient = lib::color(90, 90, 90);
}

class ui_drag_bar: public ui_base_window
{
public:
    explicit ui_drag_bar(const std::string& name);
    ~ui_drag_bar() override;

    void draw(lib::rendering::render_callback_handler& render) override;
    void input(const lib::input::input_callback_handler& input) override;

private:

};
}