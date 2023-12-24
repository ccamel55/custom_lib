#pragma once

#include <filesystem>
#include <lib_rendering/common/types.hpp>

namespace lib::rendering
{
//! generate SDF font and generate atlas mappings
class font_loader
{
public:
    font_loader(const uint8_t* font_data, float height);
    ~font_loader();

    [[nodiscard]] const font_data_t& get_font_data(font_id id) const;

private:
    std::array<font_data_t, 127 - 32> _font_data;

};
}  // namespace lib::rendering