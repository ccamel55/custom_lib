#include <backend/window_creation/glfw/window_creation.hpp>

using namespace lib::backend;

window_creation::window_creation(std::string window_name, int pos_x, int pos_y, int width, int height, window_flags flags) :
	window_creation_base(std::move(window_name), pos_x, pos_y, width, height, flags)
{
	// try init glfw
	if (!glfwInit())
	{
		throw "backend_window: failed to initialize glfw";
	}

	// disable msaa
	glfwWindowHint(GLFW_SAMPLES, 0);

	// no backend
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// remove border and title bar
	glfwWindowHint(GLFW_DECORATED, !_flags.has_flag(window_flags::window_flag_no_border));

	// create a new window
	_glfw_window_ptr = glfwCreateWindow(_window_size._x, _window_size._y, _window_name.data(), nullptr, nullptr);

	if (!_glfw_window_ptr)
	{
		glfwTerminate();
		throw "backend_window: could not create window";
	}

	// move to correct location
	glfwSetWindowPos(_glfw_window_ptr, _window_position._x, _window_position._y);

	// focus window
	glfwMakeContextCurrent(_glfw_window_ptr); // Initialize GLEW

	// make sure we can capture escape key
	glfwSetInputMode(_glfw_window_ptr, GLFW_STICKY_KEYS, GL_TRUE);

	if (_renderer) {
		_renderer->init_instance();
	}
}

void window_creation::close_window()
{
	glfwSetWindowShouldClose(_glfw_window_ptr, true);
};

void window_creation::focus_window()
{
	glfwMakeContextCurrent(_glfw_window_ptr);
};

void window_creation::window_loop()
{
	// run main render thread from current thread
	while (glfwGetKey(_glfw_window_ptr, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(_glfw_window_ptr) == 0)
	{
		// draw stuff here
		if (_render_callback)
		{
			_render_callback();
		}

		// Poll for inputs
		glfwPollEvents();
	}

	// stop glfw
	glfwTerminate();
}