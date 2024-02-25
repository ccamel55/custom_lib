#include <lib_rendering/render_api.hpp>
#include <core_sdk/types/vector/vector2D.hpp>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

// include our shaders
#include <lib_rendering/shaders/basic_shader_vert.hpp>
#include <lib_rendering/shaders/normal_shader_frag.hpp>
#include <lib_rendering/shaders/sdf_shader_frag.hpp>
#include <lib_rendering/shaders/outline_shader_frag.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <ranges>
#include <filesystem>

using namespace lib::rendering;

namespace
{
enum class image_transition_type
{
	undefined_to_transfer_dst_optimal,
	transfer_dst_optimal_to_shader_read_only_optimal,

	num_transition_types,
};

const auto query_swapchain_support = [](
	const vk::PhysicalDevice& device,
	const vk::SurfaceKHR& surface) -> vulkan::swapchain_support_details_t
{
	vulkan::swapchain_support_details_t details = {};
	device.getSurfaceCapabilitiesKHR(surface, &details.capabilities);

	uint32_t format_count = 0;
	device.getSurfaceFormatsKHR(surface, &format_count, nullptr);

	details.formats.resize(format_count);
	device.getSurfaceFormatsKHR(surface, &format_count, details.formats.data());

	uint32_t present_mode_couunt = 0;
	device.getSurfacePresentModesKHR(surface, &present_mode_couunt, nullptr);

	details.present_modes.resize(present_mode_couunt);
	device.getSurfacePresentModesKHR(surface, &present_mode_couunt, details.present_modes.data());

	return details;
};

const auto choose_swapchain_format = [](
	const std::vector<vk::SurfaceFormatKHR>& surface_formats) -> vk::SurfaceFormatKHR
{
	// we want to use SRGB since thats kinda the standard
	for (const auto& format: surface_formats)
	{
		if (format.format != vk::Format::eB8G8R8A8Unorm)
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
	const std::vector<vk::PresentModeKHR>& present_modes,
	const std::vector<vk::PresentModeKHR>& desired_modes) -> vk::PresentModeKHR
{
	for (const auto desired_mode: desired_modes)
	{
		if (desired_mode == vk::PresentModeKHR::eFifo)
		{
			continue;
		}

		for (const auto present_mode: present_modes)
		{
			if (present_mode == desired_mode)
			{
				return present_mode;
			}
		}
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

	return shader_module;
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
			pos_description.format = vk::Format::eR32G32B32Sfloat;
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
	device.allocateCommandBuffers(&allocate_info, &command_buffer);

	// start recording
	vk::CommandBufferBeginInfo buffer_begin_info = {};
	{
		buffer_begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;

		buffer_begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
		buffer_begin_info.pInheritanceInfo = nullptr;
	}

	(void)command_buffer.begin(&buffer_begin_info);
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

	(void)queue.submit(1, &submit_info, nullptr);
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
		case image_transition_type::transfer_dst_optimal_to_shader_read_only_optimal:

			barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
			barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

			source_stage = vk::PipelineStageFlagBits::eTransfer;
			destination_stage = vk::PipelineStageFlagBits::eFragmentShader;

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

render_api::render_api(const std::weak_ptr<render_api_data_t>& render_api_data, bool flush_buffers)
	: render_api_base(flush_buffers)
	, _render_api_data(render_api_data)
{
	const auto api_data = _render_api_data.lock();

	// setup vma allocator
	VmaAllocatorCreateInfo allocation_create_info = {};
	{
		allocation_create_info.vulkanApiVersion = api_data->vulkan_api_version;
		allocation_create_info.flags = VMA_ALLOCATOR_CREATE_EXTERNALLY_SYNCHRONIZED_BIT;

		allocation_create_info.physicalDevice = api_data->physical_device;
		allocation_create_info.device = api_data->device;
		allocation_create_info.instance = api_data->instance;

		allocation_create_info.pHeapSizeLimit = nullptr;
		allocation_create_info.pVulkanFunctions = nullptr;
		allocation_create_info.pTypeExternalMemoryHandleTypes = nullptr;
	}

	vmaCreateAllocator(&allocation_create_info, &_vk_allocator);

	init_swapcahin();
	init_image_views();
	init_render_pass();
	init_descriptor_set_layout();
	init_graphics_pipeline();
	init_frame_buffer();
	init_command_pool();
	init_vertex_buffer();
	init_index_buffer();
	init_uniform_buffer();
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
		if (api_data->device.createSemaphore(
			&semaphore_create_info,
			nullptr,
			&_image_available_semaphores.at(i)) != vk::Result::eSuccess ||
		api_data->device.createSemaphore(
			&semaphore_create_info,
			nullptr,
			&_render_finished_semaphores.at(i)) != vk::Result::eSuccess ||
		api_data->device.createFence(
			&fence_create_info,
			nullptr,
			&_in_flight_fences.at(i)) != vk::Result::eSuccess)
		{
			lib_log_e("render_api: failed to create synchronization objects");
			assert(false);
		}
	}

	// setup pipeline, call to update_screen_size will init the swapchain, framebuffer and image view
	update_screen_size(default_window_size);
}

render_api::~render_api()
{
	const auto api_data = _render_api_data.lock();

	api_data->device.waitIdle();

	destroy_swapchain();
	destroy_image();

	for (size_t i = 0; i < vulkan::max_frames_in_flight; i++)
	{
		vmaDestroyBuffer(_vk_allocator, _uniform_buffer.at(i), _unifrom_buffer_alloc.at(i));
	}

	api_data->device.destroySampler(_texture_sampler);
	api_data->device.destroyDescriptorPool(_descriptor_pool);
	api_data->device.destroyDescriptorSetLayout(_descriptor_set_layout);
	api_data->device.destroyRenderPass(_render_pass);

	vmaDestroyBuffer(_vk_allocator, _vertex_staging_buffer, _vertex_staging_buffer_alloc);
	vmaDestroyBuffer(_vk_allocator, _index_staging_buffer, _index_staging_buffer_alloc);

	vmaDestroyBuffer(_vk_allocator, _vertex_buffer, _vertex_buffer_alloc);
	vmaDestroyBuffer(_vk_allocator, _index_buffer, _index_buffer_alloc);

	api_data->device.destroyPipeline(_normal_pipeline);
	api_data->device.destroyPipeline(_sdf_pipeline);
	api_data->device.destroyPipeline(_outline_pipeline);

	api_data->device.destroyPipelineLayout(_pipeline_layout);

	for (size_t i = 0; i < vulkan::max_frames_in_flight; i++)
	{
		api_data->device.destroySemaphore(_image_available_semaphores.at(i));
		api_data->device.destroySemaphore(_render_finished_semaphores.at(i));
		api_data->device.destroyFence(_in_flight_fences.at(i));
	}

	api_data->device.destroyCommandPool(_command_pool);
	vmaDestroyAllocator(_vk_allocator);
}

void render_api::bind_atlas(const uint8_t* data, int width, int height)
{
	const auto api_data = _render_api_data.lock();

	// remove pre existing texture atlas if needed
	if (_init_texture)
	{
		api_data->device.waitIdle();
		destroy_image();
	}

	_init_texture = true;
	const vk::DeviceSize buffer_size = width * height * texture_pixel_size;

	VkBuffer staging_buffer = nullptr;
	VmaAllocation staging_buffer_alloc = nullptr;
	VmaAllocationInfo staging_buffer_alloc_info = {};

	VkBufferCreateInfo buffer_create_info = {};
	{
		buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_create_info.size = buffer_size;

		buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	VmaAllocationCreateInfo buffer_allocation_info = {};
	{
		buffer_allocation_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
		buffer_allocation_info.flags =
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
			VMA_ALLOCATION_CREATE_MAPPED_BIT;
	}

	vmaCreateBuffer(
		_vk_allocator,
		&buffer_create_info,
		&buffer_allocation_info,
		&staging_buffer,
		&staging_buffer_alloc,
		&staging_buffer_alloc_info);

	// copy data into staging buffer
	std::memcpy(staging_buffer_alloc_info.pMappedData, data, buffer_size);

	VkImageCreateInfo image_create_info = {};
	{
		image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_create_info.imageType = VK_IMAGE_TYPE_2D;

		image_create_info.extent.width = width;
		image_create_info.extent.height = height;

		image_create_info.extent.depth = 1;
		image_create_info.mipLevels = 1;
		image_create_info.arrayLayers = 1;

		image_create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
		image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		image_create_info.usage = 	VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		// multisampling stuff we wont be using
		image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_create_info.flags = 0;
	}

	VmaAllocationCreateInfo image_allocation_info = {};
	{
		buffer_allocation_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
	}

	vmaCreateImage(
		_vk_allocator,
		&image_create_info,
		&image_allocation_info,
		&_texture_atlas,
		&_texture_atlas_alloc,
		nullptr
		);

	const auto command_buffer = begin_command_buffer(api_data->device, _command_pool);

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
		api_data->device,
		_command_pool,
		api_data->queue,
		command_buffer);

	vmaDestroyBuffer(_vk_allocator, staging_buffer, staging_buffer_alloc);

	// both are related to textures, our descriptor set can be used for other things but
	// we don't have anything else to use it with right now
	init_texture_view();
	init_texture_sampler();

	init_descriptor_set();
}

void render_api::update_screen_size(const lib::point2Di& window_size)
{
	const auto api_data = _render_api_data.lock();

	_window_size = window_size;
	_stop_rendering = (_window_size.x == 0 || _window_size.y == 0);

	if (_stop_rendering)
	{
		return;
	}

	api_data->device.waitIdle();
	destroy_swapchain();

	init_swapcahin();
	init_image_views();
	init_frame_buffer();

	_viewport.x = 0.f;
	_viewport.y = 0.f;

	_viewport.width = static_cast<float>(_swapchain_extent.width);
	_viewport.height = static_cast<float>(_swapchain_extent.height);

	_viewport.minDepth = 0.f;
	_viewport.maxDepth = 1.f;

	// update projection and view matrix, model matrix can be instance speicifc
	_unifrom_buffer_object.projection_matrix = glm::ortho(
		0.f,
		static_cast<float>(window_size.x),
		// vulkan has y axis flipped, hence why bottom and top are swapped here :P
		0.f,
		static_cast<float>(window_size.y));

	// load identity matrix for now, we can fuck with this later
	_unifrom_buffer_object.view_matrix = glm::mat4(1.f);
}

void render_api::draw(const render_command& render_command)
{
	const auto api_data = _render_api_data.lock();

	if (_stop_rendering)
	{
		return;
	}

	// wait for gpu to finish drawing previous frame
	api_data->device.waitForFences(
		1,
		&_in_flight_fences.at(_current_frame),
		vk::True, UINT64_MAX);

	api_data->device.resetFences(1, &_in_flight_fences.at(_current_frame));

	// aquire an image from the swap chain
	uint32_t next_image_index = 0;
	api_data->device.acquireNextImageKHR(
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

	api_data->queue.submit(
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

	(void)api_data->queue.presentKHR(&present_info);
	_current_frame = (_current_frame + 1) % vulkan::max_frames_in_flight;
}

void render_api::init_swapcahin()
{
	const auto api_data = _render_api_data.lock();

	const auto swap_chain_support = query_swapchain_support(api_data->physical_device, api_data->surface);

	const auto surface_format = choose_swapchain_format(swap_chain_support.formats);
	const auto present_mode = choose_swapchain_present_mode(
		swap_chain_support.present_modes,
		{vk::PresentModeKHR::eMailbox, vk::PresentModeKHR::eImmediate});

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
		swapchain_create_info.surface = api_data->surface;

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

	if (const auto result = api_data->device.createSwapchainKHR(
		&swapchain_create_info,
		nullptr,
		&_swap_chain);
		result != vk::Result::eSuccess)
	{
		lib_log_e("render_api: failed to create swap chain {}", static_cast<int>(result));
		assert(false);
	}

	lib_log_d("render_api: created swap chain");

	// get swap chain images
	image_count = 0;
    api_data->device.getSwapchainImagesKHR(_swap_chain, &image_count, nullptr);

	_swapchain_images.resize(image_count);
	api_data->device.getSwapchainImagesKHR(_swap_chain, &image_count, _swapchain_images.data());

	// save states for use late
	_swapchian_format = surface_format.format;
	_swapchain_extent = extent;
}

void render_api::init_image_views()
{
	const auto api_data = _render_api_data.lock();
	_swapchain_image_views.resize(_swapchain_images.size());

	for (size_t i = 0; i < _swapchain_images.size(); i++)
	{
		_swapchain_image_views.at(i) = create_image_view(
			api_data->device,
			_swapchain_images.at(i),
			_swapchian_format);
	}

	lib_log_d("render_api: created swapchain image view");
}

void render_api::init_descriptor_set_layout()
{
	const auto api_data = _render_api_data.lock();

	std::array<vk::DescriptorSetLayoutBinding, 2> layout_bindings = {};
	{
		// ubo layout
		{
			auto& layout_binding = layout_bindings.at(0);

			// binding slot 0
			layout_binding.binding = 0;

			layout_binding.descriptorType = vk::DescriptorType::eUniformBuffer;
			layout_binding.descriptorCount = 1;

			// only apply for vertex shaders
			layout_binding.stageFlags = vk::ShaderStageFlagBits::eVertex;
			layout_binding.pImmutableSamplers = nullptr;
		}

		// sampler layout
		{
			auto& layout_binding = layout_bindings.at(1);

			// binding slot 0
			layout_binding.binding = 1;

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

	if (const auto result = api_data->device.createDescriptorSetLayout(
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
	const auto api_data = _render_api_data.lock();

	// setup pipeline layout
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

	if (const auto result = api_data->device.createPipelineLayout(
		&pipeline_layout_create_info,
		nullptr,
		&_pipeline_layout);
		result != vk::Result::eSuccess)
	{
		lib_log_e("render_api: failed to create pipeline layout {}", static_cast<int>(result));
		assert(false);
	}

	// shader modules can be nuked after creating pipeline
	const auto vertex_shader = create_shader_module(
		api_data->device,
		vulkan::shaders::basic_shader_vert,
		vulkan::shaders::basic_shader_vert_length);

	const auto fragment_shader_normal = create_shader_module(api_data->device,
		vulkan::shaders::normal_shader_frag,
		vulkan::shaders::normal_shader_frag_length);

	// unused for now ok?
	const auto fragment_shader_sdf = create_shader_module(api_data->device,
		vulkan::shaders::sdf_shader_frag,
		vulkan::shaders::sdf_shader_frag_length);

	const auto fragment_shader_outline = create_shader_module(api_data->device,
		vulkan::shaders::outline_shader_frag,
		vulkan::shaders::outline_shader_frag_length);

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

		// enable backface culling, front face is clockwise
		// only show front, front faces are represented by clock wise rotation,
		rasterization_state_create_info.cullMode = vk::CullModeFlagBits::eBack;
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

	// create the actual graphics pipeline
	vk::GraphicsPipelineCreateInfo pipeline_create_info = {};
	{
		pipeline_create_info.sType = vk::StructureType::eGraphicsPipelineCreateInfo;

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

	// create three pipelines, one for each shader type we need to use
	{
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
				fragment_stage_create_info.module = fragment_shader_normal;

				// entry point, we can combine multiple shaders into one by specifying a entry point
				fragment_stage_create_info.pName = "main";
			}
		}

		// frag/vertex shadeers
		pipeline_create_info.stageCount = shader_stage_create_infos.size();
		pipeline_create_info.pStages = shader_stage_create_infos.data();

		if (const auto result = api_data->device.createGraphicsPipelines(
			nullptr,
			1,
			&pipeline_create_info,
			nullptr,
			&_normal_pipeline);
			result != vk::Result::eSuccess)
		{
			lib_log_e("render_api: failed to create pipeline {}", static_cast<int>(result));
			assert(false);
		}
	}

	{
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
				fragment_stage_create_info.module = fragment_shader_sdf;

				// entry point, we can combine multiple shaders into one by specifying a entry point
				fragment_stage_create_info.pName = "main";
			}
		}

		// frag/vertex shadeers
		pipeline_create_info.stageCount = shader_stage_create_infos.size();
		pipeline_create_info.pStages = shader_stage_create_infos.data();

		if (const auto result = api_data->device.createGraphicsPipelines(
			nullptr,
			1,
			&pipeline_create_info,
			nullptr,
			&_sdf_pipeline);
			result != vk::Result::eSuccess)
		{
			lib_log_e("render_api: failed to create pipeline {}", static_cast<int>(result));
			assert(false);
		}
	}

	{
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

		// frag/vertex shadeers
		pipeline_create_info.stageCount = shader_stage_create_infos.size();
		pipeline_create_info.pStages = shader_stage_create_infos.data();

		if (const auto result = api_data->device.createGraphicsPipelines(
			nullptr,
			1,
			&pipeline_create_info,
			nullptr,
			&_outline_pipeline);
			result != vk::Result::eSuccess)
		{
			lib_log_e("render_api: failed to create pipeline {}", static_cast<int>(result));
			assert(false);
		}
	}

	api_data->device.destroyShaderModule(vertex_shader);
	api_data->device.destroyShaderModule(fragment_shader_normal);
	api_data->device.destroyShaderModule(fragment_shader_sdf);
	api_data->device.destroyShaderModule(fragment_shader_outline);

	lib_log_d("render_api: created graphics pipeline");
}

void render_api::init_command_pool()
{
	const auto api_data = _render_api_data.lock();

	vk::CommandPoolCreateInfo command_pool_create_info = {};
	{
		command_pool_create_info.sType = vk::StructureType::eCommandPoolCreateInfo;
		command_pool_create_info.flags =
			vk::CommandPoolCreateFlagBits::eResetCommandBuffer |
			vk::CommandPoolCreateFlagBits::eTransient;

		command_pool_create_info.queueFamilyIndex = api_data->present_family_index;
	}

	if (const auto result = api_data->device.createCommandPool(
		&command_pool_create_info,
		nullptr,
		&_command_pool);
		result != vk::Result::eSuccess)
	{
		lib_log_e("render_api: failed to create command pool {}", static_cast<int>(result));
		assert(false);
	}

	lib_log_d("render_api: created command pool");
}

void render_api::init_command_buffer()
{
	const auto api_data = _render_api_data.lock();

	vk::CommandBufferAllocateInfo command_buffer_allocate_info = {};
	{
		command_buffer_allocate_info.sType = vk::StructureType::eCommandBufferAllocateInfo;
		command_buffer_allocate_info.commandPool = _command_pool;
		command_buffer_allocate_info.level = vk::CommandBufferLevel::ePrimary;
		command_buffer_allocate_info.commandBufferCount = vulkan::max_frames_in_flight;
	}

	if (const auto result = api_data->device.allocateCommandBuffers(
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

	// copy data into vertex, index and uniform buffer
	std::memcpy(_vertex_staging_buffer_alloc_info.pMappedData, render_command.vertices.data(), vertex_buffer_size);
	std::memcpy(_index_staging_buffer_alloc_info.pMappedData, render_command.indices.data(), index_buffer_size);
	std::memcpy(
		_unifrom_buffer_alloc_info.at(current_frame).pMappedData,
		&_unifrom_buffer_object,
		sizeof(vulkan::uniform_buffer_object_t));

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

	const std::array<vk::Buffer, 1> vertex_buffers = {
		_vertex_buffer
	};

	constexpr std::array<vk::DeviceSize, 1> vertex_buffer_offets = {
		0
	};

	// no secondary cmd buffer so inline
	command_buffer.beginRenderPass(&render_pass_begin_info, vk::SubpassContents::eInline);

	// draw command buffer
	for (uint32_t i = 0; i < render_command.batch_count; i++)
	{
		const auto& batch = render_command.batches.at(i);

		switch (batch.shader)
		{
		case shader_type::normal:
			command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _normal_pipeline);
			break;
		case shader_type::sdf:
			command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _sdf_pipeline);
			break;
		case shader_type::sdf_outline:
			command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _outline_pipeline);
			break;
		}

		command_buffer.bindVertexBuffers(0, vertex_buffers, vertex_buffer_offets);
		command_buffer.bindIndexBuffer(_index_buffer, 0, vk::IndexType::eUint32);

		// set viewport
		command_buffer.setViewport(0, 1, &_viewport);

		vulkan::push_constants_t push_constants = {};
		push_constants.model_matrix = batch.model_matrix;

		command_buffer.pushConstants(
			_pipeline_layout,
			vk::ShaderStageFlagBits::eVertex,
			0,
			sizeof(vulkan::push_constants_t),
			&push_constants);

		command_buffer.bindDescriptorSets(
			vk::PipelineBindPoint::eGraphics,
			_pipeline_layout,
			0,
			1,
			&_descriptor_set.at(current_frame),
			0,
			nullptr);

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
	command_buffer.endRenderPass();
	command_buffer.end();
}

void render_api::init_descriptor_pool()
{
	const auto api_data = _render_api_data.lock();

	std::array<vk::DescriptorPoolSize, 2> pool_sizes = {};
	{
		// uniform buffer object pool size
		{
			auto& pool_size = pool_sizes.at(0);

			pool_size.type = vk::DescriptorType::eUniformBuffer;
			pool_size.descriptorCount = vulkan::max_frames_in_flight;
		}

		// sampler pool size
		{
			auto& pool_size = pool_sizes.at(1);

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

	if (const auto result = api_data->device.createDescriptorPool(
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
	const auto api_data = _render_api_data.lock();

	std::array<vk::DescriptorSetLayout, vulkan::max_frames_in_flight> layouts = {};
	std::ranges::fill(layouts.begin(), layouts.end(), _descriptor_set_layout);

	vk::DescriptorSetAllocateInfo allocate_info = {};
	{
		allocate_info.sType = vk::StructureType::eDescriptorSetAllocateInfo;
		allocate_info.descriptorPool = _descriptor_pool;

		allocate_info.descriptorSetCount = layouts.size();
		allocate_info.pSetLayouts = layouts.data();
	}

	if (const auto result = api_data->device.allocateDescriptorSets(
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
		// uniform buffer object descriptor
		vk::DescriptorBufferInfo buffer_info = {};
		{
			buffer_info.buffer = _uniform_buffer.at(i);
			buffer_info.offset = 0;
			buffer_info.range = sizeof(vulkan::uniform_buffer_object_t);
		}

		// smapler descriptor buffer
		vk::DescriptorImageInfo image_info = {};
		{
			image_info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

			image_info.imageView = _texture_atlas_view;
			image_info.sampler = _texture_sampler;
		}

		std::array<vk::WriteDescriptorSet, 2> descriptor_writes = {};
		{
			// ubo write
			{
				auto& descriptor_write = descriptor_writes.at(0);

				descriptor_write.sType = vk::StructureType::eWriteDescriptorSet;
				descriptor_write.dstSet = _descriptor_set.at(i);
				descriptor_write.dstBinding = 0;
				descriptor_write.dstArrayElement = 0;

				descriptor_write.descriptorType = vk::DescriptorType::eUniformBuffer;
				descriptor_write.descriptorCount = 1;
				descriptor_write.pBufferInfo = &buffer_info;
				descriptor_write.pImageInfo = nullptr;
			}

			// sampler write
			{
				auto& descriptor_write = descriptor_writes.at(1);

				descriptor_write.sType = vk::StructureType::eWriteDescriptorSet;
				descriptor_write.dstSet = _descriptor_set.at(i);
				descriptor_write.dstBinding = 1;
				descriptor_write.dstArrayElement = 0;

				descriptor_write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
				descriptor_write.descriptorCount = 1;
				descriptor_write.pBufferInfo = nullptr;
				descriptor_write.pImageInfo = &image_info;
			}
		}

		api_data->device.updateDescriptorSets(
			descriptor_writes.size(), descriptor_writes.data(),
			0, nullptr);
	}
}

void render_api::init_texture_view()
{
	const auto api_data = _render_api_data.lock();

	_texture_atlas_view = create_image_view(
		api_data->device,
		_texture_atlas,
		vk::Format::eR8G8B8A8Unorm);
}

void render_api::init_texture_sampler()
{
	const auto api_data = _render_api_data.lock();

	vk::PhysicalDeviceProperties device_properties = {};
	api_data->physical_device.getProperties(&device_properties);

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

	if (const auto result = api_data->device.createSampler(
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
	const auto api_data = _render_api_data.lock();

	std::ranges::for_each(_swapchain_frame_buffers.begin(), _swapchain_frame_buffers.end(),
	[&](const auto& frame_buffer)
	{
		api_data->device.destroyFramebuffer(frame_buffer);
	});

	std::ranges::for_each(_swapchain_image_views.begin(), _swapchain_image_views.end(),
		[&](const auto& image_view)
	{
		api_data->device.destroyImageView(image_view);
	});

	_swapchain_frame_buffers.clear();
	_swapchain_image_views.clear();

	api_data->device.destroySwapchainKHR(_swap_chain);
}

void render_api::destroy_image()
{
	const auto api_data = _render_api_data.lock();

	api_data->device.freeDescriptorSets(
		_descriptor_pool,
		_descriptor_set.size(),
		_descriptor_set.data());

	api_data->device.destroyImageView(_texture_atlas_view);

	vmaDestroyImage(_vk_allocator, _texture_atlas, _texture_atlas_alloc);
	_init_texture = false;
}

void render_api::init_vertex_buffer()
{
	constexpr vk::DeviceSize buffer_size = sizeof(vertex_t) * MAX_VERTICES;

	VkBufferCreateInfo staging_buffer_create_info = {};
	{
		staging_buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		staging_buffer_create_info.size = buffer_size;

		staging_buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		staging_buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	VmaAllocationCreateInfo staging_buffer_allocation_info = {};
	{
		staging_buffer_allocation_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
		staging_buffer_allocation_info.flags =
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
			VMA_ALLOCATION_CREATE_MAPPED_BIT;
	}

	// create staging buffer, used to write data from CPU to GPU
	vmaCreateBuffer(
		_vk_allocator,
		&staging_buffer_create_info,
		&staging_buffer_allocation_info,
		&_vertex_staging_buffer,
		&_vertex_staging_buffer_alloc,
		&_vertex_staging_buffer_alloc_info);

	VkBufferCreateInfo vertex_buffer_create_info = {};
	{
		vertex_buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vertex_buffer_create_info.size = buffer_size;

		vertex_buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		vertex_buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	VmaAllocationCreateInfo vertex_buffer_allocation_info = {};
	{
		vertex_buffer_allocation_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
	}

	// create actual vertex buffer used by GPU
	vmaCreateBuffer(
		_vk_allocator,
		&vertex_buffer_create_info,
		&vertex_buffer_allocation_info,
		&_vertex_buffer,
		&_vertex_buffer_alloc,
		nullptr);
}

void render_api::init_index_buffer()
{
	constexpr vk::DeviceSize buffer_size = sizeof(uint32_t) * MAX_INDICES;

	VkBufferCreateInfo staging_buffer_create_info = {};
	{
		staging_buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		staging_buffer_create_info.size = buffer_size;

		staging_buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		staging_buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	VmaAllocationCreateInfo staging_buffer_allocation_info = {};
	{
		staging_buffer_allocation_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
		staging_buffer_allocation_info.flags =
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
			VMA_ALLOCATION_CREATE_MAPPED_BIT;
	}

	// create staging buffer, used to write data from CPU to GPU
	vmaCreateBuffer(
		_vk_allocator,
		&staging_buffer_create_info,
		&staging_buffer_allocation_info,
		&_index_staging_buffer,
		&_index_staging_buffer_alloc,
		&_index_staging_buffer_alloc_info);

	VkBufferCreateInfo index_buffer_create_info = {};
	{
		index_buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		index_buffer_create_info.size = buffer_size;

		index_buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		index_buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	VmaAllocationCreateInfo index_buffer_allocation_info = {};
	{
		index_buffer_allocation_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
	}

	// create actual ndex buffer used by GPU
	vmaCreateBuffer(
		_vk_allocator,
		&index_buffer_create_info,
		&index_buffer_allocation_info,
		&_index_buffer,
		&_index_buffer_alloc,
		nullptr);
}

void render_api::init_render_pass()
{
	const auto api_data = _render_api_data.lock();

	vk::AttachmentDescription color_attatchment = {};
	{
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

	// we only have 1 sub pass since we don't do any fancy post processing crap in our renderer
	vk::AttachmentReference color_attatchment_reference = {};
	{
		// refereces layout location, eg: layout(location = 0) out vec4 outColor
		color_attatchment_reference.attachment = 0;
		color_attatchment_reference.layout = vk::ImageLayout::eColorAttachmentOptimal;
	}

	vk::SubpassDescription subpass_description = {};
	{
		subpass_description.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

		subpass_description.colorAttachmentCount = 1;
		subpass_description.pColorAttachments = &color_attatchment_reference;
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

		render_pass_create_info.attachmentCount = 1;
		render_pass_create_info.pAttachments = &color_attatchment;

		// only one subpass
		render_pass_create_info.subpassCount = 1;
		render_pass_create_info.pSubpasses = &subpass_description;

		render_pass_create_info.dependencyCount = subpass_dependencies.size();
		render_pass_create_info.pDependencies = subpass_dependencies.data();
	}

	if (const auto result = api_data->device.createRenderPass(
		&render_pass_create_info, nullptr, &_render_pass);
		result != vk::Result::eSuccess)
	{
		lib_log_e("render_api: failed to create render pass {}", static_cast<int>(result));
		assert(false);
	}
}

void render_api::init_frame_buffer()
{
	const auto api_data = _render_api_data.lock();

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

		if (const auto result = api_data->device.createFramebuffer(
			&framebuffer_create_info,
			nullptr,
			&_swapchain_frame_buffers.at(i));
			result != vk::Result::eSuccess)
		{
			lib_log_e("render_api: failed to create frame buffer {}", static_cast<int>(result));
			assert(false);
		}
	}
}

void render_api::init_uniform_buffer()
{
	VkBufferCreateInfo uniform_buffer_create_info = {};
	{
		uniform_buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		uniform_buffer_create_info.size = sizeof(vulkan::uniform_buffer_object_t);

		uniform_buffer_create_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		uniform_buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	VmaAllocationCreateInfo uniform_buffer_allocation_info= {};
	{
		uniform_buffer_allocation_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
		uniform_buffer_allocation_info.flags =
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
			VMA_ALLOCATION_CREATE_MAPPED_BIT;
	}

	for (size_t i = 0; i < vulkan::max_frames_in_flight; i++)
	{
		// create uniform buffer for each "frame"
		vmaCreateBuffer(
			_vk_allocator,
			&uniform_buffer_create_info,
			&uniform_buffer_allocation_info,
			&_uniform_buffer.at(i),
			&_unifrom_buffer_alloc.at(i),
			&_unifrom_buffer_alloc_info.at(i));
	}
}
