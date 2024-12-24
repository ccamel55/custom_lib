#include <module_render/util/TextureFactory.hpp>
#include <module_system/filesystem.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <module_render/stb/stb_image.hpp>

using namespace lib::render;

namespace {

    [[nodiscard]] int stb_color_type(const TextureColor color) {
        switch (color) {
            // case TextureColor::GrayScale:
            //     return STBI_grey_alpha;
            case TextureColor::RGBA:
                return STBI_rgb_alpha;
        }
    }

    [[nodiscard]] nvrhi::Format texture_format(const TextureColor color) {
        switch (color) {
            case TextureColor::RGBA:
                return nvrhi::Format::SRGBA8_UNORM;
        }
    }

}

TextureFactory::TextureFactory(nvrhi::IDevice* device)
    : _device(device) {

}

std::expected<nvrhi::TextureHandle, std::string> TextureFactory::create_texture(
    const std::filesystem::path& path,
    const TextureColor color
) const {
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
        absolute_path.c_str(),
        &width,
        &height,
        &channels,
        required_channels
   );

    if (image_data == nullptr) {
        return std::unexpected("Failed to load image from disk");
    }

    // Create render API texture
    nvrhi::TextureDesc desc;
    {
        desc.debugName          = path.filename();
        desc.dimension          = nvrhi::TextureDimension::Texture2D;
        desc.width              = width;
        desc.height             = height;
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
        command_list->writeTexture(texture, 0, 0, image_data, width * required_channels);
        command_list->setPermanentTextureState(texture, nvrhi::ResourceStates::ShaderResource);
        command_list->commitBarriers();
    }
    command_list->close();
    _device->executeCommandList(command_list);

    stbi_image_free(image_data);

    return texture;
}
