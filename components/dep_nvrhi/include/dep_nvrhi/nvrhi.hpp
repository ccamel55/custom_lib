#pragma once

#include <nvrhi/nvrhi.h>

#include <nvrhi/vulkan.h>
#include <nvrhi/validation.h>

// these macros can suck dick

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>