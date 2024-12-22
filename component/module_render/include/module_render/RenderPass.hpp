#pragma once

#include <module_core/type/point/point2D.hpp>

#include <module_render/FrameInterval.hpp>
#include <module_render/backend/Device_Common.hpp>

namespace lib::render {

class RenderPass {
public:
    explicit RenderPass(Device_Common* backend)
        : _backend(backend) {

    };

    virtual ~RenderPass() = default;

    virtual void update_frame(const FrameInterval& interval) = 0;
    virtual void render(nvrhi::IFramebuffer* frame_buffer) = 0;
    virtual void back_buffer_resizing() = 0;
    virtual void back_buffer_resized(const point2Di& size) = 0;

protected:
    [[nodiscard]] nvrhi::IDevice* device() const {
        return _backend->device();
    }

protected:
    // We start dealing with raw points etc. here because this needs to be as low overhead as possible.
    Device_Common* _backend;

};
}