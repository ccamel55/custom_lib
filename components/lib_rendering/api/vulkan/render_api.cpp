#include <lib_rendering/render_api.hpp>

// include our shaders
#include <lib_rendering/shaders/basic_shader_vert.hpp>
#include <lib_rendering/shaders/normal_shader_frag.hpp>
#include <lib_rendering/shaders/sdf_shader_frag.hpp>
#include <lib_rendering/shaders/outline_shader_frag.hpp>

#include <unordered_set>
#include <ranges>
#include <map>
#include <optional>
#include <filesystem>
#include <core_sdk/types/vector/vector2D.hpp>

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
	"VK_KHR_dynamic_rendering"
};

const std::unordered_set<std::string> validation_layers =
{
#ifndef NDEBUG
	"VK_LAYER_KHRONOS_validation"
#endif
};

enum class image_transition_type
{
	undefined_to_transfer_dst_optimal,
	undefined_to_color_attatchment_optimal,

	transfer_dst_optimal_to_shader_read_only_optimal,
	color_attatchment_optimal_to_present_src,

	num_transition_types,
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
	const vk::SurfaceKHR& surface) -> vulkan::swapchain_support_details_t
{
	vulkan::swapchain_support_details_t details = {};
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

const auto create_texture = [](
	const vk::PhysicalDevice& physical_device,
	const vk::Device& device,
	uint32_t width,
	uint32_t height,
	vk::Format format,
	vk::ImageTiling tiling,
	vk::ImageUsageFlags usage_flags,
	vk::MemoryPropertyFlags memory_property_flags,
	vk::Image& image,
	vk::DeviceMemory& image_memory
)
{
	vk::ImageCreateInfo image_create_info = {};
	{
		image_create_info.sType = vk::StructureType::eImageCreateInfo;
		image_create_info.imageType = vk::ImageType::e2D;

		image_create_info.extent.width = width;
		image_create_info.extent.height = height;

		image_create_info.extent.depth = 1;
		image_create_info.mipLevels = 1;
		image_create_info.arrayLayers = 1;

		image_create_info.format = format;
		image_create_info.tiling = tiling;
		image_create_info.initialLayout = vk::ImageLayout::eUndefined;

		image_create_info.usage = usage_flags;
		image_create_info.sharingMode = vk::SharingMode::eExclusive;

		// multisampling stuff we wont be using
		image_create_info.samples = vk::SampleCountFlagBits::e1;
		image_create_info.flags = static_cast<vk::ImageCreateFlagBits>(0);
	}

	if (const auto result = device.createImage(
		&image_create_info,
		nullptr,
		&image); result != vk::Result::eSuccess)
	{
		lib_log_e("render_api: failed to create image {}", static_cast<int>(result));
		assert(false);
	}

	// create memory for image
	vk::MemoryRequirements memory_requirements = {};
	device.getImageMemoryRequirements(image, &memory_requirements);

	vk::MemoryAllocateInfo allocate_info = {};
	{
		allocate_info.sType = vk::StructureType::eMemoryAllocateInfo;
		allocate_info.allocationSize = memory_requirements.size;

		allocate_info.memoryTypeIndex = find_memory_type(
			physical_device,
			memory_property_flags,
			memory_requirements.memoryTypeBits
			);
	}

	if (const auto result = device.allocateMemory(
		&allocate_info,
		nullptr,
		&image_memory); result != vk::Result::eSuccess)
	{
		lib_log_e("render_api: failed to allocate texture memory {}", static_cast<int>(result));
		assert(false);
	}

	// bind image to memory
	device.bindImageMemory(image, image_memory, 0);
};

// synchronous command buffer used to initalize some things
const auto begin_command_buffer = [](
	const vk::Device& device,
	const vk::CommandPool& command_pool) -> vk::CommandBuffer
{
	vk::CommandBufferAllocateInfo allocate_info = {};
	{
		allocate_info.sType = vk::StructureType::eCommandBufferAllocateInfo;
		allocate_info.level = vk::CommandBufferLevel::ePrimary;

		allocate_info.commandPool = command_pool;
		allocate_info.commandBufferCount = 1;
	}

	vk::CommandBuffer command_buffer = {};
	assert(device.allocateCommandBuffers(&allocate_info, &command_buffer) == vk::Result::eSuccess);

	// start recording
	vk::CommandBufferBeginInfo buffer_begin_info = {};
	{
		buffer_begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;

		buffer_begin_info.flags = {};
		buffer_begin_info.pInheritanceInfo = nullptr;
	}

	assert(command_buffer.begin(&buffer_begin_info) == vk::Result::eSuccess);

	return command_buffer;
};

const auto end_and_submit_command_buffer = [](
	const vk::Device& device,
	const vk::CommandPool& command_pool,
	const vk::Queue& queue,
	const vk::CommandBuffer& command_buffer)
{
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

const auto copy_buffer_to_image = [](
	const vk::CommandBuffer& command_buffer,
	const vk::Buffer& buffer,
	const vk::Image& image,
	uint32_t width,
	uint32_t height)
{
	vk::BufferImageCopy region = {};
	{
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = vk::Offset3D(0, 0, 0);
		region.imageExtent = vk::Extent3D(width, height, 1);
	}

	command_buffer.copyBufferToImage(
		buffer,
		image,
		vk::ImageLayout::eTransferDstOptimal,
		1,
		&region);
};

const auto copy_buffer = [](
	const vk::CommandBuffer& command_buffer,
	const vk::Buffer& src_buffer,
	const vk::Buffer& dst_buffer,
	vk::DeviceSize size)
{
	// copy src into dst buffer
	vk::BufferCopy copy_region = {};
	{
		copy_region.srcOffset = 0;
		copy_region.dstOffset = 0;
		copy_region.size = size;
	}

	command_buffer.copyBuffer(src_buffer, dst_buffer, 1, &copy_region);
};

const auto transition_image_layout = [](
	const vk::CommandBuffer& command_buffer,
	const vk::Image& image,
	image_transition_type transition)
{
	vk::PipelineStageFlags source_stage = {};
	vk::PipelineStageFlags destination_stage = {};

	vk::ImageMemoryBarrier barrier = {};
	{
		barrier.sType = vk::StructureType::eImageMemoryBarrier;

		barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
		barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;

		barrier.image = image;
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;

		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;

		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		switch (transition)
		{
		case image_transition_type::undefined_to_transfer_dst_optimal:

			barrier.oldLayout = vk::ImageLayout::eUndefined;
			barrier.newLayout = vk::ImageLayout::eTransferDstOptimal;

			barrier.srcAccessMask = static_cast<vk::AccessFlags>(0);
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

			source_stage = vk::PipelineStageFlagBits::eTopOfPipe;
			destination_stage = vk::PipelineStageFlagBits::eTransfer;

			break;
		case image_transition_type::undefined_to_color_attatchment_optimal:

			barrier.oldLayout = vk::ImageLayout::eUndefined;
			barrier.newLayout = vk::ImageLayout::eColorAttachmentOptimal;

			barrier.srcAccessMask = static_cast<vk::AccessFlags>(0);
			barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

			source_stage = vk::PipelineStageFlagBits::eTopOfPipe;
			destination_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

			break;
		case image_transition_type::transfer_dst_optimal_to_shader_read_only_optimal:

			barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
			barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

			source_stage = vk::PipelineStageFlagBits::eTransfer;
			destination_stage = vk::PipelineStageFlagBits::eFragmentShader;

			break;
		case image_transition_type::color_attatchment_optimal_to_present_src:

			barrier.oldLayout = vk::ImageLayout::eColorAttachmentOptimal;
			barrier.newLayout = vk::ImageLayout::ePresentSrcKHR;

			barrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
			barrier.dstAccessMask = static_cast<vk::AccessFlags>(0);

			source_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
			destination_stage = vk::PipelineStageFlagBits::eBottomOfPipe;

			break;
		case image_transition_type::num_transition_types:
			assert(false);
		}
	}

	command_buffer.pipelineBarrier(
		source_stage, destination_stage,
		static_cast<vk::DependencyFlags>(0),
		0, nullptr,
		0, nullptr,
		1, &barrier);
};

const auto create_image_view = [](
	const vk::Device& device,
	const vk::Image& image,
	vk::Format format) -> vk::ImageView
{
	vk::ImageViewCreateInfo image_view_create_info = {};
	{
		image_view_create_info.sType = vk::StructureType::eImageViewCreateInfo;
		image_view_create_info.image = image;
		image_view_create_info.viewType = vk::ImageViewType::e2D;
		image_view_create_info.format = format;

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

	vk::ImageView image_view = {};

	if (const auto result = device.createImageView(
		&image_view_create_info,
		nullptr,
		&image_view);
		result != vk::Result::eSuccess)
	{
		lib_log_e("render_api: failed to create image view {}", static_cast<int>(result));
		assert(false);
	}

	return image_view;
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
	init_descriptor_set_layout();
	init_graphics_pipeline();
	init_command_pool();
	init_vertex_buffer();
	init_index_buffer();
	init_descriptor_pool();
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
	destroy_image();

	_logical_device.destroySampler(_texture_sampler);
	_logical_device.destroyDescriptorPool(_descriptor_pool);
	_logical_device.destroyDescriptorSetLayout(_descriptor_set_layout);

	_logical_device.unmapMemory(_vertex_staging_buffer_memory);
	_logical_device.unmapMemory(_index_staging_buffer_memory);

	_logical_device.destroyBuffer(_vertex_staging_buffer);
	_logical_device.freeMemory(_vertex_staging_buffer_memory);

	_logical_device.destroyBuffer(_vertex_buffer);
	_logical_device.freeMemory(_vertex_buffer_memory);

	_logical_device.destroyBuffer(_index_staging_buffer);
	_logical_device.freeMemory(_index_staging_buffer_memory);

	_logical_device.destroyBuffer(_index_buffer);
	_logical_device.freeMemory(_index_buffer_memory);

	_logical_device.destroyPipeline(_pipeline);
	_logical_device.destroyPipelineLayout(_pipeline_layout);

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
	// remove pre existing texture atlas if needed
	if (_init_texture)
	{
		_logical_device.waitIdle();
		destroy_image();
	}

	_init_texture = true;
	const vk::DeviceSize buffer_size = width * height * texture_pixel_size;

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

	// copy data into staging buffer
	void* staging_buffer_mapped = nullptr;
	assert(_logical_device.mapMemory(
		staging_buffer_memory,
		0,
		buffer_size,
		static_cast<vk::MemoryMapFlagBits>(0),
		&staging_buffer_mapped) == vk::Result::eSuccess);

	std::memcpy(staging_buffer_mapped, data, buffer_size);
	_logical_device.unmapMemory(staging_buffer_memory);

	create_texture(
		_physical_device,
		_logical_device,
		width,
		height,
		vk::Format::eR8G8B8A8Srgb,
		vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		_texture_atlas,
		_texture_atlas_memory);

	const auto command_buffer = begin_command_buffer(
		_logical_device,
		_command_pool);

	transition_image_layout(
		command_buffer,
		_texture_atlas,
		image_transition_type::undefined_to_transfer_dst_optimal);

	copy_buffer_to_image(
		command_buffer,
		staging_buffer,
		_texture_atlas,
		width,
		height);

	transition_image_layout(
		command_buffer,
		_texture_atlas,
		image_transition_type::transfer_dst_optimal_to_shader_read_only_optimal);

	end_and_submit_command_buffer(
		_logical_device,
		_command_pool,
		_graphics_present_queue,
		command_buffer);

	_logical_device.destroyBuffer(staging_buffer);
	_logical_device.freeMemory(staging_buffer_memory);

	// both are related to textures, our descriptor set can be used for other things but
	// we don't have anything else to use it with right now
	init_texture_view();
	init_texture_sampler();

	init_descriptor_set();
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

	_viewport.x = 0.f;
	_viewport.y = 0.f;

	_viewport.width = static_cast<float>(_swapchain_extent.width);
	_viewport.height = static_cast<float>(_swapchain_extent.height);

	_viewport.minDepth = 0.f;
	_viewport.maxDepth = 0.f;

	// thank you imgui, I love you
	_push_constants.scale.x = 2.f / static_cast<float>(_swapchain_extent.width);
	_push_constants.scale.y = 2.f / static_cast<float>(_swapchain_extent.height);

	// consider changing if you want to offset the "projection matrix"
	_push_constants.translate.x = -1.f;
	_push_constants.translate.y = -1.f;
}

void render_api::update_frame_buffer(const render_command& render_command)
{
	if (_stop_rendering)
	{
		return;
	}

	// wait for gpu to finish drawing previous frame
	(void)_logical_device.waitForFences(
		1,
		&_in_flight_fences.at(_current_frame),
		vk::True, UINT64_MAX);

	(void)_logical_device.resetFences(1, &_in_flight_fences.at(_current_frame));

	// aquire an image from the swap chain
	uint32_t next_image_index = 0;
	(void)_logical_device.acquireNextImageKHR(
		_swap_chain,
		UINT64_MAX,
		_image_available_semaphores.at(_current_frame),
		nullptr,
		&next_image_index);

	// reset command buffer and render what we want
	_command_buffers.at(_current_frame).reset();
	record_command_buffer(_command_buffers.at(_current_frame), next_image_index, _current_frame, render_command);

	// submit command buffer
	constexpr std::array<vk::PipelineStageFlags, 1> wait_stages = {
		vk::PipelineStageFlagBits::eColorAttachmentOutput
	};

	vk::SubmitInfo submit_info = {};
	{
		submit_info.sType = vk::StructureType::eSubmitInfo;

		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = &_image_available_semaphores.at(_current_frame);

		submit_info.pWaitDstStageMask = wait_stages.data();

		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &_command_buffers.at(_current_frame);

		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &_render_finished_semaphores.at(_current_frame);
	}

	(void)_graphics_present_queue.submit(
		1,
		&submit_info,
		_in_flight_fences.at(_current_frame));

	vk::PresentInfoKHR present_info = {};
	{
		present_info.sType = vk::StructureType::ePresentInfoKHR;

		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = &_render_finished_semaphores.at(_current_frame);

		present_info.swapchainCount = 1;
		present_info.pSwapchains = &_swap_chain;

		present_info.pImageIndices = &next_image_index;
		present_info.pResults = nullptr;
	}

	(void)_graphics_present_queue.presentKHR(&present_info);
	_current_frame = (_current_frame + 1) % vulkan::max_frames_in_flight;
}

void render_api::draw_frame_buffer()
{
	if (_stop_rendering)
	{
		return;
	}
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
		// we dont use any device features right now so leave empty
	}

	vk::PhysicalDeviceDynamicRenderingFeatures dynamic_rendering_features = {};
	{
		dynamic_rendering_features.sType = vk::StructureType::ePhysicalDeviceDynamicRenderingFeatures;
		dynamic_rendering_features.dynamicRendering = true;
	}

	vk::DeviceCreateInfo create_info = {};
	{
		create_info.sType = vk::StructureType::eDeviceCreateInfo;
		create_info.pNext = &dynamic_rendering_features;

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
		_swapchain_image_views.at(i) = create_image_view(
			_logical_device,
			_swapchain_images.at(i),
			_swapchian_format);
	}

	lib_log_w("render_api: created swapchain image view");
	assert(!_swapchain_image_views.empty());
}

void render_api::init_descriptor_set_layout()
{
	std::array<vk::DescriptorSetLayoutBinding, 1> layout_bindings = {};
	{
		// sampler layout
		{
			auto& layout_binding = layout_bindings.at(0);

			// binding slot 0
			layout_binding.binding = 0;

			layout_binding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
			layout_binding.descriptorCount = 1;

			// only apply for vertex shaders
			layout_binding.stageFlags = vk::ShaderStageFlagBits::eFragment;
			layout_binding.pImmutableSamplers = nullptr;
		}
	}

	vk::DescriptorSetLayoutCreateInfo create_info = {};
	{
		create_info.sType = vk::StructureType::eDescriptorSetLayoutCreateInfo;

		create_info.bindingCount = layout_bindings.size();
		create_info.pBindings = layout_bindings.data();
	}

	if (const auto result = _logical_device.createDescriptorSetLayout(
		&create_info,
		nullptr,
		&_descriptor_set_layout);
		result != vk::Result::eSuccess)
	{
		lib_log_e("render_api: failed to create descriptor set layout {}", static_cast<int>(result));
		assert(false);
	}
}


void render_api::init_graphics_pipeline()
{
	// setup programmable parts of the pipeline

	// shader modules can be nuked after creating pipeline
	const auto vertex_shader = create_shader_module(
		_logical_device,
		vulkan::shaders::basic_shader_vert,
		vulkan::shaders::basic_shader_vert_length);

	const auto fragment_shader_normal = create_shader_module(_logical_device,
		vulkan::shaders::normal_shader_frag,
		vulkan::shaders::normal_shader_frag_length);

	// unused for now ok?
	const auto fragment_shader_sdf = create_shader_module(_logical_device,
		vulkan::shaders::sdf_shader_frag,
		vulkan::shaders::sdf_shader_frag_length);

	const auto fragment_shader_outline = create_shader_module(_logical_device,
		vulkan::shaders::outline_shader_frag,
		vulkan::shaders::outline_shader_frag_length);

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
			fragment_stage_create_info.module = fragment_shader_outline;

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

	// setup push constants
	vk::PushConstantRange push_constant_range = {};
	{
		push_constant_range.stageFlags = vk::ShaderStageFlagBits::eVertex;

		push_constant_range.offset = 0;
		push_constant_range.size = sizeof(vulkan::push_constants_t);
	}

	vk::PipelineLayoutCreateInfo pipeline_layout_create_info = {};
	{
		pipeline_layout_create_info.sType = vk::StructureType::ePipelineLayoutCreateInfo;

		pipeline_layout_create_info.setLayoutCount = 1;
		pipeline_layout_create_info.pSetLayouts = &_descriptor_set_layout;

		pipeline_layout_create_info.pushConstantRangeCount = 1;
		pipeline_layout_create_info.pPushConstantRanges = &push_constant_range;
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

	vk::PipelineRenderingCreateInfo pipeline_rendering_create_info = {};
	{
		pipeline_rendering_create_info.sType = vk::StructureType::ePipelineRenderingCreateInfo;

		pipeline_rendering_create_info.colorAttachmentCount = 1;
		pipeline_rendering_create_info.pColorAttachmentFormats = &_swapchian_format;
	}

	// create the actual graphics pipeline
	vk::GraphicsPipelineCreateInfo pipeline_create_info = {};
	{
		pipeline_create_info.sType = vk::StructureType::eGraphicsPipelineCreateInfo;
		pipeline_create_info.pNext = &pipeline_rendering_create_info;

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

		pipeline_create_info.renderPass = nullptr;
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
	_logical_device.destroyShaderModule(fragment_shader_normal);
	_logical_device.destroyShaderModule(fragment_shader_sdf);
	_logical_device.destroyShaderModule(fragment_shader_outline);

	assert(_pipeline);
	lib_log_d("render_api: created graphics pipeline");
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

void render_api::record_command_buffer(
	const vk::CommandBuffer& command_buffer,
	uint32_t image_index,
	uint32_t current_frame,
	const render_command& render_command) const
{
	const auto vertex_buffer_size = sizeof(vertex_t) * render_command.vertex_count;
	const auto index_buffer_size = sizeof(uint32_t) * render_command.index_count;

	// copy data into vertex and index buffer
	std::memcpy(_vertex_staging_buffer_mapped, render_command.vertices.data(), vertex_buffer_size);
	std::memcpy(_index_staging_buffer_mapped, render_command.indices.data(), index_buffer_size);

	vk::CommandBufferBeginInfo command_buffer_begin_info = {};
	{
		command_buffer_begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;

		command_buffer_begin_info.flags = {};
		command_buffer_begin_info.pInheritanceInfo = nullptr;
	}

	(void)command_buffer.begin(&command_buffer_begin_info);

	copy_buffer(
		command_buffer,
		_vertex_staging_buffer,
		_vertex_buffer,
		vertex_buffer_size);

	copy_buffer(
		command_buffer,
		_index_staging_buffer,
		_index_buffer,
		index_buffer_size);

	transition_image_layout(
		command_buffer,
		_swapchain_images.at(image_index),
		image_transition_type::undefined_to_color_attatchment_optimal);

	vk::ClearValue clear_color = {};
	{
		// clear color value
		clear_color.color = vk::ClearColorValue(0.f, 0.f, 0.f, 0.f);
	}

	vk::RenderingAttachmentInfo color_attatchment_info = {};
	{
		color_attatchment_info.sType = vk::StructureType::eRenderingAttachmentInfo;

		color_attatchment_info.imageView = _swapchain_image_views.at(image_index);
		color_attatchment_info.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;

		color_attatchment_info.loadOp = vk::AttachmentLoadOp::eClear;
		color_attatchment_info.storeOp = vk::AttachmentStoreOp::eStore;

		color_attatchment_info.clearValue = clear_color;
	}

	vk::RenderingInfo rendering_info = {};
	{
		rendering_info.sType = vk::StructureType::eRenderingInfo;

		rendering_info.renderArea.offset = vk::Offset2D(0, 0);
		rendering_info.renderArea.extent = _swapchain_extent;

		rendering_info.layerCount = 1;

		rendering_info.colorAttachmentCount = 1;
		rendering_info.pColorAttachments = &color_attatchment_info;
	}

	// no secondary cmd buffer so inline
	command_buffer.beginRendering(&rendering_info);
	command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _pipeline);

	const std::array<vk::Buffer, 1> vertex_buffers = {
		_vertex_buffer
	};

	constexpr std::array<vk::DeviceSize, 1> vertex_buffer_offets = {
		0
	};

	command_buffer.bindVertexBuffers(0, vertex_buffers, vertex_buffer_offets);
	command_buffer.bindIndexBuffer(_index_buffer, 0, vk::IndexType::eUint32);

	// set viewport
	command_buffer.setViewport(0, 1, &_viewport);

	command_buffer.pushConstants(
		_pipeline_layout,
		vk::ShaderStageFlagBits::eVertex,
		0,
		sizeof(vulkan::push_constants_t),
		&_push_constants);

	command_buffer.bindDescriptorSets(
		vk::PipelineBindPoint::eGraphics,
		_pipeline_layout,
		0,
		1,
		&_descriptor_set.at(current_frame),
		0,
		nullptr);

	// draw command buffer
	for (uint32_t i = 0; i < render_command.batch_count; i++)
	{
		const auto& batch = render_command.batches.at(i);

		// switch (batch.shader)
		// {
		// case shader_type::normal:
		// 	_normal_shader.bind();
		// 	break;
		// case shader_type::sdf:
		// 	_sdf_shader.bind();
		// 	break;
		// case shader_type::sdf_outline:
		// 	_sdf_outline_shader.bind();
		// 	break;
		// }

		vk::Rect2D scissor = {};
		{
			scissor.offset = vk::Offset2D(batch.clipped_area.x, batch.clipped_area.y);
			scissor.extent = vk::Extent2D(batch.clipped_area.z, batch.clipped_area.w);
		}
		command_buffer.setScissor(0, 1, &scissor);
		command_buffer.drawIndexed(
			batch.count,
			1,
			batch.offset,
			0,
			0);
	}

	// finish recording the command buffer
	command_buffer.endRendering();

	transition_image_layout(
		command_buffer,
		_swapchain_images.at(image_index),
		image_transition_type::color_attatchment_optimal_to_present_src);

	command_buffer.end();
}

void render_api::init_descriptor_pool()
{
	std::array<vk::DescriptorPoolSize, 1> pool_sizes = {};
	{
		// sampler pool size
		{
			auto& pool_size = pool_sizes.at(0);

			pool_size.type = vk::DescriptorType::eCombinedImageSampler;
			pool_size.descriptorCount = vulkan::max_frames_in_flight;
		}
	}

	vk::DescriptorPoolCreateInfo create_info = {};
	{
		create_info.sType = vk::StructureType::eDescriptorPoolCreateInfo;

		create_info.poolSizeCount = pool_sizes.size();
		create_info.pPoolSizes = pool_sizes.data();

		create_info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
		create_info.maxSets = vulkan::max_frames_in_flight;
	}

	if (const auto result = _logical_device.createDescriptorPool(
		&create_info,
		nullptr,
		&_descriptor_pool);
		result != vk::Result::eSuccess)
	{
		lib_log_e("render_api: failed to create descriptor pool {}", static_cast<int>(result));
		assert(false);
	}
}

void render_api::init_descriptor_set()
{
	std::array<vk::DescriptorSetLayout, vulkan::max_frames_in_flight> layouts = {};
	std::ranges::fill(layouts.begin(), layouts.end(), _descriptor_set_layout);

	vk::DescriptorSetAllocateInfo allocate_info = {};
	{
		allocate_info.sType = vk::StructureType::eDescriptorSetAllocateInfo;
		allocate_info.descriptorPool = _descriptor_pool;

		allocate_info.descriptorSetCount = layouts.size();
		allocate_info.pSetLayouts = layouts.data();
	}

	if (const auto result = _logical_device.allocateDescriptorSets(
		&allocate_info,
		_descriptor_set.data());
		result != vk::Result::eSuccess)
	{
		lib_log_e("render_api: failed to allocate descriptor sets {}", static_cast<int>(result));
		assert(false);
	}

	// populate each descriptor set
	for (size_t i = 0; i < vulkan::max_frames_in_flight; i++)
	{
		// smapler descriptor buffer
		vk::DescriptorImageInfo image_info = {};
		{
			image_info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

			image_info.imageView = _texture_atlas_view;
			image_info.sampler = _texture_sampler;
		}

		std::array<vk::WriteDescriptorSet, 1> descriptor_writes = {};
		{
			// sampler write
			{
				auto& descriptor_write = descriptor_writes.at(0);

				descriptor_write.dstSet = _descriptor_set.at(i);
				descriptor_write.dstBinding = 0;
				descriptor_write.dstArrayElement = 0;

				descriptor_write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
				descriptor_write.descriptorCount = 1;
				descriptor_write.pImageInfo = &image_info;
			}
		}

		_logical_device.updateDescriptorSets(
			descriptor_writes.size(), descriptor_writes.data(),
			0, nullptr);
	}
}

void render_api::init_texture_view()
{
	_texture_atlas_view = create_image_view(
		_logical_device,
		_texture_atlas,
		vk::Format::eR8G8B8A8Srgb);
}

void render_api::init_texture_sampler()
{
	vk::PhysicalDeviceProperties device_properties = {};
	_physical_device.getProperties(&device_properties);

	vk::SamplerCreateInfo create_info = {};
	{
		create_info.sType = vk::StructureType::eSamplerCreateInfo;

		create_info.magFilter = vk::Filter::eLinear;
		create_info.minFilter = vk::Filter::eLinear;

		create_info.addressModeU = vk::SamplerAddressMode::eRepeat;
		create_info.addressModeV = vk::SamplerAddressMode::eRepeat;
		create_info.addressModeW = vk::SamplerAddressMode::eRepeat;

		create_info.anisotropyEnable = vk::False;
		create_info.maxAnisotropy = 1.f;

		create_info.borderColor = vk::BorderColor::eIntTransparentBlack;
		create_info.unnormalizedCoordinates = vk::False;

		create_info.compareEnable = vk::False;
		create_info.compareOp = vk::CompareOp::eAlways;

		create_info.mipmapMode = vk::SamplerMipmapMode::eLinear;
		create_info.mipLodBias = 0.0f;
		create_info.minLod = 0.0f;
		create_info.maxLod = 0.0f;
	}

	if (const auto result = _logical_device.createSampler(
		&create_info,
		nullptr,
		&_texture_sampler);
		result != vk::Result::eSuccess)
	{
		lib_log_e("render_api: failed to create texture sampler {}", static_cast<int>(result));
		assert(false);
	}
}

void render_api::destroy_swapchain()
{
	std::ranges::for_each(_swapchain_image_views.begin(), _swapchain_image_views.end(),
		[&](const auto& image_view) {
		_logical_device.destroyImageView(image_view);
	});

	_logical_device.destroySwapchainKHR(_swap_chain);
}

void render_api::destroy_image()
{
	assert(_logical_device.freeDescriptorSets(
		_descriptor_pool,
		_descriptor_set.size(),
		_descriptor_set.data()) == vk::Result::eSuccess);

	_logical_device.destroyImageView(_texture_atlas_view);
	_logical_device.destroyImage(_texture_atlas);
	_logical_device.freeMemory(_texture_atlas_memory);

	_init_texture = false;
}

void render_api::init_vertex_buffer()
{
	constexpr vk::DeviceSize buffer_size = sizeof(vertex_t) * MAX_VERTICES;

	// create staging buffer, used to write data from CPU to GPU
	create_buffer(
		_physical_device,
		_logical_device,
		buffer_size,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		_vertex_staging_buffer,
		_vertex_staging_buffer_memory);

	assert(_logical_device.mapMemory(
		_vertex_staging_buffer_memory,
		0,
		sizeof(vertex_t) * MAX_VERTICES,
		static_cast<vk::MemoryMapFlagBits>(0),
		&_vertex_staging_buffer_mapped) == vk::Result::eSuccess);

	// create actual vertex buffer used by GPU
	create_buffer(
		_physical_device,
		_logical_device,
		buffer_size,
		vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		_vertex_buffer,
		_vertex_buffer_memory);
}

void render_api::init_index_buffer()
{
	constexpr vk::DeviceSize buffer_size = sizeof(uint32_t) * MAX_INDICES;

	create_buffer(
		_physical_device,
		_logical_device,
		buffer_size,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		_index_staging_buffer,
		_index_staging_buffer_memory);

	assert(_logical_device.mapMemory(
		_index_staging_buffer_memory,
		0,
		sizeof(uint32_t) * MAX_INDICES,
		static_cast<vk::MemoryMapFlagBits>(0),
		&_index_staging_buffer_mapped) == vk::Result::eSuccess);

	// create actual index buffer used by GPU
	create_buffer(
		_physical_device,
		_logical_device,
		buffer_size,
		vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		_index_buffer,
		_index_buffer_memory);
}