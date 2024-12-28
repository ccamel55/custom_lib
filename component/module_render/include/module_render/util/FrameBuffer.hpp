#pragma once

#include <dep_nvrhi/nvrhi.hpp>

namespace lib::render {

template <typename Buffer_Id, size_t Num_Ids>
requires std::is_scoped_enum_v<Buffer_Id>
class FrameBuffer {
public:
    explicit FrameBuffer(
        nvrhi::IDevice* device
    )
        : _device(device) {

    }

    nvrhi::IFramebuffer* operator[](Buffer_Id id) const {
        return _frame_buffer.at(static_cast<size_t>(id));
    }

    void back_buffer_resized(std::function<void(std::array<nvrhi::FramebufferHandle, Num_Ids>&)> cb) {
        cb(_frame_buffer);
    }

protected:
    nvrhi::IDevice* _device                                     = nullptr;
    std::array<nvrhi::FramebufferHandle, Num_Ids> _frame_buffer = {};

};

}