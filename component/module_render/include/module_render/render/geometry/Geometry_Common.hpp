#pragma once

#include <module_core/NoCopy.hpp>
#include <module_render/backend/Device_Common.hpp>

#include <module_render/util/FrameBuffer.hpp>
#include <module_render/util/Image.hpp>
#include <module_render/util/Pipeline.hpp>

namespace lib::render {

enum class FrameBuffer_Id: uint32_t {
    Geometry,

    // Must always be last
    Num_FrameBuffer_Id
};

enum class Image_Id: uint32_t {
    Geometry_ColorTarget,
    Geometry_DepthTarget,

    // Must always be last
    Num_Image_Id
};

enum class Pipeline_Id: uint32_t {
    Geometry_Texture,

    // Must always be last
    Num_Pipeline_Id
};

using Geometry_FrameBuffer  = FrameBuffer<FrameBuffer_Id, static_cast<size_t>(FrameBuffer_Id::Num_FrameBuffer_Id)>;
using Geometry_Image        = Image<Image_Id, static_cast<size_t>(Image_Id::Num_Image_Id)>;
using Geometry_Pipeline     = Pipeline<Pipeline_Id, static_cast<size_t>(Pipeline_Id::Num_Pipeline_Id)>;

struct geometry_batch_t {

};

class Geometry_Common : public NoCopy {
public:
    explicit Geometry_Common(nvrhi::DeviceHandle device)
        : _device(std::move(device)) {

    };
    virtual ~Geometry_Common() = default;

    virtual void draw_geometry(nvrhi::IFramebuffer* frame_buffer) = 0;
    virtual void back_buffer_resizing() = 0;
    virtual void back_buffer_resized(const point2Di& size) = 0;

protected:
    nvrhi::DeviceHandle _device;
    std::vector<geometry_batch_t> _batch;

};

}