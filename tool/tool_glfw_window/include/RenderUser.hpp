#pragma once

#include <module_core/type/point/point2D.hpp>
#include <module_logger/Logger.hpp>
#include <module_render/Render.hpp>

#include <memory>

class RenderUser {
public:
    RenderUser(
        const std::shared_ptr<lib::logger::Logger>& logger,
        const lib::render::device_settings_t& settings
    );

    ~RenderUser();

    void update_screen_size(const lib::point2Di& window_size, bool minimised) const;
    void on_frame(bool minimised);

private:
    std::shared_ptr<lib::logger::Logger> _logger;
    std::unique_ptr<lib::render::Device_Common> _device;

};
