#pragma once

#include <backend/render/common/types.hpp>
#include <filesystem>

namespace lib::backend::render
{
//! Open, resize and import an image then return it as a byte array/
class image_loader
{
public:
	explicit image_loader(const std::filesystem::path& image);

	[[nodiscard]] uint8_t* generate_byte_array();
	void free_byte_array();

	[[nodiscard]] int get_width() const;
	[[nodiscard]] int get_height() const;

private:
	std::filesystem::path _filepath;
	uint8_t* _data;

	int _width;
	int _height;
};
}  // namespace lib::backend::render