#pragma once

#ifdef CAMEL_NVRHI_VULKAN
#include <nvrhi/utils.h>
#include <nvrhi/validation.h>
#include <nvrhi/vulkan.h>

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
#endif

#ifdef CAMEL_NVRHI_DX_11
#include <Windows.h>
#include <dxgi1_5.h>
#include <dxgidebug.h>

#include <nvrhi/d3d11.h>
#include <nvrhi/validation.h>
#endif

#ifdef CAMEL_NVRHI_DX_12
#include <Windows.h>
#include <dxgi1_5.h>
#include <dxgidebug.h>

#include <nvrhi/d3d12.h>
#include <nvrhi/validation.h>
#endif