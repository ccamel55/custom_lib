#pragma once

#include <module_core/NoCopy.hpp>
#include <module_core/type/point/point2D.hpp>

#include <array>
#include <expected>
#include <filesystem>
#include <string>
#include <vector>

namespace lib::render {

// Renderable characters start from 32 and go upto 126
constexpr uint8_t CHAR_START    = 32;
constexpr uint8_t CHAR_END      = 127;

struct font_character_t {
    point2Di align;
    point2Di spacing;
    point2Df atlas_start;
    point2Df atlas_end;
};

struct font_properties_t {
    point2Di atlas_size;
    std::vector<uint8_t> atlas;
    std::array<font_character_t, CHAR_END - CHAR_START> character;
};

class FontFactory : public NoCopy {
public:
    explicit FontFactory(std::filesystem::path font_folder);

    [[nodiscard]] std::expected<font_properties_t, std::string> load_font(
        const std::filesystem::path& font_path,
        float height
    ) const ;

    [[nodiscard]] static std::expected<font_properties_t, std::string> load_font(
        const uint8_t* font_data,
        float height
    );

private:
    std::filesystem::path _font_folder;

};

}