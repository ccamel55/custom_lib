#include <lib_window_creation/window_creation_impl.hpp>

#include <core_sdk/logger.hpp>

#include <cassert>
#include <chrono>

using namespace lib::window_creation;

namespace
{
// this is very annoying, there is nothing we can really do to get around this besides using an exposed variable.
window_creation_impl* this_ptr = nullptr;
}  // namespace

void window_creation_impl::input_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}

window_creation_impl::window_creation_impl(const window_parameters_t& window_parameters) :
	window_creation_base(window_parameters)
{
	// try init glfw
	if (!glfwInit())
	{
		lib_log_e("window_creation: failed to init glfw");
		assert(false);
	}

	if (_window_parameters.flags & window_flags::window_flag_opengl3)
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
	glfwWindowHint(GLFW_DECORATED, !(_window_parameters.flags & window_flags::window_flag_no_border));

	// allow resizing
	glfwWindowHint(GLFW_RESIZABLE, !!(_window_parameters.flags & window_flags::window_flag_resizeable));

	// create a new window
	_glfw_window_ptr = glfwCreateWindow(
		_window_parameters.width,
		_window_parameters.height,
		_window_parameters.window_name.data(),
		nullptr,
		nullptr
		);

	if (!_glfw_window_ptr)
	{
		glfwTerminate();

		lib_log_e("window_creation: failed to create glfw window");
		assert(false);
	}

	glfwSetWindowPos(_glfw_window_ptr, _window_parameters.x_position, _window_parameters.y_position);

	glfwMakeContextCurrent(_glfw_window_ptr);

	glfwSwapInterval(1);

	glfwSetInputMode(_glfw_window_ptr, GLFW_STICKY_KEYS, GLFW_TRUE);

	this_ptr = this;
}

window_creation_impl::~window_creation_impl()
{
	this_ptr = nullptr;
}

void window_creation_impl::close_window()
{
	glfwSetWindowShouldClose(_glfw_window_ptr, true);
}

void window_creation_impl::focus_window()
{
	glfwMakeContextCurrent(_glfw_window_ptr);
}

void window_creation_impl::window_loop()
{
	lib_log_d("window_creation: starting window loop");
	std::chrono::high_resolution_clock::time_point last_frame_time = {};

	// run main render thread from current thread
	while (glfwGetKey(_glfw_window_ptr, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(_glfw_window_ptr) == 0)
	{
		glfwPollEvents();

		// draw stuff here
		if (_render_callback)
		{
			_render_callback();
		}

#ifndef DEF_LIB_RENDERING_off
		if (_renderer)
		{
			_renderer->draw_frame();
			_renderer->set_frame_time(
				static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::high_resolution_clock::now() - last_frame_time).count()));

			last_frame_time = std::chrono::high_resolution_clock::now();
		}
#endif

		glfwSwapBuffers(_glfw_window_ptr);
	}

	lib_log_d("window_creation: destroying window");
	glfwTerminate();
}

#ifndef DEF_LIB_RENDERING_off
std::unique_ptr<lib::rendering::renderer>& window_creation_impl::register_renderer(
	std::unique_ptr<rendering::renderer> renderer)
{
	// pass renderer ownership to this window class
	auto& renderer_ref = window_creation_base::register_renderer(std::move(renderer));

	// initialize renderer using glfw stuff
	renderer_ref->bind_api(nullptr);
	renderer_ref->set_window_size({_window_parameters.width, _window_parameters.height});

	return renderer_ref;
}
#endif

#ifndef DEF_LIB_INPUT_off
std::unique_ptr<lib::input::input_handler>& window_creation_impl::register_input_handler(
	std::unique_ptr<lib::input::input_handler> input_handler)
{
	// pass renderer ownership to this window class
	auto& input_ref = window_creation_base::register_input_handler(std::move(input_handler));

	// register callback to glfw
	glfwSetKeyCallback(_glfw_window_ptr, input_callback);

	return input_ref;
}
#endif


