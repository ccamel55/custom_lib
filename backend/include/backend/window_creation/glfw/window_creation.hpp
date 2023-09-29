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

	void close_window() override;
	void focus_window() override;
	void window_loop() override;

private:
	GLFWwindow* _glfw_window_ptr = nullptr;
};
}  // namespace lib::backend