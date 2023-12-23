#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <lib_window_creation/window_creation_base.hpp>

namespace lib::window_creation
{
class window_creation_impl : public window_creation_base
{
public:
	explicit window_creation_impl(const window_parameters_t& window_parameters);
	~window_creation_impl() override;

	void close_window() override;
	void focus_window() override;
	void window_loop() override;

	std::unique_ptr<rendering::renderer>& register_renderer(std::unique_ptr<rendering::renderer> renderer) override;

private:
	static void input_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
	GLFWwindow* _glfw_window_ptr = nullptr;
};
}  // namespace lib::window_creation