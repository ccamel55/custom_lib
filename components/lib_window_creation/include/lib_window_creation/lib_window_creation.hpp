#pragma once

#include <string>
#include <functional>

#include <core_sdk/types/bitflag.hpp>

#ifndef DEF_LIB_RENDERING_off
#include <lib_rendering/lib_rendering.hpp>
#endif

#ifndef DEF_LIB_INPUT_off
#include <lib_input/lib_input.hpp>
#endif

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace lib::window_creation
{
enum window_flags : bitflag_t
{
	window_flag_none = 0 << 0,
	window_flag_no_border = 1 << 0,
	window_flag_resizeable = 1 << 1,
	window_flag_vsync = 1 << 2,
};

struct window_parameters_t
{
	std::string window_name;
	int x_position;
	int y_position;
	int width;
	int height;
	bitflag flags;
};

class window_creation
{
public:
	explicit window_creation(const window_parameters_t& window_parameters);
	~window_creation();

#ifndef DEF_LIB_RENDERING_off
	//! Return a reference to the renderer, because this window owns the renderer, there should never be a case
	//! where our window is destroyed and our renderer still exists
	std::weak_ptr<rendering::renderer> register_renderer();
#else
	//! register a callback function \param render_callback that is called every render frame
	void register_render_callback(std::function<void()> render_callback);
#endif

#ifndef DEF_LIB_INPUT_off
	//! Return a reference to the input handler, because this window owns the input handler, there should never be a
	//! case where our window is destroyed and our input handler still exists
	std::weak_ptr<input::input_handler> register_input_handler();
#endif

	//! run the loop that handles inputs from the window, this function does not return until the window is closed
	void run_window_loop() const;

	//! close the os window
	void close_window() const;

	//! bring the os window to focus
	void focus_window() const;

private:
#ifndef DEF_LIB_RENDERING_off
	static void window_size_callback(GLFWwindow* window, int width, int height);
#endif

#ifndef DEF_LIB_INPUT_off
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double offset_x, double offset_y);
	static void cursor_position_callback(GLFWwindow* window, double pos_x, double pos_y);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
#endif

private:
	GLFWwindow* _glfw_window_ptr = nullptr;
	std::function<void()> _render_callback = nullptr;

#ifndef DEF_LIB_RENDERING_off
	std::shared_ptr<rendering::renderer> _renderer = nullptr;
#endif

#ifndef DEF_LIB_INPUT_off
	std::shared_ptr<input::input_handler> _input_handler = nullptr;
#endif
};
}