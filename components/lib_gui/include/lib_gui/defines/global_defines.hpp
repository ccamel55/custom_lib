#pragma once

#include <lib_gui/utils.hpp>
#include <core_sdk/types/bitflag.hpp>

#include <lib_rendering/lib_rendering.hpp>
#include <lib_input/lib_input.hpp>

namespace lib::gui
{
enum ui_flag: bitflag_t
{
    flag_none = 0 << 0,
    flag_visible = 1 << 0,
    flag_active = 1 << 1,
};

enum ui_update: bitflag_t
{
    update_none = 0 << 0,
    update_visibility = 1 << 0,
    update_active = 1 << 1,
    update_position = 1 << 2,
    update_size = 1 << 3,
};

namespace context
{
    inline lib::point2Di screen_size = {};
}

namespace font
{
    inline lib::rendering::font_id title = 0;
    inline lib::rendering::font_id heading = 0;
    inline lib::rendering::font_id text = 0;
}

namespace key
{
    constexpr auto toggle_visibility = lib::input::key_button::insert;
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