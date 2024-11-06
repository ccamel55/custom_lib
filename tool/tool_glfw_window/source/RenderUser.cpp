#include <RenderUser.hpp>

#include <module_logger/ScopedLog.hpp>
#include <module_render/backend/Device_Vulkan.hpp>

using namespace lib;

RenderUser::RenderUser(
    const std::shared_ptr<logger::Logger>& logger,
    const render::device_settings_t& settings
)
    : _logger(logger)
    , _device(std::make_unique<render::Device_Vulkan>(_logger, settings)) {

    _device->add_callback(render::CallbackState::Resized, [](const render::DeviceCallback& device) {

    });

    _device->add_callback(render::CallbackState::Resizing, [](const render::DeviceCallback& device) {

    });

    // Force update on screen size to invoke callbacks etc.
    _device->update_screen_size(settings.starting_size);
}

RenderUser::~RenderUser() {

}

void RenderUser::update_screen_size(
    const point2Di& window_size,
    [[maybe_unused]] const bool minimised
) const {
    // Only call update if screen size changes
    if (_device->back_buffer_size() == window_size) {
        return;
    }
    _device->update_screen_size(window_size);
}

void RenderUser::on_frame(const bool minimised) {

    if (!minimised) {
        _device->begin_frame();

        ///
        /// START DRAWING
        ///



        ///
        /// FINISH DRAWING
        ///
        ///
        _device->present();
    }

    _device->device()->runGarbageCollection();
}
