#include <lib_rendering/render_api.hpp>

#include <unordered_set>
#include <ranges>
#include <map>
#include <optional>

using namespace lib::rendering;

namespace
{
// assuming 1.f is highest priority, cant find shit anywhere :(
constexpr float queue_priority = 1.f;
constexpr uint32_t queue_index = 0;

const std::unordered_set<std::string> vulkan_instace_extensions =
{
	// requried extensions
	"VK_KHR_surface",
#if WIN32
	"VK_KHR_win32_surface",
#elif APPLE
	"VK_MVK_macos_surface",
	"VK_EXT_metal_surface",
	// apply needs this since vulkan 1.3.216
	"VK_KHR_portability_enumeration",
#endif

	// custom extensions we need to use

};

const std::unordered_set<std::string> vulkan_device_extensions =
{
	"VK_KHR_swapchain",
};

const std::unordered_set<std::string> validation_layers =
{
#ifndef NDEBUG
	"VK_LAYER_KHRONOS_validation"
#endif
};

const auto get_supported_extensions = [](const std::unordered_set<std::string>& extensions_set) {
	// has to std::string otherwise we pass a ptr to a string that is
	// nuke after this function leaves scope, ffs
	std::vector<std::string> extensions = {};

	uint32_t extension_count = 0;
	assert(vk::enumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr) == vk::Result::eSuccess);

	std::vector<vk::ExtensionProperties> extension_properties(extension_count);
	assert(vk::enumerateInstanceExtensionProperties(
		nullptr,
		&extension_count,
		extension_properties.data()) == vk::Result::eSuccess);

	std::ranges::for_each(
		extension_properties.begin(),
		extension_properties.end(),
		[&](const auto& properties) {
			if (const auto extensionName = std::string(properties.extensionName);
				extensions_set.contains(extensionName))
			{
				lib_log_d("render_api: added vulkan instance extension: {}", extensionName);
				extensions.emplace_back(extensionName);
			}
		});

	return extensions;
};

const auto get_supported_device_extensions = [](
	const vk::PhysicalDevice& device,
	const std::unordered_set<std::string>& extensions_set) {
	// has to std::string otherwise we pass a ptr to a string that is
	// nuke after this function leaves scope, ffs
	std::vector<std::string> extensions = {};

	uint32_t extension_count = 0;
	assert(device.enumerateDeviceExtensionProperties(nullptr, &extension_count, nullptr) == vk::Result::eSuccess);

	std::vector<vk::ExtensionProperties> extension_properties(extension_count);
	assert(device.enumerateDeviceExtensionProperties(
		nullptr,
		&extension_count,
		extension_properties.data()) == vk::Result::eSuccess);

	std::ranges::for_each(
		extension_properties.begin(),
		extension_properties.end(),
		[&](const auto& properties) {
			if (const auto extensionName = std::string(properties.extensionName);
				extensions_set.contains(extensionName))
			{
				lib_log_d("render_api: added vulkan device extension: {}", extensionName);
				extensions.emplace_back(extensionName);
			}
		});

	return extensions;
};

const auto get_supported_layers = [](const std::unordered_set<std::string>& layers_set) {
	// has to std::string otherwise we pass a ptr to a string that is
	// nuke after this function leaves scope, ffs
	std::vector<std::string> layers = {};

	uint32_t layer_count = 0;
	assert(vk::enumerateInstanceLayerProperties(&layer_count, nullptr) == vk::Result::eSuccess);

	std::vector<vk::LayerProperties> layer_properties(layer_count);
	assert(vk::enumerateInstanceLayerProperties(&layer_count, layer_properties.data()) == vk::Result::eSuccess);

	std::ranges::for_each(
		layer_properties.begin(),
		layer_properties.end(),
		[&](const auto& properties) {
			if (const auto layerName = std::string(properties.layerName);
				layers_set.contains(layerName))
			{
				lib_log_d("render_api: added vulkan layer: {}", layerName);
				layers.emplace_back(layerName);
			}
		});

	return layers;
};

struct swapchain_support_details
{
	vk::SurfaceCapabilitiesKHR capabilities = {};
	std::vector<vk::SurfaceFormatKHR> formats = {};
	std::vector<vk::PresentModeKHR> present_modes = {};
};

const auto get_candidate_devices = [](const vk::Instance& instance) -> std::multimap<uint32_t, vk::PhysicalDevice>
{
	uint32_t device_count = 0;
	assert(instance.enumeratePhysicalDevices(&device_count, nullptr) == vk::Result::eSuccess);
	assert(device_count != 0);

	std::vector<vk::PhysicalDevice> device_list(device_count);
	assert(instance.enumeratePhysicalDevices(&device_count, device_list.data()) == vk::Result::eSuccess);

	std::multimap<uint32_t, vk::PhysicalDevice> candidate_devices = {};
	std::ranges::for_each(
		device_list.begin(),
		device_list.end(),
		[&](const auto& device) {
			vk::PhysicalDeviceFeatures device_features = {};
			device.getFeatures(&device_features);

			if (device_features.geometryShader == 0)
			{
				return;
			}

			// select "highest scoring" graphics cards
			vk::PhysicalDeviceProperties device_properties = {};
			device.getProperties(&device_properties);

			// arbituary "ranking" of dedicated devices
			auto device_score = device_properties.limits.maxImageDimension2D;

			if (device_properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			{
				device_score += 1000;
			}

			candidate_devices.insert({device_score, device});
		});

	return candidate_devices;
};

const auto query_queue_family_properties = [](const vk::PhysicalDevice& device) -> std::vector<vk::QueueFamilyProperties>
{
	uint32_t queue_count = 0;
	device.getQueueFamilyProperties(&queue_count, nullptr);

	std::vector<vk::QueueFamilyProperties> queue_families(queue_count);
	device.getQueueFamilyProperties(&queue_count, queue_families.data());

	return queue_families;
};

const auto query_swapchain_support = [](
	const vk::PhysicalDevice& device,
	const vk::SurfaceKHR& surface) -> swapchain_support_details
{
	swapchain_support_details details = {};
	assert(device.getSurfaceCapabilitiesKHR(surface, &details.capabilities) == vk::Result::eSuccess);

	uint32_t format_count = 0;
	assert(device.getSurfaceFormatsKHR(surface, &format_count, nullptr) == vk::Result::eSuccess);

	details.formats.resize(format_count);
	assert(device.getSurfaceFormatsKHR(surface, &format_count, details.formats.data()) == vk::Result::eSuccess);

	uint32_t present_mode_couunt = 0;
	assert(device.getSurfacePresentModesKHR(surface, &present_mode_couunt, nullptr) == vk::Result::eSuccess);

	details.present_modes.resize(present_mode_couunt);
	assert(device.getSurfacePresentModesKHR(surface, &present_mode_couunt, details.present_modes.data()) == vk::Result::eSuccess);

	return details;
};

const auto choose_swapchain_format = [](
	const std::vector<vk::SurfaceFormatKHR>& surface_formats) -> vk::SurfaceFormatKHR
{
	// we want to use SRGB since thats kinda the standard
	// todo: confirm if color is BGRA or ARGB etc, all i know is it's 32bit lol
	for (const auto& format: surface_formats)
	{
		if (format.format != vk::Format::eB8G8R8A8Srgb)
		{
			continue;
		}

		if (format.colorSpace != vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			continue;
		}

		return format;
	}

	lib_log_e("render_api: failed to find surface format");
	assert(false);

	return {};
};

const auto choose_swapchain_present_mode = [](
	const std::vector<vk::PresentModeKHR>& present_modes) -> vk::PresentModeKHR
{
	for (const auto& mode: present_modes)
	{
		if (mode != vk::PresentModeKHR::eMailbox)
		{
			continue;
		}

		return mode;
	}

	// FIFO is guarenteed to exist
	lib_log_w("render_api: failed to find present mode, defaulting to fifo");
	return vk::PresentModeKHR::eFifo;
};

const auto choose_swapchain_extent = [](
	const vk::SurfaceCapabilitiesKHR& capabilities, const lib::point2Di& screen_size) -> vk::Extent2D
{
	// if we don't have a maximum screen size then use our extent
	if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)())
	{
		return capabilities.currentExtent;
	}

	// clamp extent to max capabilities min/max
	VkExtent2D extent = {
		static_cast<uint32_t>(screen_size.x),
		static_cast<uint32_t>(screen_size.y)
	};

	extent.width = std::clamp(
		extent.width,
		capabilities.minImageExtent.width,
		capabilities.maxImageExtent.width);

	extent.height = std::clamp(
		extent.height,
		capabilities.minImageExtent.height,
		capabilities.maxImageExtent.height);

	return extent;
};

}

render_api::render_api(void* api_context, bool flush_buffers) :
	render_api_base(api_context, flush_buffers)
{
	// api_context must be valid for vulkan
	assert(api_context);

	const auto supported_instance_extensions = get_supported_extensions(vulkan_instace_extensions);
	const auto supported_layers = get_supported_layers(validation_layers);

	// convert back into const char*, should be scoped for this function
	std::vector<const char*> instance_extensions_list = {};
	std::vector<const char*> layers_list = {};

	std::ranges::for_each(supported_instance_extensions.begin(), supported_instance_extensions.end(),
		[&](const auto& entry) {
		instance_extensions_list.push_back(entry.c_str());
	});

	std::ranges::for_each(supported_layers.begin(), supported_layers.end(),
		[&](const auto& entry) {
		layers_list.push_back(entry.c_str());
	});

	init_vulkan(instance_extensions_list, layers_list);
	init_surface();
	init_device(layers_list);
	init_swapcahin();

	// call at least once to ensure that we have everything setup for some resoltion once
	update_screen_size(default_window_size);
}

render_api::~render_api()
{
	std::ranges::for_each(_swapchain_image_views.begin(), _swapchain_image_views.end(),
		[&](const auto& image_view) {
		_logical_device.destroyImageView(image_view);
	});

	_logical_device.destroySwapchainKHR(_swap_chain);
	_logical_device.destroy();

	_instance.destroySurfaceKHR(_window_surface);
	_instance.destroy();
}

void render_api::bind_atlas(const uint8_t* data, int width, int height)
{

}

void render_api::update_screen_size(const lib::point2Di& window_size)
{
	_window_size = window_size;
}

void render_api::update_frame_buffer(const render_command& render_command)
{

}

void render_api::draw_frame_buffer()
{

}

void render_api::init_vulkan(const std::vector<const char*>& extensions, const std::vector<const char*>& layers)
{
	lib_log_d("render_api: init vulkan instance");

	vk::ApplicationInfo app_info = {};
	{
		app_info.sType = vk::StructureType::eApplicationInfo;
		app_info.pApplicationName = nullptr;
		app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		app_info.pEngineName = nullptr;
		app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		app_info.apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0);
	}

	vk::InstanceCreateInfo create_info{};
	{
		create_info.sType = vk::StructureType::eInstanceCreateInfo;
		create_info.pApplicationInfo = &app_info;

		create_info.enabledExtensionCount = extensions.size();
		create_info.ppEnabledExtensionNames = extensions.data();

		create_info.enabledLayerCount = layers.size();
		create_info.ppEnabledLayerNames = layers.data();

#if APPLE
		// apply needs this since vulkan 1.3.216
		create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
	}

	if (const auto result = vk::createInstance(&create_info, nullptr, &_instance); result != vk::Result::eSuccess)
	{
		lib_log_e("render_api: failed to create vulkan instance {}", static_cast<int>(result));
		assert(false);
	}
}

void render_api::init_device(const std::vector<const char*>& layers)
{
	const auto candidate_devices = get_candidate_devices(_instance);

	// last is first, ordered on lowest to highest
	if (candidate_devices.empty())
	{
		lib_log_e("render_api: no supported vulkan devices");
		assert(false);
	}

	std::optional<uint32_t> graphics_present_family = std::nullopt;

	// try each device, starting from "best" to worst
	for (const auto& device : std::views::values(candidate_devices))
	{
		// check if extensions we want are supported by the device
		const auto supported_device_extensions = get_supported_device_extensions(
			device,
			vulkan_device_extensions);

		if (supported_device_extensions.size() != vulkan_device_extensions.size())
		{
			continue;
		}

		// check if our swapchain can support at least one image,
		// note: important this is done after checking extensions support
		const auto swapchain_support_properties = query_swapchain_support(device, _window_surface);

		// for now, we only care if we have at least one supported format and present mode
		if (swapchain_support_properties.formats.empty() || swapchain_support_properties.present_modes.empty())
		{
			continue;
		}

		const auto queue_families = query_queue_family_properties(device);

		// find queue family that supports both presenting and graphics bits
		// note: this can be two seperate queues but we look for one that does both, which should result in
		// better performance
		for (size_t i = 0; i < queue_families.size(); i++)
		{
			vk::Bool32 present_support = false;
			assert(device.getSurfaceSupportKHR(i, _window_surface, &present_support) == vk::Result::eSuccess);

			if ((queue_families.at(i).queueFlags & vk::QueueFlagBits::eGraphics) && present_support)
			{
				_physical_device = device;
				graphics_present_family = i;

				break;
			}
		}

		if (graphics_present_family.has_value())
		{
			break;
		}
	}

	assert(_physical_device);
	assert(graphics_present_family.has_value());

	lib_log_d("render_api: found physical device");

	// find device extenions
	const auto supported_device_extensions = get_supported_device_extensions(
		_physical_device,
		vulkan_device_extensions);

	std::vector<const char*> devide_extensions_list = {};
	std::ranges::for_each(supported_device_extensions.begin(), supported_device_extensions.end(),
		[&](const auto& entry) {
		devide_extensions_list.push_back(entry.c_str());
	});

	// setup queues we need
	vk::DeviceQueueCreateInfo graphic_present_queue_create_info = {};
	{
		graphic_present_queue_create_info.sType = vk::StructureType::eDeviceQueueCreateInfo;
		graphic_present_queue_create_info.queueFamilyIndex = graphics_present_family.value();
		graphic_present_queue_create_info.queueCount = 1;
		graphic_present_queue_create_info.pQueuePriorities = &queue_priority;
	}

	vk::PhysicalDeviceFeatures device_features = {};
	{
		// todo: add shit for device features
	}

	vk::DeviceCreateInfo create_info = {};
	{
		create_info.sType = vk::StructureType::eDeviceCreateInfo;
		create_info.pQueueCreateInfos = &graphic_present_queue_create_info;
		create_info.queueCreateInfoCount = 1;
		create_info.pEnabledFeatures = &device_features;

		create_info.enabledExtensionCount = devide_extensions_list.size();
		create_info.ppEnabledExtensionNames = devide_extensions_list.data();

		create_info.enabledLayerCount = layers.size();
		create_info.ppEnabledLayerNames = layers.data();
	}

	if (const auto result = _physical_device.createDevice(&create_info, nullptr, &_logical_device);
		result != vk::Result::eSuccess)
	{
		lib_log_e("render_api: failed to create logical device {}", static_cast<int>(result));
		assert(false);
	}

	lib_log_d("render_api: created logical device");

	_logical_device.getQueue(
		graphics_present_family.value(),
		queue_index,
		&_graphics_present_queue);

	assert(_graphics_present_queue);
}

void render_api::init_surface()
{
#if WIN32
	vk::Win32SurfaceCreateInfoKHR surface_create_info = {};
	{
		surface_create_info.sType = vk::StructureType::eWin32SurfaceCreateInfoKHR;
		surface_create_info.hwnd = static_cast<HWND>(_api_context);
		surface_create_info.hinstance = GetModuleHandle(nullptr);
	}

	if (const auto result =_instance.createWin32SurfaceKHR(&surface_create_info, nullptr, &_window_surface);
		result != vk::Result::eSuccess)
	{
		lib_log_e("render_api: failed to create surface {}", static_cast<int>(result));
		assert(false);
	}

#elif APPLE
	// todo, find wtf apple uses with GLFW, I assume metal??
#error "Add vulkan apple implementation"
#endif

	assert(_window_surface);
	lib_log_d("render_api: created render surface");
}

void render_api::init_swapcahin()
{
	const auto swap_chain_support = query_swapchain_support(_physical_device, _window_surface);

	const auto surface_format = choose_swapchain_format(swap_chain_support.formats);
	const auto present_mode = choose_swapchain_present_mode(swap_chain_support.present_modes);

	// todo: use window size parameter
	// we always want to use +1 on minium image count since the driver might be doing other stuff which will
	// make us wait, slowing down the render process etc.
	const auto extent = choose_swapchain_extent(swap_chain_support.capabilities, default_window_size);
	auto image_count = swap_chain_support.capabilities.minImageCount + 1;

	// if maximum images is 0 then unlimited images supported, if not need to clamp to max
	if (swap_chain_support.capabilities.maxImageCount > 0)
	{
		lib_log_d("render_api: max image count enforced, clamping");
		image_count = std::min<uint32_t>(image_count, swap_chain_support.capabilities.maxImageCount);
	}

	lib_log_d("render_api: swap chain images: {}", image_count);

	vk::SwapchainCreateInfoKHR swapchain_create_info = {};
	{
		swapchain_create_info.sType = vk::StructureType::eSwapchainCreateInfoKHR;
		swapchain_create_info.surface = _window_surface;

		swapchain_create_info.minImageCount = image_count;
		swapchain_create_info.imageFormat = surface_format.format;
		swapchain_create_info.imageColorSpace = surface_format.colorSpace;
		swapchain_create_info.imageExtent = extent;
		swapchain_create_info.imageArrayLayers = 1;

		// we use eColorAttachment for now, when we want to deal with the fps limitng bs we want to
		// use VK_IMAGE_USAGE_TRANSFER_DST_BIT and then memcpy rendered image to the swapchain image
		swapchain_create_info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

		// since we speicfied that both the graphics and present family must be the same, then we can use
		// exclusive without any other changes, if we had two different families then we should use
		// concurrent
		swapchain_create_info.imageSharingMode = vk::SharingMode::eExclusive;
		swapchain_create_info.queueFamilyIndexCount = 0; // Optional
		swapchain_create_info.pQueueFamilyIndices = nullptr; // Optional

		// apply any image transformation we might want, we don't want to apply anything right now
		// so just use what capabilities specifies
		swapchain_create_info.preTransform = swap_chain_support.capabilities.currentTransform;

		// avoid blending with any other windows
		swapchain_create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

		swapchain_create_info.presentMode = present_mode;
		swapchain_create_info.clipped = vk::True;

		// what we want to create a swapchain from, used if we resize window
		// todo: change this depending on if we resize our window or not etc.
		swapchain_create_info.oldSwapchain = nullptr;
	}

	if (const auto result = _logical_device.createSwapchainKHR(
		&swapchain_create_info,
		nullptr,
		&_swap_chain);
		result != vk::Result::eSuccess)
	{
		lib_log_e("render_api: failed to create swap chain {}", static_cast<int>(result));
		assert(false);
	}

	assert(_swap_chain);
	lib_log_d("render_api: created swap chain");

	// get swap chain images
	image_count = 0;
    assert(_logical_device.getSwapchainImagesKHR(_swap_chain, &image_count, nullptr) == vk::Result::eSuccess);

	_swapchain_images.resize(image_count);
	assert(_logical_device.getSwapchainImagesKHR(
		_swap_chain,
		&image_count,
		_swapchain_images.data()) == vk::Result::eSuccess);

	assert(!_swapchain_images.empty());

	// save states for use late
	_swapchian_format = surface_format.format;
	_swapchain_extent = extent;
}

void render_api::init_image_views()
{
	_swapchain_image_views.resize(_swapchain_images.size());

	for (size_t i = 0; i < _swapchain_images.size(); i++)
	{
		vk::ImageViewCreateInfo image_view_create_info = {};
		{
			image_view_create_info.sType = vk::StructureType::eImageViewCreateInfo;
			image_view_create_info.image = _swapchain_images.at(i);
			image_view_create_info.viewType = vk::ImageViewType::e2D;
			image_view_create_info.format = _swapchian_format;

			// how we want to map image channels, we just want to stick to using default
			image_view_create_info.components.r = vk::ComponentSwizzle::eIdentity;
			image_view_create_info.components.g = vk::ComponentSwizzle::eIdentity;
			image_view_create_info.components.b = vk::ComponentSwizzle::eIdentity;
			image_view_create_info.components.a = vk::ComponentSwizzle::eIdentity;

			// 2d image, color target without mipmapping levels or multiple layers
			image_view_create_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
			image_view_create_info.subresourceRange.baseMipLevel = 0;
			image_view_create_info.subresourceRange.levelCount = 1;
			image_view_create_info.subresourceRange.baseArrayLayer = 0;
			image_view_create_info.subresourceRange.layerCount = 1;
		}

		if (const auto result = _logical_device.createImageView(
			&image_view_create_info,
			nullptr,
			&_swapchain_image_views.at(i));
			result != vk::Result::eSuccess)
		{
			lib_log_e("render_api: failed to create image view {}", static_cast<int>(result));
			assert(false);
		}

		assert(_swapchain_image_views.at(i));
		lib_log_w("render_api: created image view");
	}

	assert(!_swapchain_image_views.empty());
}


