#pragma once

#ifdef CAMEL_NVRHI_VULKAN

#include <queue>

#include <module_logger/Logger.hpp>
#include <module_render/backend/Device_Common.hpp>

namespace lib::render {
struct vk_create_surface_callback_t {
    vk::Instance*       instance;
    vk::SurfaceKHR*     surface;
};

struct vk_get_physical_device_support_t {
    vk::Instance*       instance;
    vk::PhysicalDevice* physical_device;
    uint32_t            queue_family;
};

//! Vulkan device implementation
class Device_Vulkan final : public Device_Common {

    class NvrhiMessageCallback final: public nvrhi::IMessageCallback {
    public:
        explicit NvrhiMessageCallback(const std::shared_ptr<logger::Logger>& logger)
            : _logger(logger) {}

        void message(const nvrhi::MessageSeverity severity, const char* message) override {
            const auto log = _logger.lock();

            if (!log) {
                return;
            }

            switch (severity) {
                case nvrhi::MessageSeverity::Info:
                    log->d("nvrhi-debug", "{}", message);
                    break;
                case nvrhi::MessageSeverity::Warning:
                    log->w("nvrhi-debug", "{}", message);
                    break;
                case nvrhi::MessageSeverity::Error:
                case nvrhi::MessageSeverity::Fatal:
                    log->e("nvrhi-debug", "{}", message);
                    break;
            }
        }
    private:
        std::weak_ptr<logger::Logger> _logger;

    };

    struct queue_family_properties_t {
        int graphics_queue  = -1;
        int compute_queue   = -1;
        int transfer_queue  = -1;
        int present_queue   = -1;
    };

    enum class PhysicalDeviceFeatures {
        AccelStruct,
        RayPipeline,
        RayQueue,
        Meshlets,
        VRS,
        Synchronization2,
        Maintenance4,
        SwapChainMutableFormat,
        BufferDeviceAddress
    };

    struct swapchain_image_t {
        vk::Image               image;
        nvrhi::TextureHandle    texture;
    };

public:
    Device_Vulkan(const std::shared_ptr<logger::Logger>& logger, const device_settings_t& settings);
    ~Device_Vulkan() override;

public:
    [[nodiscard]] nvrhi::IDevice* device() const override;
    [[nodiscard]] const point2Di& back_buffer_size() const override;
    [[nodiscard]] nvrhi::ITexture* current_back_buffer() const override;
    [[nodiscard]] nvrhi::ITexture* back_buffer(uint32_t index) const override;
    [[nodiscard]] uint32_t current_back_buffer_index() const override;
    [[nodiscard]] uint32_t back_buffer_count() const override;
    void update_screen_size(const point2Di& window_size) override;
    void begin_frame() override;
    void present() override;

private:
    [[nodiscard]] std::expected<void, std::string> create_vk_instance();
    [[nodiscard]] std::expected<void, std::string> create_vk_device();
    [[nodiscard]] std::expected<void, std::string> create_vk_swapchain();
    [[nodiscard]] std::expected<void, std::string> create_logical_device();

    [[nodiscard]] std::expected<vk::PhysicalDevice, std::string> pick_physical_device() const;
    [[nodiscard]] std::expected<queue_family_properties_t, std::string> pick_queue_families(const vk::PhysicalDevice& device) const;

    void destroy_swapchain();

    // Internal vulkan callback - don't push to consumer because it's vulkan specific
    static VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
        VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t obj,
        size_t location,
        int32_t code,
        const char* layerPrefix,
        const char* msg,
        void* userData
    );

private:
    device_settings_t _settings;
    std::shared_ptr<logger::Logger> _logger;

    vulkan_extension_set_t _extension_enabled;
    vulkan_extension_set_t _extension_optional;

    // Vulkan handles
    vk::Instance _vk_instance               = nullptr;
    vk::PhysicalDevice _vk_physical_device  = nullptr;
    vk::Device _vk_device                   = nullptr;
    vk::SurfaceKHR _vk_surface              = nullptr;

    vk::DebugReportCallbackEXT _debug_report_callback = nullptr;

    vk::Queue _graphics_queue   = nullptr;
    vk::Queue _compute_queue    = nullptr;
    vk::Queue _transfer_queue   = nullptr;
    vk::Queue _present_queue    = nullptr;

    queue_family_properties_t _queue_family = {};
    std::unordered_set<PhysicalDeviceFeatures> _physical_device_features = {};

    point2Di _back_buffer_size = {};
    std::unique_ptr<NvrhiMessageCallback> _nv_callback;

    nvrhi::vulkan::DeviceHandle _nv_device              = nullptr;
    nvrhi::DeviceHandle _nv_device_validation           = nullptr;
    nvrhi::CommandListHandle _nv_command_list_barrier   = nullptr;

    std::queue<nvrhi::EventQueryHandle> _nv_frames_in_flight    = {};
    std::vector<nvrhi::EventQueryHandle> _nv_query_pool         = {};

    std::vector<vk::Semaphore> _present_semaphores      = {};
    uint32_t _present_semaphore_index                   = 0;

    vk::SurfaceFormatKHR _vk_swapchain_formats  = {};
    vk::SwapchainKHR _vk_swapchain              = nullptr;

    uint32_t _vk_swapchain_index                        = static_cast<uint32_t>(-1);
    std::vector<swapchain_image_t> _vk_swapchain_images = {};

};
}

#endif