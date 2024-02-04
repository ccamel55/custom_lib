#include <lib_window_creation/helpers.hpp>

using namespace lib::window_creation;

#ifndef DEF_LIB_INPUT_off
lib::input::key_button helpers::glfw_to_input_key(int key)
{
    switch (key)
    {
        case GLFW_KEY_TAB: return lib::input::key_button::tab;
        case GLFW_KEY_LEFT: return lib::input::key_button::leftarrow;
        case GLFW_KEY_RIGHT: return lib::input::key_button::rightalt;
        case GLFW_KEY_UP: return lib::input::key_button::uparrow;
        case GLFW_KEY_DOWN: return lib::input::key_button::downarrow;
        case GLFW_KEY_PAGE_UP: return lib::input::key_button::pageup;
        case GLFW_KEY_PAGE_DOWN: return lib::input::key_button::pagedown;
        case GLFW_KEY_HOME: return lib::input::key_button::home;
        case GLFW_KEY_END: return lib::input::key_button::end;
        case GLFW_KEY_INSERT: return lib::input::key_button::insert;
        case GLFW_KEY_DELETE: return lib::input::key_button::delete_key;
        case GLFW_KEY_BACKSPACE: return lib::input::key_button::backspace;
        case GLFW_KEY_SPACE: return lib::input::key_button::space;
        case GLFW_KEY_ENTER: return lib::input::key_button::enter;
        case GLFW_KEY_ESCAPE: return lib::input::key_button::escape;
        case GLFW_KEY_APOSTROPHE: return lib::input::key_button::apostrophe;
        case GLFW_KEY_COMMA: return lib::input::key_button::comma;
        case GLFW_KEY_MINUS: return lib::input::key_button::minus;
        case GLFW_KEY_PERIOD: return lib::input::key_button::period;
        case GLFW_KEY_SLASH: return lib::input::key_button::slash;
        case GLFW_KEY_SEMICOLON: return lib::input::key_button::semicolon;
        case GLFW_KEY_EQUAL: return lib::input::key_button::equal;
        case GLFW_KEY_LEFT_BRACKET: return lib::input::key_button::leftbracket;
        case GLFW_KEY_BACKSLASH: return lib::input::key_button::backslash;
        case GLFW_KEY_RIGHT_BRACKET: return lib::input::key_button::rightbracket;
        case GLFW_KEY_GRAVE_ACCENT: return lib::input::key_button::graveaccent;
        case GLFW_KEY_CAPS_LOCK: return lib::input::key_button::capslock;
        case GLFW_KEY_SCROLL_LOCK: return lib::input::key_button::scrolllock;
        case GLFW_KEY_NUM_LOCK: return lib::input::key_button::numlock;
        case GLFW_KEY_PRINT_SCREEN: return lib::input::key_button::printscreen;
        case GLFW_KEY_PAUSE: return lib::input::key_button::pause;
        case GLFW_KEY_KP_0: return lib::input::key_button::keypad0;
        case GLFW_KEY_KP_1: return lib::input::key_button::keypad1;
        case GLFW_KEY_KP_2: return lib::input::key_button::keypad2;
        case GLFW_KEY_KP_3: return lib::input::key_button::keypad3;
        case GLFW_KEY_KP_4: return lib::input::key_button::keypad4;
        case GLFW_KEY_KP_5: return lib::input::key_button::keypad5;
        case GLFW_KEY_KP_6: return lib::input::key_button::keypad6;
        case GLFW_KEY_KP_7: return lib::input::key_button::keypad7;
        case GLFW_KEY_KP_8: return lib::input::key_button::keypad8;
        case GLFW_KEY_KP_9: return lib::input::key_button::keypad9;
        case GLFW_KEY_KP_DECIMAL: return lib::input::key_button::keypaddecimal;
        case GLFW_KEY_KP_DIVIDE: return lib::input::key_button::keypaddivide;
        case GLFW_KEY_KP_MULTIPLY: return lib::input::key_button::keypadmultiply;
        case GLFW_KEY_KP_SUBTRACT: return lib::input::key_button::keypadsubtract;
        case GLFW_KEY_KP_ADD: return lib::input::key_button::keypadadd;
        case GLFW_KEY_KP_ENTER: return lib::input::key_button::keypadenter;
        case GLFW_KEY_KP_EQUAL: return lib::input::key_button::keypadequal;
        case GLFW_KEY_LEFT_SHIFT: return lib::input::key_button::leftshift;
        case GLFW_KEY_LEFT_CONTROL: return lib::input::key_button::leftctrl;
        case GLFW_KEY_LEFT_ALT: return lib::input::key_button::leftalt;
        case GLFW_KEY_LEFT_SUPER: return lib::input::key_button::leftsuper;
        case GLFW_KEY_RIGHT_SHIFT: return lib::input::key_button::rightshift;
        case GLFW_KEY_RIGHT_CONTROL: return lib::input::key_button::rightctrl;
        case GLFW_KEY_RIGHT_ALT: return lib::input::key_button::rightalt;
        case GLFW_KEY_RIGHT_SUPER: return lib::input::key_button::rightsuper;
        case GLFW_KEY_MENU: return lib::input::key_button::menu;
        case GLFW_KEY_0: return lib::input::key_button::num_0;
        case GLFW_KEY_1: return lib::input::key_button::num_1;
        case GLFW_KEY_2: return lib::input::key_button::num_2;
        case GLFW_KEY_3: return lib::input::key_button::num_3;
        case GLFW_KEY_4: return lib::input::key_button::num_4;
        case GLFW_KEY_5: return lib::input::key_button::num_5;
        case GLFW_KEY_6: return lib::input::key_button::num_6;
        case GLFW_KEY_7: return lib::input::key_button::num_7;
        case GLFW_KEY_8: return lib::input::key_button::num_8;
        case GLFW_KEY_9: return lib::input::key_button::num_9;
        case GLFW_KEY_A: return lib::input::key_button::a;
        case GLFW_KEY_B: return lib::input::key_button::b;
        case GLFW_KEY_C: return lib::input::key_button::c;
        case GLFW_KEY_D: return lib::input::key_button::d;
        case GLFW_KEY_E: return lib::input::key_button::e;
        case GLFW_KEY_F: return lib::input::key_button::f;
        case GLFW_KEY_G: return lib::input::key_button::g;
        case GLFW_KEY_H: return lib::input::key_button::h;
        case GLFW_KEY_I: return lib::input::key_button::i;
        case GLFW_KEY_J: return lib::input::key_button::j;
        case GLFW_KEY_K: return lib::input::key_button::k;
        case GLFW_KEY_L: return lib::input::key_button::l;
        case GLFW_KEY_M: return lib::input::key_button::m;
        case GLFW_KEY_N: return lib::input::key_button::n;
        case GLFW_KEY_O: return lib::input::key_button::o;
        case GLFW_KEY_P: return lib::input::key_button::p;
        case GLFW_KEY_Q: return lib::input::key_button::q;
        case GLFW_KEY_R: return lib::input::key_button::r;
        case GLFW_KEY_S: return lib::input::key_button::s;
        case GLFW_KEY_T: return lib::input::key_button::t;
        case GLFW_KEY_U: return lib::input::key_button::u;
        case GLFW_KEY_V: return lib::input::key_button::v;
        case GLFW_KEY_W: return lib::input::key_button::w;
        case GLFW_KEY_X: return lib::input::key_button::x;
        case GLFW_KEY_Y: return lib::input::key_button::y;
        case GLFW_KEY_Z: return lib::input::key_button::z;
        case GLFW_KEY_F1: return lib::input::key_button::f1;
        case GLFW_KEY_F2: return lib::input::key_button::f2;
        case GLFW_KEY_F3: return lib::input::key_button::f3;
        case GLFW_KEY_F4: return lib::input::key_button::f4;
        case GLFW_KEY_F5: return lib::input::key_button::f5;
        case GLFW_KEY_F6: return lib::input::key_button::f6;
        case GLFW_KEY_F7: return lib::input::key_button::f7;
        case GLFW_KEY_F8: return lib::input::key_button::f8;
        case GLFW_KEY_F9: return lib::input::key_button::f9;
        case GLFW_KEY_F10: return lib::input::key_button::f10;
        case GLFW_KEY_F11: return lib::input::key_button::f11;
        case GLFW_KEY_F12: return lib::input::key_button::f12;
		default: return lib::input::key_button::none;
    }
}

lib::input::key_button helpers::glfw_to_mouse_key(int button)
{
	switch (button)
    {
        case GLFW_MOUSE_BUTTON_LEFT: return lib::input::key_button::mouseleft;
		case GLFW_MOUSE_BUTTON_RIGHT: return lib::input::key_button::mouseright;
		case GLFW_MOUSE_BUTTON_MIDDLE: return lib::input::key_button::mousemiddle;
		default: return lib::input::key_button::none;
    }
}
#endif