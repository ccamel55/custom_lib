#include <lib_rendering/common/image_loader.hpp>
#include <lib_rendering/render_api_base.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <lib_rendering/common/stb/stb_image.hpp>

using namespace lib::rendering;

image_loader::image_loader(const std::filesystem::path& image)
	: _filepath(image)
{
	int channels;
	const auto data = stbi_load(
		_filepath.string().c_str(),
		&_width,
		&_height,
		&channels,
		STBI_rgb_alpha);

	// copy data into vector and free image data
	const auto image_size = _width * _height * texture_pixel_size;

	_data.resize(image_size);
	std::copy(&data[0], &data[image_size], _data.begin());

	stbi_image_free(data);
}

int image_loader::get_width() const
{
	return _width;
}

int image_loader::get_height() const
{
	return _height;
}

const std::vector<uint8_t>& image_loader::get_byte_buffer() const
{
	return _data;
}

