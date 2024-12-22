#include <module_render/Render.hpp>
#include <module_render/backend/Device_Vulkan.hpp>

#include <module_system/filesystem.hpp>

using namespace lib::render;

Render::Render(
    const std::shared_ptr<logger::Logger>& logger,
    const device_settings_t& settings,
    const RenderAPI render_api
)
    : _logger(logger) {

    switch (render_api) {
        case RenderAPI::Vulkan:
            _device = std::make_unique<Device_Vulkan>(
                _logger,
                settings,
                [this]() { back_buffer_resizing(); },
                [this]() { back_buffer_resized(); }
            );
            break;
        default:
            throw std::runtime_error("Unsupported render_api selected");
    }

    if (const auto res = _device->create_device(); !res.has_value()) [[unlikely]] {
        throw std::runtime_error(res.error());
    }

    if (const auto res = _device->create_swap_chain(); !res.has_value()) [[unlikely]] {
        throw std::runtime_error(res.error());
    }

    // Force callbacks to be invoked at least once
    _device->_settings.back_buffer_size.x = 0;
    _device->_settings.back_buffer_size.y = 0;

    update_screen_size({_device->_settings.back_buffer_size.x, _device->_settings.back_buffer_size.y});
}

Render::~Render() {
    _swap_chain_frame_buffers.clear();
    _device->destroy_device_and_swap_chain();
}

const std::unique_ptr<Device_Common>& Render::backend() const {
    return _device;
}

void Render::emplace_render_pass_back(RenderPass* pass) {
    _render_passes.remove(pass);
    _render_passes.push_front(pass);

    pass->back_buffer_resizing();
    pass->back_buffer_resized(_device->_settings.back_buffer_size);
}

void Render::emplace_render_pass_front(RenderPass* pass) {
    _render_passes.remove(pass);
    _render_passes.push_front(pass);

    pass->back_buffer_resizing();
    pass->back_buffer_resized(_device->_settings.back_buffer_size);
}

void Render::erase_render_pass(RenderPass* pass) {
    _render_passes.remove(pass);
}

void Render::present_passes() {

    if (_is_visible) {
        update_frame();
        _device->begin_frame();
        render();
        _device->present();
    }

    _device->device()->runGarbageCollection();
    _interval.emplace();
}

void Render::update_screen_size(const point2Di& size) {

    if (size.x == 0 || size.y == 0) {
        _is_visible = false;
        return;
    }

    _is_visible = true;

    if (auto& back_buffer_size = _device->_settings.back_buffer_size; back_buffer_size != size) {
        back_buffer_resizing();
        {
            back_buffer_size = size;
            _device->resize_swap_chain();
        }
        back_buffer_resized();
    }
}

void Render::update_frame() const {
    for (const auto pass : _render_passes) {
        pass->update_frame(_interval);
    }
}

void Render::render() const {
    nvrhi::IFramebuffer* frame_buffer = _swap_chain_frame_buffers[_device->current_back_buffer_index()];

    for (const auto pass : _render_passes) {
        pass->render(frame_buffer);
    }
}

void Render::back_buffer_resizing() {
    _swap_chain_frame_buffers.clear();

    for (const auto pass : _render_passes) {
        pass->back_buffer_resizing();
    }
}

void Render::back_buffer_resized() {

    for(const auto pass : _render_passes) {
        pass->back_buffer_resized(_device->_settings.back_buffer_size);
    }

    const auto back_buffer_count = _device->back_buffer_count();
    _swap_chain_frame_buffers.resize(back_buffer_count);

    for (uint32_t index = 0; index < back_buffer_count; index++)
    {
        nvrhi::FramebufferDesc desc;
        desc.addColorAttachment(_device->back_buffer(index));

        _swap_chain_frame_buffers[index] = _device->device()->createFramebuffer(desc);
    }
}

