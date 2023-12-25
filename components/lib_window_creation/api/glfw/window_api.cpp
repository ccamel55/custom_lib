#include <lib_window_creation/window_api.hpp>

#include <cassert>
#include <corecrt_io.h>

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

	if (window_parameters.flags & window_flags::window_flag_opengl3)
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
	glfwWindowHint(GLFW_DECORATED, !(window_parameters.flags & window_flags::window_flag_no_border));

	// allow resizing
	glfwWindowHint(GLFW_RESIZABLE, !!(window_parameters.flags & window_flags::window_flag_resizeable));

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

#ifndef DEF_LIB_RENDERING_off
bool window_api::register_renderer(std::unique_ptr<rendering::renderer>& renderer)
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
bool window_api::register_input_handler(std::unique_ptr<input::input_handler>& input_handler)
{
	// register callback to glfw
	_input_handler_ptr = input_handler.get();
	glfwSetKeyCallback(_glfw_window_ptr, input_callback);

	return true;
}
#endif

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

void window_api::input_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}