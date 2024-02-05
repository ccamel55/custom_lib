#include <lib_window_creation/lib_window_creation.hpp>
#include <cassert>

using namespace lib::window_creation;

window_creation::window_creation(const window_parameters_t& window_parameters)
{
	// if using renderer, we will only call renderer draw_frame, otherwise we can use what ever callback we want
	_render_callback = [&]()
	{
#ifndef DEF_LIB_RENDERING_off
		if (_renderer)
		{
			_renderer->draw_frame();
		}
#endif
	};

	// try init glfw
	if (!glfwInit())
	{
		lib_log_e("window_creation: failed to init glfw");
		assert(false);
	}

#ifdef DEF_LIB_RENDERING_gl3
	// Opengl version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

	// OpenGl core profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
	// Vulkan and other backends will not use OpenGL
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif

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

	glfwSwapInterval(0);

	glfwSetWindowUserPointer(_glfw_window_ptr, this);
	glfwSetWindowSizeCallback(_glfw_window_ptr, window_size_callback);
}

window_creation::~window_creation()
{
	_renderer.reset();

#ifdef DEF_LIB_RENDERING_vulkan
	helpers::destroy_vulkan_instance(*_api_data);
#endif

	glfwTerminate();
}

void window_creation::run_window_loop() const
{
	lib_log_d("window_creation: starting window loop");

	// run main render thread from current thread
	while (!glfwWindowShouldClose(_glfw_window_ptr))
	{
		glfwPollEvents();

		// callback the parent which will do more stuff
		_render_callback();

#ifdef DEF_LIB_RENDERING_gl3
		glfwSwapBuffers(_glfw_window_ptr);
#endif
	}

	lib_log_d("window_creation: destroying window");
	glfwTerminate();
}

void window_creation::close_window() const
{
	glfwSetWindowShouldClose(_glfw_window_ptr, GLFW_TRUE);
}

void window_creation::focus_window() const
{
	glfwMakeContextCurrent(_glfw_window_ptr);
}

#ifndef DEF_LIB_RENDERING_off
std::weak_ptr<lib::rendering::renderer> window_creation::register_renderer()
{
	lib_log_d("window_creation: registering renderer");

	lib::point2Di window_size = {};
	glfwGetWindowSize(_glfw_window_ptr, &window_size.x, &window_size.y);

	lib::point2Di frame_buffer_size = {};
	glfwGetFramebufferSize(_glfw_window_ptr, &frame_buffer_size.x, &frame_buffer_size.y);

	_scale =
	{
		static_cast<float>(frame_buffer_size.x) / static_cast<float>(window_size.x),
		static_cast<float>(frame_buffer_size.y) / static_cast<float>(window_size.y)
	};

	_api_data = std::make_shared<lib::rendering::render_api_data_t>();

#ifdef DEF_LIB_RENDERING_vulkan
	helpers::create_vulkan_instance(_glfw_window_ptr, *_api_data);
#endif

	_renderer = std::make_shared<rendering::renderer>(_api_data, true);
	_renderer->set_window_size(window_size);

	return _renderer;
}

void window_creation::window_size_callback(GLFWwindow* window, int width, int height)
{
	const auto this_ptr = static_cast<window_creation*>(glfwGetWindowUserPointer(window));

	lib::point2Di frame_buffer_size = {};
	glfwGetFramebufferSize(window, &frame_buffer_size.x, &frame_buffer_size.y);

	this_ptr->_scale =
	{
		static_cast<float>(frame_buffer_size.x) / static_cast<float>(width),
		static_cast<float>(frame_buffer_size.y) / static_cast<float>(height)
	};

	this_ptr->_renderer->set_window_size({width, height});
}

#else
void window_creation::register_render_callback(std::function<void()> render_callback)
{
	_render_callback = std::move(render_callback);
}
#endif

#ifndef DEF_LIB_INPUT_off
std::weak_ptr<lib::input::input_handler> window_creation::register_input_handler()
{
	lib_log_d("window_creation: registering input_handler");

	_input_handler = std::make_shared<input::input_handler>();

	glfwSetKeyCallback(_glfw_window_ptr, key_callback);
	glfwSetScrollCallback(_glfw_window_ptr, scroll_callback);
	glfwSetCursorPosCallback(_glfw_window_ptr, cursor_position_callback);
	glfwSetMouseButtonCallback(_glfw_window_ptr, mouse_button_callback);

	return _input_handler;
}

void window_creation::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	(void)scancode;
	(void)mods;

	// only check for press and release
	if (action != GLFW_PRESS && action != GLFW_RELEASE)
	{
		return;
	}

	// _input_handler_ptr will always be valid here
	const lib::input::input_t input = {
		.type = input::keyboard,
		.key = helpers::glfw_to_input_key(key),
		.state = action == GLFW_PRESS,
	};

	const auto this_ptr = static_cast<window_creation*>(glfwGetWindowUserPointer(window));
	this_ptr->_input_handler->add_input(input);
}

void window_creation::scroll_callback(GLFWwindow* window, double offset_x, double offset_y)
{
	// _input_handler_ptr will always be valid here
	const lib::input::input_t input = {
		.type = input::mouse,
		.key = lib::input::key_button::mouse_scroll,
		.state = lib::point2Di{ static_cast<int>(offset_x), static_cast<int>(offset_y)}
	};

	const auto this_ptr = static_cast<window_creation*>(glfwGetWindowUserPointer(window));
	this_ptr->_input_handler->add_input(input);
}

void window_creation::cursor_position_callback(GLFWwindow* window, double pos_x, double pos_y)
{
	const auto this_ptr = static_cast<window_creation*>(glfwGetWindowUserPointer(window));

	// _input_handler_ptr will always be valid here
	const lib::input::input_t input = {
		.type = input::mouse,
		.key = lib::input::key_button::mouse_move,
		.state = lib::point2Di
		{
			static_cast<int>(static_cast<float>(pos_x) * this_ptr->_scale.x),
			static_cast<int>(static_cast<float>(pos_y) * this_ptr->_scale.y)
		}
	};

	this_ptr->_input_handler->add_input(input);
}

void window_creation::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	(void)mods;

	// _input_handler_ptr will always be valid here
	const lib::input::input_t input = {
		.type = input::mouse,
		.key = helpers::glfw_to_mouse_key(button),
		.state = action == GLFW_PRESS,
	};

	const auto this_ptr = static_cast<window_creation*>(glfwGetWindowUserPointer(window));
	this_ptr->_input_handler->add_input(input);
}
#endif
