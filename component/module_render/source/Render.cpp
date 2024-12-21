#include <module_render/Render.hpp>
#include <module_render/backend/Device_Vulkan.hpp>

#include <module_system/filesystem.hpp>

using namespace lib::render;

Render::Render(
    const std::shared_ptr<logger::Logger>& logger,
    const RenderAPI render_api,
    const device_settings_t& settings
)
    : _logger(logger) {

    switch (render_api) {
        case RenderAPI::Vulkan:
            _device = std::make_unique<Device_Vulkan>(_logger, settings);
            break;
        default:
            throw std::runtime_error("Unsupported render_api selected");
    }

    _device->add_callback(CallbackState::Resizing,  FrameBuffer_Resizing);
    _device->add_callback(CallbackState::Resized,   FrameBuffer_Resized);

    // Force update on screen size to invoke callbacks etc.
    _device->update_screen_size(settings.starting_size, true);
}

Render::~Render() {

}

void Render::update_screen_size(const point2Di& window_size) const {
    _device->update_screen_size(window_size, false);
}

void Render::on_frame() {

    if (!_device->is_minimised()) {
        _device->begin_frame();

        ///
        /// START DRAWING
        ///



        ///
        /// FINISH DRAWING
        ///

        _device->present();
    }

    _device->device()->runGarbageCollection();
}

void Render::FrameBuffer_Resizing(const DeviceCallback& device) {

}

void Render::FrameBuffer_Resized(const DeviceCallback& device) {

}

