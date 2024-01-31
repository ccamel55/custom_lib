#pragma once

#include <lib_rendering/render_api_base.hpp>

// todo: maybe move this into cmake? IDK what is more appropriate
#if WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#elif APPLE
// todo, find wtf apple uses with GLFW, I assume metal??
#error "Add vulkan apple implementation"
#endif

#include <vulkan/vulkan.hpp>
#include <optional>

namespace lib::rendering
{
namespace vulkan
{
constexpr uint8_t max_frames_in_flight = 2;
}

class render_api final : public render_api_base
{
public:
    //! \p api_context should equal the window ptr that vulkan will bind it's surface too.
    render_api(void* api_context, bool flush_buffers);
    ~render_api() override;

    void bind_atlas(const uint8_t* data, int width, int height) override;
    void update_screen_size(const lib::point2Di& window_size) override;
    void update_frame_buffer(const render_command& render_command) override;
    void draw_frame_buffer() override;

private:
    void init_vulkan(const std::vector<const char*>& extensions, const std::vector<const char*>& layers);
    void init_device(const std::vector<const char*>& layers);
    void init_surface();
    void init_swapcahin();
    void init_image_views();

    void init_render_passes();
    void init_graphics_pipeline();
    void init_frame_buffers();
    void init_command_pool();
    void init_vertex_buffer();
    void init_index_buffer();
    void init_command_buffer();

    void destroy_swapchain();
    void record_command_buffer(const vk::CommandBuffer& command_buffer, uint32_t image_index) const;

private:
    std::optional<uint32_t> _graphics_present_family_index = std::nullopt;

    vk::Instance _instance = nullptr;
    vk::SurfaceKHR _window_surface = nullptr;

    vk::PhysicalDevice _physical_device = nullptr;
    vk::Device _logical_device = nullptr;
    vk::Queue _graphics_present_queue = nullptr;
    vk::CommandPool _command_pool = nullptr;
    std::array<vk::CommandBuffer, vulkan::max_frames_in_flight> _command_buffers = {};

    vk::SwapchainKHR _swap_chain = nullptr;
    std::vector<vk::Image> _swapchain_images = {};
    std::vector<vk::ImageView> _swapchain_image_views = {};
    std::vector<vk::Framebuffer> _swapchain_frame_buffers = {};

    // states of current swapchain, so we can use later
    vk::Format _swapchian_format = {};
    vk::Extent2D _swapchain_extent = {};

    vk::RenderPass _render_pass = nullptr;
    vk::PipelineLayout _pipeline_layout = nullptr;
    vk::Pipeline _pipeline = nullptr;

    vk::Buffer _vertex_buffer = nullptr;
    vk::DeviceMemory _vertex_buffer_memory = nullptr;

    vk::Buffer _index_buffer = nullptr;
    vk::DeviceMemory _index_buffer_memory = nullptr;

    // synchronization
    std::array<vk::Semaphore, vulkan::max_frames_in_flight> _image_available_semaphores = {};
    std::array<vk::Semaphore, vulkan::max_frames_in_flight> _render_finished_semaphores = {};
    std::array<vk::Fence, vulkan::max_frames_in_flight> _in_flight_fences = {};

    uint32_t _current_frame = 0;
    bool _stop_rendering = false;
};
}  // namespace lib::rendering