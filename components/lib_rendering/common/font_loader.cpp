#include <lib_rendering/common/font_loader.hpp>
#include <core_sdk/logger.hpp>

#define STB_TRUETYPE_IMPLEMENTATION
#include <lib_rendering/common/stb/stb_truetype.hpp>

using namespace lib::rendering;

namespace
{
constexpr int on_edge_value = 128;
constexpr int padding = 2;

constexpr float pixel_dist_scale = 32.f;
}  // namespace

font_loader::font_loader(const uint8_t* font_data, float height)
{
	stbtt_fontinfo font_info = {};

	if (stbtt_InitFont(&font_info, font_data, 0) == 0)
	{
		lib_log_e("font_loader: failed to load font wop wop");
		assert(false);
	}

	// get scale we need to get the desired height
	const auto scale = stbtt_ScaleForPixelHeight(&font_info, height);

	// get metrics for all our drawable characters
	// ascii values 32 - 126 are all our drawable UTF8 characters
	for (uint8_t character = 32; character < 127; character++)
	{
		auto& metric = _font_data.at(character - 32);
		const auto sdf_bitmap = stbtt_GetCodepointSDF(
			&font_info,
			scale,
			character,
			padding,
			on_edge_value,
			pixel_dist_scale,
			&metric.width,
			&metric.height,
			&metric.offset_x,
			&metric.offset_y);

		if (metric.width >0 && metric.height > 0)
		{
			// convert int uint32 now, womp womp
			metric.data = new uint8_t[metric.width * metric.height * 4];

			const auto data_as_uint32 = reinterpret_cast<uint32_t*>(metric.data);
			std::fill_n(data_as_uint32, metric.width * metric.height, 0x00000000);

			for (int y = 0; y < metric.height; y++)
			{
				for (int x = 0; x < metric.width; x++)
				{
					// colors written as ABGR
					const auto bitmap_index = (metric.width * y) + x;

					if (const auto val = sdf_bitmap[bitmap_index]; val > 0)
					{
						data_as_uint32[bitmap_index] = 0x00FFFFFF;
						data_as_uint32[bitmap_index] |= (val << 24);
					}
				}
			}
		}

		stbtt_FreeSDF(sdf_bitmap, nullptr);
	}
}

font_loader::~font_loader()
{
	for (const auto& font_data: _font_data)
	{
		delete[] font_data.data;
	}
}

const font_data_t& font_loader::get_font_data(font_id id) const
{
	return _font_data.at(id - 32);
}