#pragma once

#ifndef DEF_LIB_RENDERING_off
#include <lib_rendering/lib_rendering.hpp>
#endif

#ifndef DEF_LIB_INPUT_off
#include <lib_input/lib_input.hpp>
#endif

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace lib::window_creation::helpers
{
#ifndef DEF_LIB_INPUT_off
lib::input::key_button glfw_to_input_key(int key);
lib::input::key_button glfw_to_mouse_key(int button);
#endif
};