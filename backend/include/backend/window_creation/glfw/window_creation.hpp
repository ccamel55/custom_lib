#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <backend/window_creation/window_creation_base.hpp>

namespace lib::backend
{
class window_creation : public window_creation_base
{
public:
	window_creation(std::string window_name, int pos_x, int pos_y, int width, int height, window_flags flags);
	~window_creation() override;

	void close_window() override;
	void focus_window() override;
	void window_loop() override;

	std::unique_ptr<renderer_base>& register_renderer(std::unique_ptr<renderer_base> renderer) override;
	std::unique_ptr<input_handler_base>& register_input_handler(
		std::unique_ptr<input_handler_base> input_handler) override;

private:
	static void input_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
	GLFWwindow* _glfw_window_ptr = nullptr;
};
}  // namespace lib::backend