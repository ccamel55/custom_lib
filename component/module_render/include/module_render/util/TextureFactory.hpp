#pragma once

#include <dep_nvrhi/nvrhi.hpp>
#include <module_core/NoCopy.hpp>

#include <filesystem>

namespace lib::render {

enum class TextureColor {
    // GrayScale,
    RGBA,
};

class TextureFactory : public NoCopy {
public:
    explicit TextureFactory(const nvrhi::DeviceHandle& device);

    [[nodiscard]] std::expected<nvrhi::TextureHandle, std::string> create_texture(
        const std::filesystem::path& path,
        TextureColor color
    ) const;

private:
    nvrhi::DeviceHandle _device;

};

}