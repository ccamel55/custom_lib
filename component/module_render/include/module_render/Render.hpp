#pragma once

#include <module_logger/Logger.hpp>

#include <module_render/Render.hpp>
#include <module_render/RenderConsumer.hpp>
#include <module_render/util/Shader.hpp>

namespace lib::render
{
// Alias correct type
class Render;
using RenderConsumer = RenderConsumer_Base<Render>;

enum class RenderAPI {
    DX11,
    DX12,
    Vulkan,
};

//! Render handler
class Render : public RenderConsumer {
    friend class RenderConsumer_Base;

public:
    Render(
        const std::shared_ptr<logger::Logger>& logger,
        RenderAPI render_api,
        const device_settings_t& settings
    );

    ~Render();

    void update_screen_size(const point2Di& window_size) const;
    void on_frame();

private:
    static void FrameBuffer_Resizing(const DeviceCallback& device);
    static void FrameBuffer_Resized(const DeviceCallback& device);

private:
    void internal_some_func() {

    }

private:
    std::shared_ptr<logger::Logger> _logger;
    std::unique_ptr<Device_Common> _device;

};}
