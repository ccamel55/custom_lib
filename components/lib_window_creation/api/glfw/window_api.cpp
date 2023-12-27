#include <lib_window_creation/window_api.hpp>

#include <cassert>

using namespace lib::window_creation;

namespace
{
// this is very annoying, there is nothing we can really do to get around this besides using an exposed variable.
window_api_base* this_ptr = nullptr;
}  // namespace

window_api::window_api(const window_parameters_t& window_parameters, std::function<void()> window_loop_callback)
	: window_api_base(std::move(window_loop_callback))
{
	// try init glfw
	if (!glfwInit())
	{
		lib_log_e("window_creation: failed to init glfw");
		assert(false);
	}

	if (window_parameters.flags.has(window_flags::window_flag_opengl3))
	{
		// Opengl version 3.3
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

		// OpenGl core profile
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}
	else
	{
		// Vulkan and other backends will not use OpenGL
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}

	// disable msaa
	glfwWindowHint(GLFW_SAMPLES, 0);

	// remove border and title bar
	glfwWindowHint(GLFW_DECORATED, !(window_parameters.flags.has(window_flags::window_flag_no_border)));

	// allow resizing
	glfwWindowHint(GLFW_RESIZABLE, !!(window_parameters.flags.has(window_flags::window_flag_resizeable)));

	// create a new window
	_glfw_window_ptr = glfwCreateWindow(
		window_parameters.width,
		window_parameters.height,
		window_parameters.window_name.data(),
		nullptr,
		nullptr
		);

	if (!_glfw_window_ptr)
	{
		glfwTerminate();

		lib_log_e("window_creation: failed to create glfw window");
		assert(false);
	}

	glfwSetWindowPos(_glfw_window_ptr, window_parameters.x_position, window_parameters.y_position);

	glfwMakeContextCurrent(_glfw_window_ptr);

	glfwSwapInterval(1);

	glfwSetInputMode(_glfw_window_ptr, GLFW_STICKY_KEYS, GLFW_TRUE);

	this_ptr = this;
}

window_api::~window_api()
{
	glfwTerminate();
	this_ptr = nullptr;
}


void window_api::window_loop() const
{
	lib_log_d("window_creation: starting window loop");

	// run main render thread from current thread
	while (glfwGetKey(_glfw_window_ptr, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(_glfw_window_ptr) == 0)
	{
		glfwPollEvents();

		// callback the parent which will do more stuff
		_window_loop_callback();

		glfwSwapBuffers(_glfw_window_ptr);
	}

	lib_log_d("window_creation: destroying window");
	glfwTerminate();
}

void window_api::close_window() const
{
	glfwSetWindowShouldClose(_glfw_window_ptr, true);
}

void window_api::focus_window() const
{
	glfwMakeContextCurrent(_glfw_window_ptr);
}

#ifndef DEF_LIB_RENDERING_off
bool window_api::register_renderer(std::shared_ptr<rendering::renderer>& renderer)
{
	lib::point2Di window_size = {};
	glfwGetWindowSize(_glfw_window_ptr, &window_size._x, &window_size._y);

	// initialize renderer using glfw stuff
	renderer->bind_api(nullptr);
	renderer->set_window_size(window_size);

	return true;
}
#endif

#ifndef DEF_LIB_INPUT_off
namespace
{
lib::input::key_button glfw_to_input_key(int key)
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

lib::input::key_button glfw_to_mouse_key(int button)
{
	switch (button)
    {
        case GLFW_MOUSE_BUTTON_LEFT: return lib::input::key_button::mouseleft;
		case GLFW_MOUSE_BUTTON_RIGHT: return lib::input::key_button::mouseright;
		case GLFW_MOUSE_BUTTON_MIDDLE: return lib::input::key_button::mousemiddle;
		default: return lib::input::key_button::none;
    }
}
}

bool window_api::register_input_handler(std::shared_ptr<input::input_handler>& input_handler)
{
	// register callback to glfw
	_input_handler = input_handler;

	glfwSetKeyCallback(_glfw_window_ptr, key_callback);
	glfwSetScrollCallback(_glfw_window_ptr, scroll_callback);
	glfwSetCursorPosCallback(_glfw_window_ptr, cursor_position_callback);
	glfwSetMouseButtonCallback(_glfw_window_ptr, mouse_button_callback);

	return true;
}

void window_api::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// only check for press and release
	if (action != GLFW_PRESS && action != GLFW_RELEASE)
	{
		return;
	}

	// _input_handler_ptr will always be valid here
	const lib::input::input_t input = {
		.type = input::keyboard,
		.key = glfw_to_input_key(key),
		.state = action == GLFW_PRESS,
	};

	this_ptr->add_input(input);
}

void window_api::scroll_callback(GLFWwindow* window, double offset_x, double offset_y)
{
	// _input_handler_ptr will always be valid here
	const lib::input::input_t input = {
		.type = input::mouse,
		.key = lib::input::key_button::mouse_scroll,
		.state = lib::point2Di{ static_cast<int>(offset_x), static_cast<int>(offset_y)}
	};

	this_ptr->add_input(input);
}

void window_api::cursor_position_callback(GLFWwindow* window, double pos_x, double pos_y)
{
	// _input_handler_ptr will always be valid here
	const lib::input::input_t input = {
		.type = input::mouse,
		.key = lib::input::key_button::mouse_move,
		.state = lib::point2Di{ static_cast<int>(pos_x), static_cast<int>(pos_y)}
	};

	this_ptr->add_input(input);
}

void window_api::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	// _input_handler_ptr will always be valid here
	const lib::input::input_t input = {
		.type = input::mouse,
		.key = glfw_to_mouse_key(button),
		.state = action == GLFW_PRESS,
	};

	this_ptr->add_input(input);
}
#endif