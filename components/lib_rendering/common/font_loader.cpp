#include <lib_rendering/common/font_loader.hpp>
#include <lib_rendering/render_api_base.hpp>

#include <core_sdk/logger/logger.hpp>

#define STB_TRUETYPE_IMPLEMENTATION
#include <lib_rendering/common/stb/stb_truetype.hpp>

using namespace lib::rendering;

namespace
{
constexpr int on_edge_value = 128;
constexpr int padding = 4;

constexpr float pixel_dist_scale =
	static_cast<float>(on_edge_value) / static_cast<float>(padding);
}

font_loader::font_loader(font_properties_t& font_properties, const uint8_t* font_data, float height)
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
		auto& internal_property = _font_internal_properties.at(character - 32);
		auto& font_property = font_properties.at(character - 32);

		const auto stb_bitmap = stbtt_GetCodepointSDF(
					&font_info,
					scale,
					character,
					padding,
					on_edge_value,
					pixel_dist_scale,
					&internal_property.size.x,
					&internal_property.size.y,
					&font_property.offset.x,
					&font_property.offset.y);

		stbtt_GetCodepointHMetrics(&font_info, character, &font_property.spacing.x, nullptr);
		stbtt_GetFontVMetrics(&font_info, &font_property.spacing.y, nullptr, nullptr);

		font_property.spacing.x = static_cast<int>(std::roundf(static_cast<float>(font_property.spacing.x) * scale));
		font_property.spacing.y = static_cast<int>(std::roundf(static_cast<float>(font_property.spacing.y) * scale));

		if (internal_property.size.x > 0 && internal_property.size.y > 0)
		{
			// convert int uint32 now, womp womp
			internal_property.data.resize(internal_property.size.x * internal_property.size.y * texture_pixel_size);

			const auto data_as_uint32 = reinterpret_cast<uint32_t*>(internal_property.data.data());
			std::fill_n(data_as_uint32, internal_property.size.x * internal_property.size.y, 0x00000000);

			for (int y = 0; y < internal_property.size.y; y++)
			{
				for (int x = 0; x < internal_property.size.x; x++)
				{
					// colors written as ABGR
					const auto bitmap_index = (internal_property.size.x * y) + x;

					if (const auto val = stb_bitmap[bitmap_index]; val > 0)
					{
						data_as_uint32[bitmap_index] = 0x00FFFFFF;
						data_as_uint32[bitmap_index] |= (val << 24);
					}
				}
			}
		}

		stbtt_FreeSDF(stb_bitmap, nullptr);
	}
}

const font_internal_property_t& font_loader::get_font_internal_property(uint8_t c) const
{
	return _font_internal_properties.at(c - 32);
}