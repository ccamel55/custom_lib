#pragma once

#include <dep_nvrhi/nvrhi.hpp>
#include <module_core/NoCopy.hpp>

#include <filesystem>
#include <module_core/type/point/point2D.hpp>

namespace lib::render {

enum class TextureColor {
    GrayScale,
    RGBA,
};

class TextureFactory : public NoCopy {
public:
    explicit TextureFactory(nvrhi::IDevice* device, std::filesystem::path  texture_folder);

    [[nodiscard]] std::expected<nvrhi::TextureHandle, std::string> create_texture(
        std::filesystem::path path,
        TextureColor color
    ) const;

    [[nodiscard]] std::expected<nvrhi::TextureHandle, std::string> create_texture(
        const std::vector<uint8_t>& bytes,
        const point2Di& size,
        TextureColor color
    ) const;

private:
    nvrhi::IDevice* _device;
    std::filesystem::path _texture_folder;

};

}
