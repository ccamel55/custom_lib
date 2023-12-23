#include <core_sdk/logger.hpp>
#include <lib_rendering/common/image_loader.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <lib_rendering/common/stb/stb_image.hpp>

#include <cassert>

using namespace lib::rendering;

image_loader::image_loader(const std::filesystem::path& image) :
	_filepath(image), _data(nullptr), _width(0), _height(0)
{
	// open image with stb_image and generate a pointer to "data"

	int _channels;
	_data = stbi_load(_filepath.string().c_str(), &_width, &_height, &_channels, STBI_rgb_alpha);
}

image_loader::~image_loader()
{
	stbi_image_free(_data);
	_data = nullptr;
}

int image_loader::get_width() const
{
	return _width;
}

int image_loader::get_height() const
{
	return _height;
}

uint8_t* image_loader::get_byte_buffer() const
{
	return _data;
}

