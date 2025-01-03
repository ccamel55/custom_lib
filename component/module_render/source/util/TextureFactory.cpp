#include <module_render/util/TextureFactory.hpp>
#include <module_system/filesystem.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <module_render/stb/stb_image.hpp>
#include <utility>

using namespace lib::render;

namespace {

    [[nodiscard]] int stb_color_type(const TextureColor color) {
        switch (color) {
            case TextureColor::GrayScale:
                return STBI_grey;
            case TextureColor::RGBA:
                return STBI_rgb_alpha;
        }
        return STBI_default;
    }

    [[nodiscard]] nvrhi::Format texture_format(const TextureColor color) {
        switch (color) {
            case TextureColor::GrayScale:
                return nvrhi::Format::R8_UNORM;
            case TextureColor::RGBA:
                return nvrhi::Format::SRGBA8_UNORM;
        }
        return nvrhi::Format::UNKNOWN;
    }

}

TextureFactory::TextureFactory(nvrhi::IDevice* device, std::filesystem::path  texture_folder)
    : _device(device)
    , _texture_folder(std::move(texture_folder)) {

}

std::expected<nvrhi::TextureHandle, std::string> TextureFactory::create_texture(
    std::filesystem::path path,
    const TextureColor color
) const {

    path = _texture_folder / path;

    if (!exists(path) || is_directory(path)) {
        return std::unexpected("Texture file does not exist: " + path.string());
    }

    const auto absolute_path = absolute(path);

    // Load image using STBI
    int width = 0;
    int height = 0;

    [[maybe_unused]] int channels;

    const int required_channels = stb_color_type(color);

    uint8_t* image_data = stbi_load(
        absolute_path.string().c_str(),
        &width,
        &height,
        &channels,
        required_channels
   );

    if (image_data == nullptr) {
        return std::unexpected("Failed to load image from disk");
    }

    const std::vector image_data_vec(image_data, image_data + width * height * required_channels);
    stbi_image_free(image_data);

    return create_texture(image_data_vec.data(), { width, height }, color);
}

std::expected<nvrhi::TextureHandle, std::string> TextureFactory::create_texture(
    const uint8_t* bytes,
    const point2Di& size,
    const TextureColor color
) const {

    const int required_channels = stb_color_type(color);

    // Create render API texture
    nvrhi::TextureDesc desc;
    {
        // desc.debugName          = path.filename().string();
        desc.dimension          = nvrhi::TextureDimension::Texture2D;
        desc.width              = size.x;
        desc.height             = size.y;
        desc.format             = texture_format(color);
        desc.initialState       = nvrhi::ResourceStates::ShaderResource;
        desc.keepInitialState   = true;
    }
    nvrhi::TextureHandle texture = _device->createTexture(desc);

    // Map texture data to nvrhi texture (load it onto GPU)
    const auto command_list = _device->createCommandList();
    command_list->open();
    {
        command_list->beginTrackingTextureState(texture, nvrhi::AllSubresources, nvrhi::ResourceStates::Common);
        command_list->writeTexture(texture, 0, 0, bytes, size.x * required_channels);
        command_list->setPermanentTextureState(texture, nvrhi::ResourceStates::ShaderResource);
        command_list->commitBarriers();
    }
    command_list->close();
    _device->executeCommandList(command_list);

    return texture;
}