#pragma once

#include <dep_nvrhi/nvrhi.hpp>

namespace lib::render {

class TextureFactory {
public:
    explicit TextureFactory(const nvrhi::DeviceHandle& device);

private:
    nvrhi::DeviceHandle _device;

};

}