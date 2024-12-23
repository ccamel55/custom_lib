#pragma once

#include <dep_nvrhi/nvrhi.hpp>

#include <module_core/NoCopy.hpp>
#include <module_core/type/point/point2D.hpp>
#include <module_logger/Logger.hpp>

#include <functional>
#include <memory>
#include <unordered_set>

namespace lib::render {

class Render;

//
// Constants used by each device implementation
//
constexpr nvrhi::Format SWAP_CHAIN_FORMAT   = nvrhi::Format::SBGRA8_UNORM;
constexpr size_t SWAP_CHAIN_BUFFER_COUNT    = 3;
constexpr size_t MAX_FRAMES_IN_FLIGHT       = 2;

//! Vulkan extensions enabled for each respective object
struct vulkan_extension_set_t {
    std::unordered_set<std::string> instance;
    std::unordered_set<std::string> layer;
    std::unordered_set<std::string> device;

    void merge(vulkan_extension_set_t other) {
        instance.merge(other.instance);
        layer.merge(other.layer);
        device.merge(other.device);
    }
};

//! Create device settings
struct device_settings_t {
    bool debug          = false;
    bool validation     = false;
    bool compute_queue  = false;
    bool copy_queue     = false;
    bool vsync          = false;

    point2Di back_buffer_size = {};

#ifdef CAMEL_NVRHI_VULKAN
    // Vulkan specific settings
    struct vulkan_t {
        vulkan_extension_set_t extra_extension_enabled;
        vulkan_extension_set_t extra_extension_optional;

        // These are specific to what you are using to create a window.
        // GLFW provides a function that will call the right version depending on platform.
        // Note: void* should be any type of user data

        std::function<int(void*)> create_window_surface;
        std::function<int(void*)> get_physical_device_presentation_support;
    } vulkan;
#endif

#ifdef CAMEL_NVRHI_DX_11
    // DirectX 11 specific settings
    struct d3d11_t {

    } d3d11;
#endif

#ifdef CAMEL_NVRHI_DX_11
    // DirectX 12 specific settings
    struct d3d12_t {

    } d3d12;
#endif
};

using RenderCallback_Fn = std::function<void()>;

//! Shared common device type.
class Device_Common : public NoCopy {
    friend class Render;

public:
    Device_Common(
        const std::shared_ptr<logger::Logger>& logger,
        const device_settings_t& settings,
        RenderCallback_Fn cb_resizing = nullptr,
        RenderCallback_Fn cb_resized = nullptr
    );

    virtual ~Device_Common() = default;

    //! Get NVRHI device raw ptr
    //! \return nvrhi device ptr
    [[nodiscard]] virtual nvrhi::IDevice* device() const = 0;

    //! Get NVRHI device handle
    //! \return nvrhi device handle
    [[nodiscard]] virtual nvrhi::DeviceHandle device_handle() const = 0;

    //! Get current back buffer texture
    //! \return current back buffer texture
    [[nodiscard]] virtual nvrhi::ITexture* current_back_buffer() const = 0;
    //
    //! Get back buffer texture for specific index
    //! \param index index of back buffer we want
    //! \return texture for back buffer at index
    [[nodiscard]] virtual nvrhi::ITexture* back_buffer(uint32_t index) const = 0;

    //! Get index of current back buffer
    //! \return current back buffer index
    [[nodiscard]] virtual uint32_t current_back_buffer_index() const = 0;

    //! Get number of back buffers we have created
    //! \return number of back buffers we have
    [[nodiscard]] virtual uint32_t back_buffer_count() const = 0;

protected:
    [[nodiscard]] virtual std::expected<void, std::string> create_device() = 0;
    [[nodiscard]] virtual std::expected<void, std::string> create_swap_chain() = 0;
    virtual void resize_swap_chain() = 0;
    virtual void destroy_device_and_swap_chain() = 0;
    virtual void begin_frame() = 0;
    virtual void present() = 0;

protected:
    std::shared_ptr<logger::Logger> _logger;
    device_settings_t _settings;

    RenderCallback_Fn _cb_resizing;
    RenderCallback_Fn _cb_resized;

};
}
