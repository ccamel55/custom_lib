#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <lib_window_creation/window_api_base.hpp>

namespace lib::window_creation
{
class window_api : public window_api_base
{
public:
	window_api(const window_parameters_t& window_parameters, std::function<void()> window_loop_callback);
	~window_api() override;

#ifndef DEF_LIB_RENDERING_off
	bool register_renderer(std::shared_ptr<rendering::renderer>& renderer) final;
#endif

#ifndef DEF_LIB_INPUT_off
	bool register_input_handler(std::shared_ptr<input::input_handler>& input_handler) final;
#endif

	void window_loop() const final;
	void close_window() const final;
	void focus_window() const final;

private:
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double offset_x, double offset_y);
	static void cursor_position_callback(GLFWwindow* window, double pos_x, double pos_y);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

private:
	GLFWwindow* _glfw_window_ptr = nullptr;

};
}  // namespace lib::window_creation