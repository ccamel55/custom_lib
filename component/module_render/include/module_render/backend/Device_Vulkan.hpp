#pragma once

#include <module_render/backend/Device_Common.hpp>

namespace lib::render {
// This will either create a new Vulkan device or get a pointer to the existing vulkan device
class Device_Vulkan : public Device_Common {

};
}