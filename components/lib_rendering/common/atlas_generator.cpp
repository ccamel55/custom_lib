#include <lib_rendering/common/atlas_generator.hpp>
#include <lib_rendering/render_api_base.hpp>

#define STB_RECT_PACK_IMPLEMENTATION
#include <core_sdk/logger/logger.hpp>
#include <lib_rendering/common/stb/stb_rect_pack.hpp>

using namespace lib::rendering;

namespace
{
	constexpr uint16_t default_width = 1024;
	constexpr uint16_t default_height = 1024;
}

atlas_generator::atlas_generator()
	: _width(default_width), _height(default_height)
{
	_texture_data.reserve(128);
	_texture_rects.reserve(128);
	_texture_properties.reserve(128);
}

atlas_generator::~atlas_generator()
{
	// because we predelcare some rect pact stuff in the header we need to explictly define the
	// desctructor else we get linking issues :(
	reset();
}

void atlas_generator::reset()
{
	_data.clear();
	_texture_data.clear();
	_texture_rects.clear();
	_texture_properties.clear();

	_width = default_width;
	_height = default_height;
}

texture_id atlas_generator::add_texture(const std::vector<uint8_t>& data, int width, int height)
{
	// ensure each array is same sized else wise our ID screws up
	assert(_texture_rects.size() == _texture_data.size());
	assert(_texture_rects.size() == _texture_properties.size());

	if (width == 0 || height == 0)
    {
		lib_log_e("atlas_generator: tried to a texture with width or height of 0");
		assert(false);
    }

	lib_log_d("atlas_generator: added texture with size {}x{}", width, height);

	// make a local copy for us to use later
	_texture_properties.emplace_back();
	_texture_data.emplace_back(data);

	const auto id = static_cast<texture_id>(_texture_properties.size() - 1);
	auto& texture_rect = _texture_rects.emplace_back();

	texture_rect.w = width;
	texture_rect.h = height;

	return id;
}

bool atlas_generator::build_atlas()
{
	if (_texture_rects.empty()) [[unlikely]]
	{
		return false;
	}

	assert(_texture_rects.size() == _texture_data.size());
	assert(_texture_rects.size() < INT_MAX);

	// generate position for each rect by using packer
	stbrp_context rect_pack_context = {};

	std::vector<stbrp_node> packed_nodes = {};
	packed_nodes.resize(_texture_rects.size());

	// keep trying to pack all rects, if we have at least one unpacked, then double the size of the texture
	// until everything fits
	bool all_rects_fit = false;

	while (!all_rects_fit)
	{
		lib_log_d("atlas_generator: trying to pack all textures into a {}x{} texture", _width, _height);

		stbrp_init_target(&rect_pack_context, _width, _height, packed_nodes.data(), static_cast<int>(packed_nodes.size()));
		stbrp_pack_rects(&rect_pack_context, _texture_rects.data(), static_cast<int>(_texture_rects.size()));

		// assume true, but if we come across unpacked rect then set to false
		all_rects_fit = true;

		for (const auto& rect: _texture_rects)
		{
			if (!rect.was_packed)
			{
				// try again
				all_rects_fit = false;

				_width *= 2;
				_height *= 2;

				break;
			}
		}
	}

	// generate atlas
	_data.resize(_width * _height * texture_pixel_size);

	const auto data_as_uint32 = reinterpret_cast<uint32_t*>(_data.data());
	std::fill_n(data_as_uint32, _width * _height, 0x00000000);

	for (size_t i = 0; i < _texture_rects.size(); i++)
	{
		const auto& texture_rect = _texture_rects.at(i);

		// only draw packed rects
		if (texture_rect.w == 0 || texture_rect.h == 0 || !texture_rect.was_packed)
		{
			lib_log_e("atlas_generator: one or more textures could not be packed");
			assert(false);
		}

		// write to texture atlas
		const auto texture_data = reinterpret_cast<uint32_t*>(_texture_data.at(i).data());

		for (int y = 0; y < texture_rect.h; y++)
		{
			for (int x = 0; x < texture_rect.w; x++)
			{
				const auto data_index = (_width * (texture_rect.y + y)) + texture_rect.x + x;
				const auto bitmap_index = (texture_rect.w * y) + x;

				data_as_uint32[data_index] = texture_data[bitmap_index];
			}
		}

		// calculate texture properties, texture_rect.id should equal i
		auto& texture_properties = _texture_properties.at(i);

		texture_properties.start_pixel.x = texture_rect.x;
		texture_properties.start_pixel.y = texture_rect.y;

		texture_properties.size_pixel.x = texture_rect.w;
		texture_properties.size_pixel.y = texture_rect.h;

		texture_properties.start_normalised.x = static_cast<float>(texture_rect.x) / static_cast<float>(_width);
		texture_properties.start_normalised.y = static_cast<float>(texture_rect.y) / static_cast<float>(_height);

		texture_properties.end_normalised.x =
			static_cast<float>(texture_rect.x + texture_rect.w) / static_cast<float>(_width);

		texture_properties.end_normalised.y =
			static_cast<float>(texture_rect.y + texture_rect.h) / static_cast<float>(_height);
	}

	return true;
}

int atlas_generator::get_width() const
{
	return _width;
}

int atlas_generator::get_height() const
{
	return _height;
}

const std::vector<uint8_t>& atlas_generator::get_byte_buffer() const
{
	return _data;
}

const texture_properties_t& atlas_generator::get_texture_properties(texture_id id) const
{
	return _texture_properties.at(id);
}


