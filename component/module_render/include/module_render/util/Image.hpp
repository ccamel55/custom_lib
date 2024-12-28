#pragma once

#include <dep_nvrhi/nvrhi.hpp>

namespace lib::render {

template <typename Image_Id, size_t Num_Ids>
requires std::is_scoped_enum_v<Image_Id>
class Image {
public:
    explicit Image(
        nvrhi::IDevice* device
    )
        : _device(device) {

    }

    nvrhi::ITexture* operator[](Image_Id id) const {
        return _images.at(static_cast<size_t>(id));
    }

    void back_buffer_resized(std::function<void(std::array<nvrhi::TextureHandle, Num_Ids>&)> cb) {
        cb(_images);
    }

protected:
    nvrhi::IDevice* _device                             = nullptr;
    std::array<nvrhi::TextureHandle, Num_Ids> _images   = {};

};

}