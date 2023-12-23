#pragma once

#include <filesystem>
#include <lib_rendering/common/types.hpp>

namespace lib::rendering
{
//! generate SDF font and generate atlas mappings
class font_loader
{
public:
    font_loader(const uint8_t* font_data, float weight, float height);
    ~font_loader();

    [[nodiscard]] int get_width() const;
    [[nodiscard]] int get_height() const;
    [[nodiscard]] const uint8_t* get_byte_buffer() const;

private:
    uint8_t* _data;

    int _width;
    int _height;
};
}  // namespace lib::rendering