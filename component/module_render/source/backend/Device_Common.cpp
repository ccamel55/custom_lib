#include <module_render/backend/Device_Common.hpp>

using namespace lib::render;

Device_Common::Device_Common(
    const std::shared_ptr<logger::Logger>& logger,
    const device_settings_t& settings,
    RenderCallback_Fn cb_resizing,
    RenderCallback_Fn cb_resized
)
    : _logger(logger)
    , _settings(settings)
    , _cb_resizing(std::move(cb_resizing))
    , _cb_resized(std::move(cb_resized)) {

}

