#include <backend/render/common/image_loader.hpp>
#include <common/logger.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "backend/render/common/stb/stb_image.hpp"

using namespace lib::backend::render;

image_loader::image_loader(const std::filesystem::path& image) : _filepath(image), _data(nullptr), _width(0), _height(0)
{
}

uint8_t* image_loader::generate_byte_array()
{
	int _channels;

	// open image with stb_image and generate a pointer to "data"
	_data = stbi_load(_filepath.string().c_str(), &_width, &_height, &_channels, STBI_rgb_alpha);

	return _data;
}

void image_loader::free_byte_array()
{
	if (!_data)
	{
		lib_log_e("image_loader: image data does not exist, image was not loaded correctly");
		assert(false);
	}

	stbi_image_free(_data);
	_data = nullptr;
}

int image_loader::get_width() const
{
	if (!_data)
	{
		lib_log_e("image_loader: image data does not exist, image was not loaded correctly");
		assert(false);
	}

	return _width;
}

int image_loader::get_height() const
{
	if (!_data)
	{
		lib_log_e("image_loader: image data does not exist, image was not loaded correctly");
		assert(false);
	}

	return _height;
}
