#include <module_render/util/FontFactory.hpp>
#include <module_system/filesystem.hpp>

#define STB_RECT_PACK_IMPLEMENTATION
#include <module_render/stb/stb_rect_pack.hpp>

#define STB_TRUETYPE_IMPLEMENTATION
#include <module_render/stb/stb_truetype.hpp>

using namespace lib::render;

namespace {

constexpr int ON_EDGE_VALUE = 128;
constexpr int PADDING = 4;

constexpr float PIXEL_DIST_SCALE = static_cast<float>(ON_EDGE_VALUE) / static_cast<float>(PADDING);

constexpr uint16_t DEFAULT_ATLAS_WIDTH  = 512;
constexpr uint16_t DEFAULT_ATLAS_HEIGHT = 512;

}

FontFactory::FontFactory(std::filesystem::path font_folder)
    : _font_folder(std::move(font_folder)) {

}

std::expected<font_properties_t, std::string> FontFactory::load_font(
    const std::filesystem::path& font_path,
    const float height
) const {
    const std::filesystem::path full_path = (_font_folder / font_path).replace_extension(".ttf");
    if (!exists(full_path) || is_directory(full_path)) {
        return std::unexpected("Font file does not exist: " + full_path.string());
    }

    const std::filesystem::path absolute_path = absolute(full_path);
    const std::vector<char> font_data = system::read_file_as_bytes(absolute_path);
    if (font_data.empty()) {
        return std::unexpected("Could not read font file: " + absolute_path.string());
    }

    return load_font(reinterpret_cast<const uint8_t*>(font_data.data()), height);
}

std::expected<font_properties_t, std::string> FontFactory::load_font(
    const uint8_t* font_data,
    const float height
) {
    constexpr uint8_t ATLAS_CHANNELS = 4;

    stbtt_fontinfo font_info;
    if (stbtt_InitFont(&font_info, font_data, 0) == 0) {
        return std::unexpected("Font data is not valid true type font");
    }

    // Find scaling needed to achieve desired height
    const float height_scale = stbtt_ScaleForPixelHeight(&font_info, height);

    // For each drawable character, we will render to our texture atlas.
    // Note: in the future we should use rect packing for this/put into a global font atlas.
    font_properties_t properties = {};

    std::array<std::vector<uint8_t>, CHAR_END - CHAR_START> tmp_bitmap;
    std::array<stbrp_rect, CHAR_END - CHAR_START> tmp_size = {};

    for (uint8_t c = CHAR_START; c < CHAR_END; c++) {

        auto& [align, spacing, _1, _2] = properties.character.at(c - CHAR_START);
        auto& size = tmp_size.at(c - CHAR_START);

        uint8_t* bitmap_ptr = stbtt_GetCodepointSDF(
            &font_info,
            height_scale,
            c,
            PADDING,
            ON_EDGE_VALUE,
            PIXEL_DIST_SCALE,
            &size.w,
            &size.h,
            &align.x,
            &align.y
        );

        if (bitmap_ptr != nullptr && size.w > 0 && size.h > 0) {
            // Load spacing X and Y
            stbtt_GetCodepointHMetrics(&font_info, c, &spacing.x, nullptr);
            stbtt_GetFontVMetrics(&font_info, &spacing.y, nullptr, nullptr);

            spacing.x = static_cast<int>(std::roundf(static_cast<float>(spacing.x) * height_scale));
            spacing.y = static_cast<int>(std::roundf(static_cast<float>(spacing.y) * height_scale));

            // Allocate data for abgr bitmap then cast as uint32 to make working with data much easier
            auto& bitmap = tmp_bitmap.at(c - CHAR_START);
            bitmap.resize(size.w * size.h * ATLAS_CHANNELS, 0);

            const auto tmp_bitmap_uint32 = reinterpret_cast<uint32_t*>(bitmap.data());

            for (int y = 0; y < size.h; y++) {
                for (int x = 0; x < size.w; x++) {
                    const auto bitmap_index = (size.w * y) + x;
                    if (const auto val = bitmap_ptr[bitmap_index]; val > 0) {
                        tmp_bitmap_uint32[bitmap_index] = 0x00FFFFFF;
                        tmp_bitmap_uint32[bitmap_index] |= (val << 24);
                    }
                }
            }
        }
        stbtt_FreeSDF(bitmap_ptr, nullptr);
    }

    // Do rect packing with font characters
    properties.atlas_size = { DEFAULT_ATLAS_WIDTH, DEFAULT_ATLAS_HEIGHT };

    stbrp_context rect_pack_context;
    std::array<stbrp_node, CHAR_END - CHAR_START> pack_nodes = {};

    bool pack_complete = false;
    while (!pack_complete) {

        // Try pack with current atlas size, if it fails, try pack again
        stbrp_init_target(
            &rect_pack_context,
            properties.atlas_size.x,
            properties.atlas_size.y,
            pack_nodes.data(),
            pack_nodes.size()
        );

        stbrp_pack_rects(
            &rect_pack_context,
            tmp_size.data(),
            tmp_size.size()
        );

        pack_complete = true;

        // Check that all rects where packed, if not double in size
        for (const auto& rect: tmp_size) {
            if (!rect.was_packed) {
                pack_complete = false;
                properties.atlas_size.x *= 2;
                properties.atlas_size.y *= 2;
                break;
            }
        }
    }

    // Generate texture atlas
    properties.atlas.resize(properties.atlas_size.x * properties.atlas_size.y * ATLAS_CHANNELS, 0);
    const auto atlas_uint32 = reinterpret_cast<uint32_t*>(properties.atlas.data());

    for (size_t i = 0; i < tmp_size.size(); i++) {
        const stbrp_rect& rect = tmp_size.at(i);
        if (rect.w == 0 || rect.h == 0 || !rect.was_packed) {
            continue;
        }

        const auto bitmap_uint32 = reinterpret_cast<uint32_t*>(tmp_bitmap.at(i).data());
        for (int y = 0; y < rect.h; y++) {
            for (int x = 0; x < rect.w; x++) {
                const int data_index = (properties.atlas_size.x * (rect.y + y)) + rect.x + x;
                const int bitmap_index = (rect.w * y) + x;
                atlas_uint32[data_index] = bitmap_uint32[bitmap_index];
            }
        }

        // Update texture properties
        // calculate texture properties, texture_rect.id should equal i
        auto& [_1, _2, atlas_start, atlas_end] = properties.character.at(i);

        atlas_start = {
            static_cast<float>(rect.x) / static_cast<float>(properties.atlas_size.x),
            static_cast<float>(rect.y) / static_cast<float>(properties.atlas_size.y)
        };

        atlas_end = {
            static_cast<float>(rect.x + rect.w) / static_cast<float>(properties.atlas_size.x),
            static_cast<float>(rect.y + rect.h) / static_cast<float>(properties.atlas_size.y)
        };
    }

    return properties;
}

