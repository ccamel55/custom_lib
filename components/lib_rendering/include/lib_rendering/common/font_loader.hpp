#pragma once

#include <filesystem>
#include <lib_rendering/common/types.hpp>

namespace lib::rendering
{
//! generate SDF font and generate atlas mappings
class font_loader
{
public:
    font_loader(font_properties_t& font_properties, const uint8_t* font_data, float height);
    [[nodiscard]] const font_internal_property_t& get_font_internal_property(uint8_t c) const;

private:
    std::array<font_internal_property_t, 127 - 32> _font_internal_properties = {};

};
}  // namespace lib::rendering