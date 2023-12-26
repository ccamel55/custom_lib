#pragma once

#include <lib_rendering/lib_rendering.hpp>
#include <lib_input/lib_input.hpp>

namespace lib::gui
{
enum ui_flag: uint32_t
{
    flag_none = 0 << 0,
    flag_pinned = 1 << 0,
    flag_blocked = 1 << 1,
    flag_input_only = 1 << 2,
    flag_active = 1 << 3,
    flag_drawgroup_active = 1 << 4
};

enum ui_type: uint32_t
{
    type_none = 0 << 0,
    type_form = 1 << 0, // represents a window
    type_group = 1 << 1, // represents something that has other elements
    type_control = 1 << 2, // something that can be interacted with
    type_static_item = 1 << 3, // something that is drawn only
};

namespace font
{
    inline lib::rendering::font_id title = 0;
    inline lib::rendering::font_id heading = 0;
    inline lib::rendering::font_id text = 0;
}

namespace key
{
    constexpr auto select = lib::input::key_button::delete_key;

    constexpr auto up = lib::input::key_button::uparrow;
    constexpr auto down = lib::input::key_button::downarrow;
    constexpr auto left = lib::input::key_button::leftarrow;
    constexpr auto right = lib::input::key_button::rightarrow;
}

namespace color
{
    constexpr auto white = lib::color(255, 255, 255);
    constexpr auto blue = lib::color(45, 125, 215);
    constexpr auto light_blue = lib::color(95, 160, 250);
    constexpr auto green = lib::color(20, 195, 55);
    constexpr auto red = lib::color(215, 40, 25);
    constexpr auto yellow = lib::color(245, 245, 60);
    constexpr auto gray = lib::color(90, 90, 90);
    constexpr auto light_gray = lib::color(100, 100, 100);
    constexpr auto black = lib::color(0, 0, 0);
}
}