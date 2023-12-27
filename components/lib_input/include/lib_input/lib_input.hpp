#pragma once

#include <array>
#include <functional>
#include <variant>

#include <core_sdk/types/bitflag.hpp>
#include <core_sdk/types/point/point2D.hpp>

namespace lib::input
{
// predeclaration is usually bad
class input_handler;

enum input_type: bitflag_t
{
    none = 0 << 0,
    keyboard = 1 << 0,
    mouse = 1 << 1,
};

enum input_state: bitflag_t
{
    up = 0 << 0,
    down = 1 << 0,
    pressed = 1 << 1,
    released = 1 << 2,
};

// tolen from imgui, thanks imgui
enum class key_button: uint8_t
{
    none,
    tab,
    leftarrow,
    rightarrow,
    uparrow,
    downarrow,
    pageup,
    pagedown,
    home,
    end,
    insert,
    delete_key,
    backspace,
    space,
    enter,
    escape,
    leftctrl, leftshift, leftalt, leftsuper,
    rightctrl, rightshift, rightalt, rightsuper,
    menu,
    num_0, num_1, num_2, num_3, num_4, num_5, num_6, num_7, num_8, num_9,
    a, b, c, d, e, f, g, h, i, j,
    k, l, m, n, o, p, q, r, s, t,
    u, v, w, x, y, z,
    f1, f2, f3, f4, f5, f6,
    f7, f8, f9, f10, f11, f12,
    apostrophe,        // '
    comma,             // ,
    minus,             // -
    period,            // .
    slash,             // /
    semicolon,         // ;
    equal,             // =
    leftbracket,       // [
    backslash,         // \ (this text inhibit multiline comment caused by backslash)
    rightbracket,      // ]
    graveaccent,       // `
    capslock,
    scrolllock,
    numlock,
    printscreen,
    pause,
    keypad0, keypad1, keypad2, keypad3, keypad4,
    keypad5, keypad6, keypad7, keypad8, keypad9,
    keypaddecimal,
    keypaddivide,
    keypadmultiply,
    keypadsubtract,
    keypadadd,
    keypadenter,
    keypadequal,

    // mouse buttons
    mouseleft, mouseright, mousemiddle, mouse_move, mouse_scroll,

    // modifiers
    ctrl, shift, alt, super, shortcut,

    num_keys,
};

struct input_t
{
    bitflag type = input_type::none;
    key_button key = key_button::none;

    std::variant<bool, lib::point2Di> state = {};
};

struct input_callback_t
{
    // what type of input to listen to
    input_type type = input_type::none;

    // something that will be called when the input is updated
    std::function<void(const input_handler&)> callback = nullptr;
};

//! Input handler class
class input_handler
{
public:
    //! Add an input to our handler
    void add_input(const input_t& input);

    //! Register a callback to be called when an input is updated
    void register_callback(uint32_t type, std::function<void(const input_handler&)>&& callback);

    //! Get the current state of any key_button key.
    [[nodiscard]] const bitflag& get_key_state(key_button key) const;

    //! Get the last key_button input that was received.
    [[nodiscard]] key_button get_last_key() const;

    //! Get the current position of our cursor
    [[nodiscard]] const point2Di& get_cursor_position() const;

    //! Get the current cursor delta.
    [[nodiscard]] const point2Di& get_cursor_delta() const;

    //! Get the current scroll delta.
    [[nodiscard]] const point2Di& get_scroll_delta() const;

private:
    point2Di _cursor_position = {};
    point2Di _cursor_delta = {};
    point2Di _scroll_delta = {};

    // save input state for all keys
    key_button _last_key = key_button::none;
    std::array<bitflag, static_cast<size_t>(key_button::num_keys)> _key_state = {};

    std::vector<input_callback_t> _input_callbacks = {};

};
}