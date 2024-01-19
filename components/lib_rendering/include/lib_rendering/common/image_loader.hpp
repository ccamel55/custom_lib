#pragma once

#include <filesystem>

namespace lib::rendering
{
//! Open, resize and import an image then return it as a byte array/
class image_loader
{
public:
	explicit image_loader(const std::filesystem::path& image);
	
	[[nodiscard]] int get_width() const;
	[[nodiscard]] int get_height() const;
	[[nodiscard]] const std::vector<uint8_t>& get_byte_buffer() const;

private:
	std::filesystem::path _filepath = {};
	std::vector<uint8_t> _data = {};

	int _width = 0;
	int _height = 0;
};
}  // namespace lib::rendering