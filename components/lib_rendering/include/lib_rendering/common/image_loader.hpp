#pragma once

#include <filesystem>

namespace lib::rendering
{
//! Open, resize and import an image then return it as a byte array/
class image_loader
{
public:
	image_loader (const image_loader&) = delete;
	image_loader& operator= (const image_loader&) = delete;

	explicit image_loader(const std::filesystem::path& image);
	~image_loader();
	
	[[nodiscard]] int get_width() const;
	[[nodiscard]] int get_height() const;
	[[nodiscard]] uint8_t* get_byte_buffer() const;

private:
	std::filesystem::path _filepath;
	uint8_t* _data;

	int _width;
	int _height;
};
}  // namespace lib::rendering