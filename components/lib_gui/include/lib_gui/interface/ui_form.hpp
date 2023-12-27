#pragma once

#include <lib_gui/interface/ui_base_window.hpp>

namespace lib::gui
{
namespace form
{
constexpr auto border_size = 7;
constexpr auto title_size_y = 20;

constexpr auto pin_offset = lib::point2Di(10, title_size_y / 2);
constexpr auto pin_size = lib::point2Di(6, 6);

constexpr auto contents_start_offset = lib::point2Di(border_size, title_size_y);
constexpr auto contents_end_offset = lib::point2Di(border_size * 2, title_size_y + border_size);

constexpr auto color_fill = lib::color(64, 64, 64);
constexpr auto color_fill_gradient = lib::color(90, 90, 90);

constexpr auto color_contents = lib::color(190, 190, 190);
constexpr auto color_contents_alt = lib::color(70, 70, 70, 120);
}

class ui_form: public ui_base_window
{
public:
    ui_form(const std::string& name, const lib::point2Di& size);
    ~ui_form() override;

    void draw(lib::rendering::renderer& render) override;
    void input(const lib::input::input_handler& input) override;

private:

};
}