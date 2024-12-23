#pragma once

#include <module_core/NoCopy.hpp>
#include <module_render/backend/Device_Common.hpp>

namespace lib::render {

class Geometry_Common : public NoCopy {
public:
    explicit Geometry_Common(const nvrhi::DeviceHandle& device)
        : _device(device) {

    };
    virtual ~Geometry_Common() = default;

    virtual void draw_geometry(nvrhi::IFramebuffer* frame_buffer) = 0;
    virtual void back_buffer_resizing() = 0;

protected:
    nvrhi::DeviceHandle _device;

};

}