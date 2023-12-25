#pragma once

#include <cstdint>
#include <functional>

#include <core_sdk/types/point/point2D.hpp>

namespace lib::input
{
enum input_type: uint32_t
{
    none = 0 << 0,
    keyboard = 1 << 0,
    mouse = 1 << 1,
};

enum input_state: uint32_t
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

class input_handler;

//! Everything that wants to recieve an input must use a input receiver
//! The input handler will dispatch events to all receivers when it recives appropriate updates
class input_receiver
{
    friend class input_handler;
public:
    explicit input_receiver(input_type type);
    ~input_receiver() = default;

    //! register a function to be called when an input is updated, everything that checks for inputs should
    //! be called within this function
    void register_input_callback(std::function<void()> callback);

    [[nodiscard]] key_button get_last_key() const;
    [[nodiscard]] input_state get_key_state(key_button key) const;

    [[nodiscard]] const point2Df& get_cursor_position() const;
    [[nodiscard]] const point2Df& get_cursor_delta() const;
    [[nodiscard]] const point2Df& get_scroll_delta() const;

private:
    void emit_update(input_type type) const;
    void register_handler(input_handler* handler);

private:
    // this is bad practice i know.
    input_handler* _input_handler;

    input_type _type;
    std::function<void()> _input_callback;
};
}