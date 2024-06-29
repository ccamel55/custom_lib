#pragma once

#include <module_core/types/bitflag.hpp>
#include <module_core/types/point/point2D.hpp>

namespace lib::input
{
enum input_type : lib::bitflag_t
{
    INPUT_TYPE_NONE        = 0,
    INPUT_TYPE_KEYBOARD    = 1 << 0,
    INPUT_TYPE_MOUSE       = 1 << 1,
    INPUT_TYPE_OTHER       = 1 << 2,
};

enum button_state: lib::bitflag_t
{
    BUTTON_STATE_UP          = 0,
    BUTTON_STATE_DOWN        = 1 << 0,
    BUTTON_STATE_PRESSED     = 1 << 1,
    BUTTON_STATE_RELEASED    = 1 << 2,
};

// stolen from imgui, thanks imgui
enum class key : uint32_t
{
    NONE,
    TAB,
    LEFT_ARROW,
    RIGHT_ARROW,
    UP_ARROW,
    DOWN_ARROW,
    PAGE_UP,
    PAGE_DOWN,
    HOME,
    END,
    INSERT,
    DELETE_KEY,
    BACKSPACE,
    SPACE,
    ENTER,
    ESCAPE,
    LEFT_CTRL,  LEFT_SHIFT,     LEFT_ALT,   LEFT_SUPER,
    RIGHT_CTRL, RIGHT_SHIFT,    RIGHT_ALT,  RIGHT_SUPER,
    MENU,
    NUM_0, NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8, NUM_9,
    A, B, C, D, E, F, G, H, I, J,
    K, L, M, N, O, P, Q, R, S, T,
    U, V, W, X, Y, Z,
    F1, F2, F3, F4, F5, F6,
    F7, F8, F9, F10, F11, F12,
    APOSTROPHE,        // '
    COMMA,             // ,
    MINUS,             // -
    PERIOD,            // .
    SLASH,             // /
    SEMICOLON,         // ;
    EQUAL,             // =
    LEFT_BRACKET,       // [
    BACKSLASH,         // \ (THIS TEXT INHIBIT MULTILINE COMMENT CAUSED BY BACKSLASH)
    RIGHT_BRACKET,      // ]
    GRAVE_ACCENT,       // `
    CAPSLOCK,
    SCROLL_LOCK,
    NUM_LOCK,
    PRINT_SCREEN,
    PAUSE,
    KEYPAD0, KEYPAD1, KEYPAD2, KEYPAD3, KEYPAD4,
    KEYPAD5, KEYPAD6, KEYPAD7, KEYPAD8, KEYPAD9,
    KEYPAD_DECIMAL,
    KEYPAD_DIVIDE,
    KEYPAD_MULTIPLY,
    KEYPAD_SUBTRACT,
    KEYPAD_ADD,
    KEYPAD_ENTER,
    KEYPAD_EQUAL,

    // MOUSE BUTTONS
    MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE, MOUSE_MOVE, MOUSE_SCROLL,

    // MODIFIERS
    CTRL, SHIFT, ALT, SUPER, SHORTCUT,

    NUM_KEYS,
};

//! Interface that is passed to observer callback. Thank you CRTP!
template<typename Base>
class InputObserver_Base {
public:
    //! Get the current state of any key
    //! \param key key to query
    //! \return bitflag representing current key state
    [[nodiscard]] bitflag get_state(key key) const {
        return static_cast<const Base*>(this)->internal_get_state(key);
    }

    //! Get the last key input that was updated
    //! \return last key that was updated
    [[nodiscard]] key last_key() const {
        return static_cast<const Base*>(this)->internal_last_key();
    }

    //! Get cursor position
    //! \return cursor position
    [[nodiscard]] const point2Di& cursor_position() const {
        return static_cast<const Base*>(this)->internal_cursor_position();
    }

    //! Get cursor delta, only updated if cursor was moved
    //! \return cursor delta
    [[nodiscard]] const point2Di& cursor_delta() const {
        return static_cast<const Base*>(this)->internal_cursor_delta();
    }

    //! Get scroll delta, only updated if scroll was moved
    //! \return scroll delta
    [[nodiscard]] const point2Di& scroll_delta() const {
        return static_cast<const Base*>(this)->internal_scroll_delta();
    }
};
}