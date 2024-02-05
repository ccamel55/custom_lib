#pragma once

#include <lib_rendering/render_api_base.hpp>
#include <core_sdk/types/vector/vector2D.hpp>

// todo: maybe move this into cmake? IDK what is more appropriate
#if WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#elif APPLE
// todo, find wtf apple uses with GLFW, I assume metal??
#error "Add vulkan apple implementation"
#endif

// disable no discard, asserts sometimes fuck up parameters on release?
// seems to be a bug but not sure lol
#define VULKAN_HPP_NO_NODISCARD_WARNINGS

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

#include <optional>

namespace lib::rendering
{
namespace vulkan
{
constexpr uint8_t max_frames_in_flight = 2;

struct swapchain_support_details_t
{
    vk::SurfaceCapabilitiesKHR capabilities = {};
    std::vector<vk::SurfaceFormatKHR> formats = {};
    std::vector<vk::PresentModeKHR> present_modes = {};
};

// note: careful because vulkan expects shit to be aligned in a certain way depending on type
struct push_constants_t
{
    lib::vector2D scale = {};
    lib::vector2D translate = {};
};
}

struct render_api_data_t
{
    vk::Instance instance = {};
    vk::SurfaceKHR surface = {};
    vk::Queue queue = {};

    vk::PhysicalDevice physical_device = {};
    vk::Device device = {};

    uint32_t vulkan_api_version = VK_MAKE_API_VERSION(0, 1, 3, 0);
    uint32_t present_family_index = 0;
};

class render_api final : public render_api_base
{
public:
    //! \p api_context should equal the window ptr that vulkan will bind it's surface too.
    render_api(const std::weak_ptr<render_api_data_t>& render_api_data, bool flush_buffers);
    ~render_api() override;

    void bind_atlas(const uint8_t* data, int width, int height) override;
    void update_screen_size(const lib::point2Di& window_size) override;
    void draw(const render_command& render_command) override;

private:
    void init_swapcahin();
    void init_image_views();

    void init_descriptor_set_layout();
    void init_graphics_pipeline();
    void init_command_pool();
    void init_vertex_buffer();
    void init_index_buffer();
    void init_command_buffer();
    void init_descriptor_pool();
    void init_descriptor_set();
    void init_texture_view();
    void init_texture_sampler();

    void destroy_swapchain();
    void destroy_image();

    void record_command_buffer(
        const vk::CommandBuffer& command_buffer,
        uint32_t image_index,
        uint32_t current_frame,
        const render_command& render_command) const;

private:
    std::weak_ptr<render_api_data_t> _render_api_data = {};
    uint32_t _current_frame = 0;

    bool _stop_rendering = false;
    bool _init_texture = false;

    vk::CommandPool _command_pool = {};
    std::array<vk::CommandBuffer, vulkan::max_frames_in_flight> _command_buffers = {};

    vk::SwapchainKHR _swap_chain = {};
    std::vector<vk::Image> _swapchain_images = {};
    std::vector<vk::ImageView> _swapchain_image_views = {};

    // states of current swapchain, so we can use later
    vk::Format _swapchian_format = {};
    vk::Extent2D _swapchain_extent = {};

    // render class crap
    vk::Viewport _viewport = {};
    vulkan::push_constants_t _push_constants = {};

    vk::RenderPass _render_pass = {};
    vk::DescriptorSetLayout _descriptor_set_layout = {};
    vk::DescriptorPool _descriptor_pool = {};
    vk::PipelineLayout _pipeline_layout = {};

    vk::Pipeline _normal_pipeline = {};
    vk::Pipeline _sdf_pipeline = {};
    vk::Pipeline _outline_pipeline = {};

    vk::ImageView _texture_atlas_view = {};
    vk::Sampler _texture_sampler = {};

    VmaAllocator _vk_allocator = nullptr;

    VkImage _texture_atlas = {};
    VmaAllocation _texture_atlas_alloc = nullptr;

    VkBuffer _vertex_buffer = {};
    VmaAllocation _vertex_buffer_alloc = nullptr;

    VkBuffer _vertex_staging_buffer = {};
    VmaAllocation _vertex_staging_buffer_alloc = nullptr;
    VmaAllocationInfo _vertex_staging_buffer_alloc_info = {};

    VkBuffer _index_buffer = {};
    VmaAllocation _index_buffer_alloc = nullptr;

    VkBuffer _index_staging_buffer = {};
    VmaAllocation _index_staging_buffer_alloc = nullptr;
    VmaAllocationInfo _index_staging_buffer_alloc_info = {};

    // synchronization
    std::array<vk::Semaphore, vulkan::max_frames_in_flight> _image_available_semaphores = {};
    std::array<vk::Semaphore, vulkan::max_frames_in_flight> _render_finished_semaphores = {};
    std::array<vk::Fence, vulkan::max_frames_in_flight> _in_flight_fences = {};

    std::array<vk::DescriptorSet, vulkan::max_frames_in_flight> _descriptor_set = {};
};
}  // namespace lib::rendering