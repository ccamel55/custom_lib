#include <lib_rendering/render_api.hpp>

// include our shaders
#include <lib_rendering/shaders/basic_shader_frag.hpp>
#include <lib_rendering/shaders/basic_shader_vert.hpp>

#include <unordered_set>
#include <ranges>
#include <map>
#include <optional>
#include <filesystem>

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

struct swapchain_support_details
{
	vk::SurfaceCapabilitiesKHR capabilities = {};
	std::vector<vk::SurfaceFormatKHR> formats = {};
	std::vector<vk::PresentModeKHR> present_modes = {};
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

const auto create_shader_module = [](
	const vk::Device& device, const uint8_t shader_bytes[], const uint32_t shader_size) -> vk::ShaderModule
{
	vk::ShaderModuleCreateInfo shader_create_info = {};
	{
		shader_create_info.sType = vk::StructureType::eShaderModuleCreateInfo;
		shader_create_info.codeSize = shader_size;

		// data is expected as uint32, but we give as char, this is bad usally but data in vector
		// is guarenteed to be aligned, thus uint32 should never overflow
		shader_create_info.pCode = reinterpret_cast<const uint32_t*>(shader_bytes);
	}

	vk::ShaderModule shader_module = nullptr;

	if (const auto result = device.createShaderModule(&shader_create_info, nullptr, &shader_module);
		result != vk::Result::eSuccess)
	{
		lib_log_e("render_api: failed to create shader module {}", static_cast<int>(result));
		assert(false);
	}

	assert(shader_module);
	return shader_module;
};

const auto find_memory_type = [](
	const vk::PhysicalDevice& device,
	const vk::MemoryPropertyFlags& memory_property_flags,
	uint32_t type_filter) -> uint32_t
{
	vk::PhysicalDeviceMemoryProperties memory_properties = {};
	device.getMemoryProperties(&memory_properties);

	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++)
	{
		if ((type_filter & (1 << i)) &&
			(memory_properties.memoryTypes[i].propertyFlags & memory_property_flags) == memory_property_flags)
		{
			return i;
		}
	}

	lib_log_e("render_api: could not find appropriate memory properties");
	assert(false);

	return 0;
};

const auto get_vertex_binding_description = []() -> vk::VertexInputBindingDescription
{
	vk::VertexInputBindingDescription description = {};
	{
		description.binding = 0;
		description.stride = sizeof(vertex_t);
		description.inputRate = vk::VertexInputRate::eVertex;
	}

	return description;
};

const auto get_vertex_attribute_descriptions = []() -> std::array<vk::VertexInputAttributeDescription, 3>
{
	std::array<vk::VertexInputAttributeDescription, 3> descriptions = {};
	{
		{
			auto& pos_description = descriptions.at(0);

			pos_description.binding = 0;
			pos_description.location = 0;
			pos_description.format = vk::Format::eR32G32Sfloat;
			pos_description.offset = offsetof(vertex_t, position);
		}

		{
			auto& color_description = descriptions.at(1);
			color_description.binding = 0;
			color_description.location = 1;
			color_description.format = vk::Format::eR32Uint;
			color_description.offset = offsetof(vertex_t, color);
		}

		{
			auto& texture_pos_description = descriptions.at(2);

			texture_pos_description.binding = 0;
			texture_pos_description.location = 2;
			texture_pos_description.format = vk::Format::eR32G32Sfloat;
			texture_pos_description.offset = offsetof(vertex_t, texture_position);
		}
	}

	return descriptions;
};

const auto create_buffer = [](
	const vk::PhysicalDevice& physical_device,
	const vk::Device& device,
	vk::DeviceSize size,
	vk::BufferUsageFlags buffer_usage_flags,
	vk::MemoryPropertyFlags memory_property_flags,
	vk::Buffer& buffer,
	vk::DeviceMemory& device_memory)
{
	vk::BufferCreateInfo buffer_create_info = {};
	{
		buffer_create_info.sType = vk::StructureType::eBufferCreateInfo;
		buffer_create_info.size = size;

		buffer_create_info.usage = buffer_usage_flags;
		buffer_create_info.sharingMode = vk::SharingMode::eExclusive;
	}

	if (const auto result = device.createBuffer(&buffer_create_info, nullptr, &buffer);
		result != vk::Result::eSuccess)
	{
		lib_log_e("render_api: failed to create buffer {}", static_cast<int>(result));
		assert(false);
	}

	assert(buffer);

	// allocate memory
	vk::MemoryRequirements memory_requirements = {};
	device.getBufferMemoryRequirements(buffer, &memory_requirements);

	vk::MemoryAllocateInfo memory_allocate_info = {};
	{
		memory_allocate_info.sType = vk::StructureType::eMemoryAllocateInfo;
		memory_allocate_info.allocationSize = memory_requirements.size;

		memory_allocate_info.memoryTypeIndex = find_memory_type(
			physical_device,
			memory_property_flags,
			memory_requirements.memoryTypeBits
		);
	}

	if (const auto result = device.allocateMemory(
		&memory_allocate_info, nullptr, &device_memory);
		result != vk::Result::eSuccess)
	{
		lib_log_e("render_api: failed to allocate buffer memory {}", static_cast<int>(result));
		assert(false);
	}

	assert(device_memory);
	device.bindBufferMemory(buffer, device_memory, 0);
};

const auto copy_buffer = [](
	const vk::Device& device,
	const vk::CommandPool& command_pool,
	const vk::Queue& queue,
	vk::Buffer src_buffer,
	vk::Buffer dst_buffer,
	vk::DeviceSize size)
{
	vk::CommandBufferAllocateInfo allocate_info = {};
	{
		allocate_info.sType = vk::StructureType::eCommandBufferAllocateInfo;
		allocate_info.level = vk::CommandBufferLevel::ePrimary;

		allocate_info.commandPool = command_pool;
		allocate_info.commandBufferCount = 1;
	}

	vk::CommandBuffer command_buffer = nullptr;
	assert(device.allocateCommandBuffers(&allocate_info, &command_buffer) == vk::Result::eSuccess);

	// start recording
	vk::CommandBufferBeginInfo buffer_begin_info = {};
	{
		buffer_begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;
		buffer_begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
	}

	assert(command_buffer.begin(&buffer_begin_info) == vk::Result::eSuccess);

	// copy src into dst buffer
	vk::BufferCopy copy_region = {};
	{
		copy_region.srcOffset = 0;
		copy_region.dstOffset = 0;
		copy_region.size = size;
	}

	command_buffer.copyBuffer(src_buffer, dst_buffer, 1, &copy_region);
	command_buffer.end();

	// execute the command buffer
	vk::SubmitInfo submit_info = {};
	{
		submit_info.sType = vk::StructureType::eSubmitInfo;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;
	}

	assert(queue.submit(1, &submit_info, nullptr) == vk::Result::eSuccess);
	queue.waitIdle();

	// cleanup buffers
	device.freeCommandBuffers(command_pool, 1, &command_buffer);
};

constexpr std::array<vertex_t, 3> vertex_buffer_test = {
	vertex_t{ { 0.f, -0.5f }, { 255, 0, 0 }, { 0.f, 0.f } },
	vertex_t{ { 0.5f, 0.5f }, { 0, 255, 0 }, { 0.f, 0.f } },
	vertex_t{ { -0.5f, 0.5f }, { 0, 0, 255 }, { 0.f, 0.f } },
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

	// setup vulkan for drawing
	init_vulkan(instance_extensions_list, layers_list);
	init_surface();
	init_device(layers_list);
	init_swapcahin();
	init_image_views();

	// setup pipeline
	init_render_passes();
	init_graphics_pipeline();
	init_frame_buffers();
	init_command_pool();
	init_vertex_buffer();
	init_index_buffer();
	init_command_buffer();

	// setup synchronization objects
	vk::SemaphoreCreateInfo semaphore_create_info = {};
	{
		semaphore_create_info.sType = vk::StructureType::eSemaphoreCreateInfo;
	}

	vk::FenceCreateInfo fence_create_info = {};
	{
		fence_create_info.sType = vk::StructureType::eFenceCreateInfo;
		fence_create_info.flags = vk::FenceCreateFlagBits::eSignaled;
	}

	for (size_t i = 0; i < vulkan::max_frames_in_flight; i++)
	{
		if (_logical_device.createSemaphore(
			&semaphore_create_info,
			nullptr,
			&_image_available_semaphores.at(i)) != vk::Result::eSuccess ||
		_logical_device.createSemaphore(
			&semaphore_create_info,
			nullptr,
			&_render_finished_semaphores.at(i)) != vk::Result::eSuccess ||
		_logical_device.createFence(
			&fence_create_info,
			nullptr,
			&_in_flight_fences.at(i)) != vk::Result::eSuccess)
		{
			lib_log_e("render_api: failed to create synchronization objects");
			assert(false);
		}
	}

	// call at least once to ensure that we have everything setup for some resoltion once
	update_screen_size(default_window_size);
}

render_api::~render_api()
{
	_logical_device.waitIdle();
	destroy_swapchain();

	_logical_device.destroyBuffer(_vertex_buffer);
	_logical_device.freeMemory(_vertex_buffer_memory);

	_logical_device.destroyBuffer(_index_buffer);
	_logical_device.freeMemory(_index_buffer_memory);

	_logical_device.destroyPipeline(_pipeline);
	_logical_device.destroyPipelineLayout(_pipeline_layout);
	_logical_device.destroyRenderPass(_render_pass);

	for (size_t i = 0; i < vulkan::max_frames_in_flight; i++)
	{
		_logical_device.destroySemaphore(_image_available_semaphores.at(i));
		_logical_device.destroySemaphore(_render_finished_semaphores.at(i));
		_logical_device.destroyFence(_in_flight_fences.at(i));
	}

	_logical_device.destroyCommandPool(_command_pool);
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
	_stop_rendering = (_window_size.x == 0 || _window_size.y == 0);

	if (_stop_rendering)
	{
		return;
	}

	_logical_device.waitIdle();
	destroy_swapchain();

	init_swapcahin();
	init_image_views();
	init_frame_buffers();
}

void render_api::update_frame_buffer(const render_command& render_command)
{
	if (_stop_rendering)
	{
		return;
	}
}

void render_api::draw_frame_buffer()
{
	if (_stop_rendering)
	{
		return;
	}

	// wait for gpu to finish drawing previous frame
	assert(_logical_device.waitForFences(
		1,
		&_in_flight_fences.at(_current_frame),
		vk::True, UINT64_MAX) == vk::Result::eSuccess);

	assert(_logical_device.resetFences(1, &_in_flight_fences.at(_current_frame)) == vk::Result::eSuccess);

	// aquire an image from the swap chain
	uint32_t next_image_index = 0;
	assert(_logical_device.acquireNextImageKHR(
		_swap_chain,
		UINT64_MAX,
		_image_available_semaphores.at(_current_frame),
		nullptr,
		&next_image_index) == vk::Result::eSuccess);

	// reset command buffer and render what we want
	_command_buffers.at(_current_frame).reset();
	record_command_buffer(_command_buffers.at(_current_frame), next_image_index);

	// submit command buffer
	const std::array<vk::Semaphore, 1> wait_semaphores = {
		_image_available_semaphores.at(_current_frame)
	};

	const std::array<vk::Semaphore, 1> signal_semaphores = {
		_render_finished_semaphores.at(_current_frame)
	};

	constexpr std::array<vk::PipelineStageFlags, 1> wait_stages = {
		vk::PipelineStageFlagBits::eColorAttachmentOutput
	};

	vk::SubmitInfo submit_info = {};
	{
		submit_info.sType = vk::StructureType::eSubmitInfo;

		submit_info.waitSemaphoreCount = wait_semaphores.size();
		submit_info.pWaitSemaphores = wait_semaphores.data();

		submit_info.pWaitDstStageMask = wait_stages.data();

		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &_command_buffers.at(_current_frame);

		submit_info.signalSemaphoreCount = signal_semaphores.size();
		submit_info.pSignalSemaphores = signal_semaphores.data();
	}

	assert(_graphics_present_queue.submit(
		1,
		&submit_info,
		_in_flight_fences.at(_current_frame)) == vk::Result::eSuccess);

	const std::array<vk::SwapchainKHR, 1> swapchains = {
		_swap_chain
	};

	vk::PresentInfoKHR present_info = {};
	{
		present_info.sType = vk::StructureType::ePresentInfoKHR;

		present_info.waitSemaphoreCount = signal_semaphores.size();
		present_info.pWaitSemaphores = signal_semaphores.data();

		present_info.swapchainCount = swapchains.size();
		present_info.pSwapchains = swapchains.data();

		present_info.pImageIndices = &next_image_index;
		present_info.pResults = nullptr;
	}

	assert( _graphics_present_queue.presentKHR(&present_info) == vk::Result::eSuccess);
	_current_frame = (_current_frame + 1) % vulkan::max_frames_in_flight;
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

	if (const auto result = vk::createInstance(
		&create_info,
		nullptr,
		&_instance); result != vk::Result::eSuccess)
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
				_graphics_present_family_index = i;

				break;
			}
		}

		if (_graphics_present_family_index.has_value())
		{
			break;
		}
	}

	assert(_physical_device);
	assert(_graphics_present_family_index.has_value());

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
	std::array<vk::DeviceQueueCreateInfo, 1> device_queue_create_infos = {};
	{
		{
			auto& graphic_present_queue_create_info = device_queue_create_infos[0];

			graphic_present_queue_create_info.sType = vk::StructureType::eDeviceQueueCreateInfo;
			graphic_present_queue_create_info.queueFamilyIndex = _graphics_present_family_index.value();
			graphic_present_queue_create_info.queueCount = 1;
			graphic_present_queue_create_info.pQueuePriorities = &queue_priority;
		}
	}

	vk::PhysicalDeviceFeatures device_features = {};
	{
		// todo: add shit for device features
	}

	vk::DeviceCreateInfo create_info = {};
	{
		create_info.sType = vk::StructureType::eDeviceCreateInfo;
		create_info.pQueueCreateInfos = device_queue_create_infos.data();
		create_info.queueCreateInfoCount = device_queue_create_infos.size();
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
		_graphics_present_family_index.value(),
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
	const auto extent = choose_swapchain_extent(swap_chain_support.capabilities, _window_size);
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

void render_api::init_render_passes()
{
	constexpr int num_attatchments = 1;
	constexpr int num_subpasses = 1;

	std::array<vk::AttachmentDescription, num_attatchments> color_attachment_descriptions = {};
	{
		{
			auto& color_attatchment = color_attachment_descriptions[0];

			color_attatchment.format = _swapchian_format;
			color_attatchment.samples = vk::SampleCountFlagBits::e1;

			color_attatchment.loadOp = vk::AttachmentLoadOp::eClear;
			color_attatchment.storeOp = vk::AttachmentStoreOp::eStore;

			// we dont do nothing with the stencil buffer, just set to what ever
			color_attatchment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
			color_attatchment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

			// don't care about image layout, present it to swap chain
			color_attatchment.initialLayout = vk::ImageLayout::eUndefined;
			color_attatchment.finalLayout = vk::ImageLayout::ePresentSrcKHR;
		}
	}

	// we only have 1 sub pass since we don't do any fancy post processing crap in our renderer
	std::array<vk::AttachmentReference, num_attatchments> color_attachment_references = {};
	{
		{
			auto& color_attatchment_reference = color_attachment_references[0];

			// refereces layout location, eg: layout(location = 0) out vec4 outColor
			color_attatchment_reference.attachment = 0;
			color_attatchment_reference.layout = vk::ImageLayout::eColorAttachmentOptimal;
		}
	}

	std::array<vk::SubpassDescription, num_subpasses> subpass_descriptions = {};
	{
		{
			auto& subpass_description = subpass_descriptions[0];

			// colorAttachmentCount = how many attatch
			subpass_description.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
			subpass_description.colorAttachmentCount = color_attachment_references.size();
			subpass_description.pColorAttachments = color_attachment_references.data();
		}
	}

	std::array<vk::SubpassDependency, 1> subpass_dependencies = {};
	{
		{
			auto& subpass_dependency = subpass_dependencies[0];

			subpass_dependency.srcSubpass = vk::SubpassExternal;
			subpass_dependency.dstSubpass = 0;

			subpass_dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
			subpass_dependency.srcAccessMask = static_cast<vk::AccessFlags>(0);

			subpass_dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
			subpass_dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
		}
	}

	vk::RenderPassCreateInfo render_pass_create_info = {};
	{
		render_pass_create_info.sType = vk::StructureType::eRenderPassCreateInfo;

		render_pass_create_info.attachmentCount = color_attachment_descriptions.size();
		render_pass_create_info.pAttachments = color_attachment_descriptions.data();

		// only one subpass
		render_pass_create_info.subpassCount = subpass_descriptions.size();
		render_pass_create_info.pSubpasses = subpass_descriptions.data();

		render_pass_create_info.dependencyCount = subpass_dependencies.size();
		render_pass_create_info.pDependencies = subpass_dependencies.data();
	}

	if (const auto result = _logical_device.createRenderPass(&render_pass_create_info, nullptr, &_render_pass);
		result != vk::Result::eSuccess)
	{
		lib_log_e("render_api: failed to create render pass {}", static_cast<int>(result));
		assert(false);
	}

	assert(_render_pass);
	lib_log_d("render_api: setup render pass");
}

void render_api::init_graphics_pipeline()
{
	// setup programmable parts of the pipeline

	// shader modules can be nuked after creating pipeline
	const auto vertex_shader = create_shader_module(
		_logical_device,
		vulkan::shaders::basic_shader_vert,
		vulkan::shaders::basic_shader_vert_length);

	const auto fragment_shader = create_shader_module(_logical_device,
		vulkan::shaders::basic_shader_frag,
		vulkan::shaders::basic_shader_frag_length);

	// [0] = vertex
	// [1] = fragment
	std::array<vk::PipelineShaderStageCreateInfo, 2> shader_stage_create_infos = {};
	{
		{
			auto& vertex_stage_create_info = shader_stage_create_infos[0];
			vertex_stage_create_info.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
			vertex_stage_create_info.stage = vk::ShaderStageFlagBits::eVertex;
			vertex_stage_create_info.module = vertex_shader;

			// entry point, we can combine multiple shaders into one by specifying a entry point
			vertex_stage_create_info.pName = "main";
		}

		{
			auto& fragment_stage_create_info = shader_stage_create_infos[1];
			fragment_stage_create_info.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
			fragment_stage_create_info.stage = vk::ShaderStageFlagBits::eFragment;
			fragment_stage_create_info.module = fragment_shader;

			// entry point, we can combine multiple shaders into one by specifying a entry point
			fragment_stage_create_info.pName = "main";
		}
	}

	// setup fixed function parts of the pipeline

	// specify vertex data format
	constexpr auto binding_description = get_vertex_binding_description();
	constexpr auto attribute_description = get_vertex_attribute_descriptions();

	vk::PipelineVertexInputStateCreateInfo vertex_input_state_create_info = {};
	{
		vertex_input_state_create_info.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo;

		vertex_input_state_create_info.vertexBindingDescriptionCount = 1;
		vertex_input_state_create_info.pVertexBindingDescriptions = &binding_description; // Optional

		vertex_input_state_create_info.vertexAttributeDescriptionCount = attribute_description.size();
		vertex_input_state_create_info.pVertexAttributeDescriptions = attribute_description.data(); // Optional
	}

	// specify what primitives we want to use, our renderer only uses triangles
	// todo: enable index buffer support
	vk::PipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {};
	{
		input_assembly_state_create_info.sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo;
		input_assembly_state_create_info.topology = vk::PrimitiveTopology::eTriangleList;
		input_assembly_state_create_info.primitiveRestartEnable = vk::False;
	}

	constexpr std::array<vk::DynamicState, 2> dynamic_states = {
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor
	};

	vk::PipelineDynamicStateCreateInfo dynamic_state_create_info = {};
	{
		dynamic_state_create_info.sType = vk::StructureType::ePipelineDynamicStateCreateInfo;
		dynamic_state_create_info.dynamicStateCount = dynamic_states.size();
		dynamic_state_create_info.pDynamicStates = dynamic_states.data();
	}

	vk::PipelineViewportStateCreateInfo viewport_state_create_info = {};
	{
		viewport_state_create_info.sType = vk::StructureType::ePipelineViewportStateCreateInfo;
		viewport_state_create_info.viewportCount = 1;
		viewport_state_create_info.scissorCount = 1;
	}

	// setup rastertizer
	vk::PipelineRasterizationStateCreateInfo rasterization_state_create_info = {};
	{
		rasterization_state_create_info.sType = vk::StructureType::ePipelineRasterizationStateCreateInfo;

		// dont touch these unless really needed, if do touch refer to documentation coz most require extra GPU features
		rasterization_state_create_info.depthClampEnable = vk::False;
		rasterization_state_create_info.rasterizerDiscardEnable = vk::False;
		rasterization_state_create_info.polygonMode = vk::PolygonMode::eFill;
		rasterization_state_create_info.lineWidth = 1.f;

		// disable culling
		rasterization_state_create_info.cullMode = vk::CullModeFlagBits::eNone;
		rasterization_state_create_info.frontFace = vk::FrontFace::eClockwise;

		// disable depth bias, we don't gaf about depth values now since we dont do anything with them
		rasterization_state_create_info.depthBiasEnable = vk::False;
		rasterization_state_create_info.depthBiasConstantFactor = 0.f;
		rasterization_state_create_info.depthBiasClamp = 0.f;
		rasterization_state_create_info.depthBiasSlopeFactor = 0.f;
	}

	// setup multisampling, we disable this since we don't care about it in our renderer
	vk::PipelineMultisampleStateCreateInfo multisample_state_create_info = {};
	{
		multisample_state_create_info.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo;
		multisample_state_create_info.sampleShadingEnable = vk::False;
		multisample_state_create_info.rasterizationSamples = vk::SampleCountFlagBits::e1;
		multisample_state_create_info.minSampleShading = 1.f;
		multisample_state_create_info.pSampleMask = nullptr;
		multisample_state_create_info.alphaToCoverageEnable = vk::False;
		multisample_state_create_info.alphaToOneEnable = vk::False;
	}

	// setup color blending
	std::array<vk::PipelineColorBlendAttachmentState, 1> color_blend_attachment_states = {};
	{
		{
			auto& color_blend_attachment_state = color_blend_attachment_states[0];

			color_blend_attachment_state.colorWriteMask =
			vk::ColorComponentFlagBits::eR |
			vk::ColorComponentFlagBits::eG |
			vk::ColorComponentFlagBits::eB |
			vk::ColorComponentFlagBits::eA;

			color_blend_attachment_state.blendEnable = vk::True;

			// how to do color blending
			color_blend_attachment_state.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
			color_blend_attachment_state.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
			color_blend_attachment_state.colorBlendOp = vk::BlendOp::eAdd;

			// how to do alpha blending
			color_blend_attachment_state.srcAlphaBlendFactor = vk::BlendFactor::eOne;
			color_blend_attachment_state.dstAlphaBlendFactor = vk::BlendFactor::eZero;
			color_blend_attachment_state.alphaBlendOp = vk::BlendOp::eAdd;
		}
	}

	vk::PipelineColorBlendStateCreateInfo color_blend_state_create_info = {};
	{
		color_blend_state_create_info.sType = vk::StructureType::ePipelineColorBlendStateCreateInfo;

		// disable bitwise combination of blending since we are using the attatchment method
		color_blend_state_create_info.logicOpEnable = vk::False;
		color_blend_state_create_info.logicOp = vk::LogicOp::eCopy; // Optional

		// use color attatchment blending
		color_blend_state_create_info.attachmentCount = color_blend_attachment_states.size();
		color_blend_state_create_info.pAttachments = color_blend_attachment_states.data();

		// we don't touch the constants
		color_blend_state_create_info.blendConstants[0] = 0.f; // Optional
		color_blend_state_create_info.blendConstants[1] = 0.f; // Optional
		color_blend_state_create_info.blendConstants[2] = 0.f; // Optional
		color_blend_state_create_info.blendConstants[3] = 0.f; // Optional
	}

	// create uniforms, needed even if we don't use any
	// todo: add uniforms we need
	vk::PipelineLayoutCreateInfo pipeline_layout_create_info = {};
	{
		pipeline_layout_create_info.sType = vk::StructureType::ePipelineLayoutCreateInfo;

		pipeline_layout_create_info.setLayoutCount = 0;
		pipeline_layout_create_info.pSetLayouts = nullptr;

		pipeline_layout_create_info.pushConstantRangeCount = 0;
		pipeline_layout_create_info.pPushConstantRanges = nullptr;
	}

	if (const auto result = _logical_device.createPipelineLayout(
		&pipeline_layout_create_info,
		nullptr,
		&_pipeline_layout);
		result != vk::Result::eSuccess)
	{
		lib_log_e("render_api: failed to create pipeline layout {}", static_cast<int>(result));
		assert(false);
	}

	assert(_pipeline_layout);

	// create the actual graphics pipeline
	vk::GraphicsPipelineCreateInfo pipeline_create_info = {};
	{
		pipeline_create_info.sType = vk::StructureType::eGraphicsPipelineCreateInfo;

		// frag/vertex shadeers
		pipeline_create_info.stageCount = shader_stage_create_infos.size();
		pipeline_create_info.pStages = shader_stage_create_infos.data();

		// fixed functions
		pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
		pipeline_create_info.pInputAssemblyState = &input_assembly_state_create_info;
		pipeline_create_info.pViewportState = &viewport_state_create_info;
		pipeline_create_info.pRasterizationState = &rasterization_state_create_info;
		pipeline_create_info.pMultisampleState = &multisample_state_create_info;
		pipeline_create_info.pColorBlendState = &color_blend_state_create_info;
		pipeline_create_info.pDynamicState = &dynamic_state_create_info;
		pipeline_create_info.pDepthStencilState = nullptr;

		// other descriptions of the render pipeline
		pipeline_create_info.layout = _pipeline_layout;

		pipeline_create_info.renderPass = _render_pass;
		pipeline_create_info.subpass = 0;

		// this is also our first graphics pipeline so we don't want to reference anything
		pipeline_create_info.basePipelineHandle = nullptr; // Optional
		pipeline_create_info.basePipelineIndex = -1; // Optional
	}

	if (const auto result = _logical_device.createGraphicsPipelines(
		nullptr,
		1,
		&pipeline_create_info,
		nullptr,
		&_pipeline);
		result != vk::Result::eSuccess)
	{
		lib_log_e("render_api: failed to create pipeline {}", static_cast<int>(result));
		assert(false);
	}

	_logical_device.destroyShaderModule(vertex_shader);
	_logical_device.destroyShaderModule(fragment_shader);

	assert(_pipeline);
	lib_log_d("render_api: created graphics pipeline");
}

void render_api::init_frame_buffers()
{
	// create frame buffer for each image view
	_swapchain_frame_buffers.resize(_swapchain_image_views.size());

	for (size_t i = 0; i < _swapchain_image_views.size(); i++)
	{
		const std::array<vk::ImageView, 1> attatchments =
		{
			_swapchain_image_views.at(i)
		};

		vk::FramebufferCreateInfo framebuffer_create_info = {};
		{
			framebuffer_create_info.sType = vk::StructureType::eFramebufferCreateInfo;
			framebuffer_create_info.renderPass = _render_pass;

			framebuffer_create_info.attachmentCount = attatchments.size();
			framebuffer_create_info.pAttachments = attatchments.data();

			framebuffer_create_info.width = _swapchain_extent.width;
			framebuffer_create_info.height = _swapchain_extent.height;

			framebuffer_create_info.layers = 1;
		}

		if (const auto result = _logical_device.createFramebuffer(
			&framebuffer_create_info,
			nullptr,
			&_swapchain_frame_buffers.at(i));
			result != vk::Result::eSuccess)
		{
			lib_log_e("render_api: failed to create frame buffer {}", static_cast<int>(result));
			assert(false);
		}

		assert(_swapchain_frame_buffers.at(i));
	}
}

void render_api::init_command_pool()
{
	vk::CommandPoolCreateInfo command_pool_create_info = {};
	{
		command_pool_create_info.sType = vk::StructureType::eCommandPoolCreateInfo;
		command_pool_create_info.flags =
			vk::CommandPoolCreateFlagBits::eResetCommandBuffer |
			vk::CommandPoolCreateFlagBits::eTransient;

		command_pool_create_info.queueFamilyIndex = _graphics_present_family_index.value();
	}

	if (const auto result = _logical_device.createCommandPool(
		&command_pool_create_info,
		nullptr,
		&_command_pool);
		result != vk::Result::eSuccess)
	{
		lib_log_e("render_api: failed to create command pool {}", static_cast<int>(result));
		assert(false);
	}

	assert(_command_pool);
	lib_log_d("render_api: created command pool");
}

void render_api::init_command_buffer()
{
	vk::CommandBufferAllocateInfo command_buffer_allocate_info = {};
	{
		command_buffer_allocate_info.sType = vk::StructureType::eCommandBufferAllocateInfo;
		command_buffer_allocate_info.commandPool = _command_pool;
		command_buffer_allocate_info.level = vk::CommandBufferLevel::ePrimary;
		command_buffer_allocate_info.commandBufferCount = vulkan::max_frames_in_flight;
	}

	if (const auto result = _logical_device.allocateCommandBuffers(
		&command_buffer_allocate_info,
		_command_buffers.data());
		result != vk::Result::eSuccess)
	{
		lib_log_e("render_api: failed to create command buffer {}", static_cast<int>(result));
		assert(false);
	}

	lib_log_d("render_api: created command buffer");
}

void render_api::record_command_buffer(const vk::CommandBuffer& command_buffer, uint32_t image_index) const
{
	vk::CommandBufferBeginInfo command_buffer_begin_info = {};
	{
		command_buffer_begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;

		command_buffer_begin_info.flags = {};
		command_buffer_begin_info.pInheritanceInfo = nullptr;
	}

	if (const auto result = command_buffer.begin(&command_buffer_begin_info);
		result != vk::Result::eSuccess)
	{
		lib_log_e("render_api: failed to begin command buffer {}", static_cast<int>(result));
		assert(false);
	}

	vk::ClearValue clear_color = {};
	{
		// clear color value
		clear_color.color = vk::ClearColorValue(0.f, 0.f, 0.f, 0.f);
	}

	vk::RenderPassBeginInfo render_pass_begin_info = {};
	{
		render_pass_begin_info.sType = vk::StructureType::eRenderPassBeginInfo;

		render_pass_begin_info.renderPass = _render_pass;
		render_pass_begin_info.framebuffer = _swapchain_frame_buffers.at(image_index);

		render_pass_begin_info.renderArea.offset = vk::Offset2D(0, 0);
		render_pass_begin_info.renderArea.extent = _swapchain_extent;

		render_pass_begin_info.clearValueCount = 1;
		render_pass_begin_info.pClearValues = &clear_color;
	}

	// no secondary cmd buffer so inline
	command_buffer.beginRenderPass(&render_pass_begin_info, vk::SubpassContents::eInline);
	command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _pipeline);

	// set viewport and scissor
	vk::Viewport viewport = {};
	{
		viewport.x = 0.f;
		viewport.y = 0.f;

		viewport.width = static_cast<float>(_swapchain_extent.width);
		viewport.height = static_cast<float>(_swapchain_extent.height);

		viewport.minDepth = 0.f;
		viewport.maxDepth = 0.f;
	}
	command_buffer.setViewport(0, 1, &viewport);

	vk::Rect2D scissor = {};
	{
		scissor.offset = vk::Offset2D(0, 0);
		scissor.extent = _swapchain_extent;
	}
	command_buffer.setScissor(0, 1, &scissor);

	// draw command buffer
	const std::array<vk::Buffer, 1> vertex_buffers = {
		_vertex_buffer
	};

	constexpr std::array<vk::DeviceSize, 1> vertex_buffer_offets = {
		0
	};

	command_buffer.bindVertexBuffers(0, vertex_buffers, vertex_buffer_offets);

	command_buffer.draw(vertex_buffer_test.size(), 1, 0, 0);
	command_buffer.endRenderPass();

	// finish recording the command buffer
	command_buffer.end();
}

void render_api::destroy_swapchain()
{
	std::ranges::for_each(_swapchain_frame_buffers.begin(), _swapchain_frame_buffers.end(),
		[&](const auto& frame_buffer) {
		_logical_device.destroyFramebuffer(frame_buffer);
	});

	std::ranges::for_each(_swapchain_image_views.begin(), _swapchain_image_views.end(),
		[&](const auto& image_view) {
		_logical_device.destroyImageView(image_view);
	});

	_logical_device.destroySwapchainKHR(_swap_chain);
}

void render_api::init_vertex_buffer()
{
	constexpr vk::DeviceSize buffer_size = sizeof(vertex_t) * MAX_VERTICES;

	// create staging buffer, used to write data from CPU to GPU
	vk::Buffer staging_buffer = nullptr;
	vk::DeviceMemory staging_buffer_memory = nullptr;

	create_buffer(
		_physical_device,
		_logical_device,
		buffer_size,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		staging_buffer,
		staging_buffer_memory);

	// map data into the buffer and write to it
	void* data = nullptr;
	assert(_logical_device.mapMemory(
		staging_buffer_memory,
		0,
		buffer_size,
		static_cast<vk::MemoryMapFlagBits>(0),
		&data) == vk::Result::eSuccess);

	// copy data into vertex buffer
	std::memcpy(data, vertex_buffer_test.data(), sizeof(vertex_buffer_test));
	_logical_device.unmapMemory(staging_buffer_memory);

	// create actual vertex buffer used by GPU
	create_buffer(
		_physical_device,
		_logical_device,
		buffer_size,
		vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		_vertex_buffer,
		_vertex_buffer_memory);

	// copy contents of cpu memory mapped buffer into device buffer
	copy_buffer(
		_logical_device,
		_command_pool,
		_graphics_present_queue,
		staging_buffer,
		_vertex_buffer,
		buffer_size);

	// destroy staging buffer
	_logical_device.destroyBuffer(staging_buffer);
	_logical_device.freeMemory(staging_buffer_memory);
}

void render_api::init_index_buffer()
{

}
