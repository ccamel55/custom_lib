#include "glad/glad.hpp"

#include <backend/input_handler/glfw/input_handler.hpp>
#include <backend/window_creation/glfw/window_creation.hpp>

#include <cassert>
#include <chrono>

using namespace lib::backend;

namespace
{
// this is very annoying, there is nothing we can really do to get around this besides using an exposed variable.
window_creation* this_ptr = nullptr;
}  // namespace

void window_creation::input_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (!this_ptr->_input_handler)
	{
		return;
	}

	glfw_input_handler_data_t data = {};
	{
		data.window = window;
		data.key = key;
		data.scancode = scancode;
		data.action = action;
		data.modifiers = mods;
	}

	this_ptr->_input_handler->update_state(&data);
}

window_creation::window_creation(
	std::string window_name, int pos_x, int pos_y, int width, int height, window_flags flags) :
	window_creation_base(std::move(window_name), pos_x, pos_y, width, height, flags)
{
	// try init glfw
	if (!glfwInit())
	{
		lib_log_e("window_creation: failed to init glfw");
		assert(false);
	}

	if (_flags.has_flag(window_flags::window_flag_opengl3))
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
	glfwWindowHint(GLFW_DECORATED, !_flags.has_flag(window_flags::window_flag_no_border));

	// allow resizing
	glfwWindowHint(GLFW_RESIZABLE, _flags.has_flag(window_flags::window_flag_resizeable));

	// create a new window
	_glfw_window_ptr = glfwCreateWindow(_window_size._x, _window_size._y, _window_name.data(), nullptr, nullptr);

	if (!_glfw_window_ptr)
	{
		glfwTerminate();

		lib_log_e("window_creation: failed to create glfw window");
		assert(false);
	}

	glfwSetWindowPos(_glfw_window_ptr, _window_position._x, _window_position._y);

	glfwMakeContextCurrent(_glfw_window_ptr);

	glfwSwapInterval(1);

	glfwSetInputMode(_glfw_window_ptr, GLFW_STICKY_KEYS, GLFW_TRUE);

	this_ptr = this;
}

window_creation::~window_creation()
{
	this_ptr = nullptr;
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

	std::chrono::high_resolution_clock::time_point frame_start_time = {};
	std::chrono::high_resolution_clock::time_point frame_end_time = {};

	// run main render thread from current thread
	while (glfwGetKey(_glfw_window_ptr, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(_glfw_window_ptr) == 0)
	{
		if (_renderer)
		{
			frame_start_time = std::chrono::high_resolution_clock::now();

			// draw stuff here
			if (_render_callback)
			{
				_render_callback();
				_renderer->draw_frame();
			}

			frame_end_time = std::chrono::high_resolution_clock::now();

			_renderer->set_frame_time(
				static_cast<float>(
					std::chrono::duration_cast<std::chrono::nanoseconds>(frame_end_time - frame_start_time).count()) /
				1000000.f);
		}

		glfwSwapBuffers(_glfw_window_ptr);
		glfwPollEvents();
	}

	lib_log_d("window_creation: destroying window");
	glfwTerminate();
}

std::unique_ptr<renderer>& window_creation::register_renderer(std::unique_ptr<renderer> renderer)
{
	// pass renderer ownership to this window class
	auto& renderer_ref = window_creation_base::register_renderer(std::move(renderer));

	// initialize renderer using glfw stuff
	renderer_ref->bind_api(nullptr);
	renderer_ref->set_window_size(_window_size);

	return renderer_ref;
}

std::unique_ptr<input_handler_base>& window_creation::register_input_handler(
	std::unique_ptr<input_handler_base> input_handler)
{
	// pass renderer ownership to this window class
	auto& input_ref = window_creation_base::register_input_handler(std::move(input_handler));

	// register callback to glfw
	glfwSetKeyCallback(_glfw_window_ptr, input_callback);

	return input_ref;
}