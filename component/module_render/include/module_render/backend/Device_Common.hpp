#pragma once

#include <dep_nvrhi/nvrhi.hpp>

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <module_core/type/point/point2D.hpp>

namespace lib::render {

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

    point2Di starting_size = {};

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

enum class CallbackState {
    Resizing,
    Resized
};

//! Base class that defines all callback methods
class DeviceCallback {
public:
    virtual ~DeviceCallback() = default;

    //! Get NVRHI device handle
    //! \return nvrhi device handle
    [[nodiscard]] virtual nvrhi::IDevice* device() const = 0;

    //! Get back buffer size in pixels
    //! \return back buffer size in pixels
    [[nodiscard]] virtual const point2Di& back_buffer_size() const = 0;

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

    //! Whether current screen is minimised.
    [[nodiscard]] bool is_minimised() const {
        const point2Di back_buffer = back_buffer_size();
        return back_buffer.x == 0 || back_buffer.y == 0;
    }
};

//! Shared common device type.
class Device_Common : public DeviceCallback {
public:
    //! Used to explicitly update screen size
    //! \param window_size size of window in pixels
    //! \param force_update force update/recreate swap chain regardless of current screen size.
    virtual void update_screen_size(const point2Di& window_size, bool force_update) = 0;

    //! Called before writing to command pool
    virtual void begin_frame() = 0;

    //! Called to draw command pool
    virtual void present() = 0;

public:
    //! Add a callback for a specific device state
    //! \param state State to invoke callback
    //! \param callback callback to invoke
    void add_callback(CallbackState state, std::function<void(const DeviceCallback&)> callback);

protected:
    std::unordered_map<CallbackState, std::function<void(const DeviceCallback&)>> _callback = {};

};
}
