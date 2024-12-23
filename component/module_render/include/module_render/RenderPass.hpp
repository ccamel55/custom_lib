#pragma once

#include <dep_nvrhi/nvrhi.hpp>

#include <module_core/NoCopy.hpp>
#include <module_core/type/point/point2D.hpp>
#include <module_render/FrameInterval.hpp>

namespace lib::render {

class RenderPass : public NoCopy {
public:
    explicit RenderPass(nvrhi::DeviceHandle device)
        : _device(std::move(device)) {

    }
    virtual ~RenderPass() = default;

    virtual void update_frame(const FrameInterval& interval) = 0;
    virtual void render(nvrhi::IFramebuffer* frame_buffer) = 0;
    virtual void back_buffer_resizing() = 0;
    virtual void back_buffer_resized(const point2Di& size) = 0;

protected:
    // We start dealing with raw points etc. here because this needs to be as low overhead as possible.
    nvrhi::DeviceHandle _device;

};
}
