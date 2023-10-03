#pragma once

#include <GLFW/glfw3.h>
#include <backend/input_handler/input_handler_base.hpp>

namespace lib::backend
{
struct glfw_input_handler_data_t
{
	GLFWwindow* window;
	int key;
	int scancode;
	int action;
	int modifiers;
};

class input_handler : public input_handler_base
{
public:
	void update_state(void* data) override;
};
}  // namespace lib::backend