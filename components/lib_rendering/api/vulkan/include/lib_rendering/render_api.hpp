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

namespace lib::rendering
{
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

private:
    vk::Instance _instance = nullptr;
    vk::SurfaceKHR _window_surface = nullptr;

    vk::PhysicalDevice _physical_device = nullptr;
    vk::Device _logical_device = nullptr;
    vk::Queue _graphics_present_queue = nullptr;

    vk::SwapchainKHR _swap_chain = nullptr;
    std::vector<vk::Image> _swapchain_images = {};
    std::vector<vk::ImageView> _swapchain_image_views = {};

    // states of current swapchain, so we can use later
    vk::Format _swapchian_format = {};
    vk::Extent2D _swapchain_extent = {};

};
}  // namespace lib::rendering