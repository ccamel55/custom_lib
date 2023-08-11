#include <backend/window_creation/glfw/window_creation.hpp>
#include <misc/lib_exception.hpp>

using namespace lib::backend;

window_creation::window_creation(std::string window_name, int pos_x, int pos_y, int width, int height, window_flags flags) :
	window_creation_base(std::move(window_name), pos_x, pos_y, width, height, flags)
{
	// try init glfw
	if (!glfwInit())
	{
		throw lib::lib_exception("window_creation: failed to init glfw");
	}

	if (_flags.has_flag(window_flags::window_flag_opengl3))
	{
		// Opengl version 3.3
		glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
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
	glfwWindowHint(GLFW_DECORATED, !_flags.has_flag(window_flags::window_flag_no_border));

	// allow resizing
	glfwWindowHint(GLFW_RESIZABLE, _flags.has_flag(window_flags::window_flag_resizeable));

	// create a new window
	_glfw_window_ptr = glfwCreateWindow(_window_size._x, _window_size._y, _window_name.data(), nullptr, nullptr);

	if (!_glfw_window_ptr)
	{
		glfwTerminate();
		throw lib::lib_exception("window_creation: failed to create glfw window");
	}

	glfwSetWindowPos(_glfw_window_ptr, _window_position._x, _window_position._y);

	glfwMakeContextCurrent(_glfw_window_ptr);

	glfwSwapInterval(1);

	glfwSetInputMode(_glfw_window_ptr, GLFW_STICKY_KEYS, GLFW_TRUE);
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
	lib_log_d("window_creation: starting window loop");

	// run main render thread from current thread
	while (glfwGetKey(_glfw_window_ptr, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(_glfw_window_ptr) == 0)
	{
		// draw stuff here
		if (_render_callback)
		{
			_render_callback();
		}

		glfwSwapBuffers(_glfw_window_ptr);
		glfwPollEvents();
	}

	lib_log_d("window_creation: destroying window");
	glfwTerminate();
}