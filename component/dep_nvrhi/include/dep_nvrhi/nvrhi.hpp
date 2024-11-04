#pragma once

#include <nvrhi/nvrhi.h>
#include <nvrhi/utils.h>
#include <nvrhi/validation.h>

#ifdef CAMEL_NVRHI_VULKAN
#include <nvrhi/vulkan.h>

// these macros can suck dick
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
#endif

#ifdef CAMEL_NVRHI_DX_11
#include <nvrhi/d3d11.h>
#include <d3d11.h>
#endif

#ifdef CAMEL_NVRHI_DX_12
#include <nvrhi/d3d12.h>
#include <d3d12.h>
#endif