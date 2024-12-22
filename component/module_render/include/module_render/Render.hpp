#pragma once

#include <module_core/type/point/point2D.hpp>
#include <module_logger/Logger.hpp>
#include <module_render/RenderPass.hpp>
#include <module_render/backend/Device_Common.hpp>

#include <list>

namespace lib::render
{
enum class RenderAPI {
    DX11,
    DX12,
    Vulkan,
};

/*
    TODO:
        - multi-threaded rendering support
        - batch system
        - create texture atlas for basic types
        - support custom shader
        - support custom textures
        - support custom transforms
        - use shader specialisation to pick specific draw types (SDF etc.)
*/

//! Render handler
class Render {
public:
    Render(
        const std::shared_ptr<logger::Logger>& logger,
        const device_settings_t& settings,
        RenderAPI render_api
    );

    ~Render();

public:
    [[nodiscard]] const std::unique_ptr<Device_Common>& backend() const;
    void emplace_render_pass_back(RenderPass* pass);
    void emplace_render_pass_front(RenderPass* pass);
    void erase_render_pass(RenderPass* pass);

    void update_screen_size(const point2Di& size);
    void present();

private:
    void passes_update_frame() const;
    void passes_render() const;
    void back_buffer_resizing();
    void back_buffer_resized();

private:
    std::shared_ptr<logger::Logger> _logger;
    std::unique_ptr<Device_Common> _device;

    FrameInterval _interval;
    bool _is_visible = false;

    std::vector<nvrhi::FramebufferHandle> _swap_chain_frame_buffers;
    std::list<RenderPass*> _render_passes;

};}
