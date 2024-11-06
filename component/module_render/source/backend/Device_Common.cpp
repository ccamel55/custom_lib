#include <module_render/backend/Device_Common.hpp>

using namespace lib::render;

void Device_Common::add_callback(const CallbackState state, std::function<void(const DeviceCallback&)> callback) {
    _callback[state] = std::move(callback);
}



