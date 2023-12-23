#include <lib_rendering/common/font_loader.hpp>
#include <core_sdk/logger.hpp>

#define STB_RECT_PACK_IMPLEMENTATION
#include <lib_rendering/common/stb/stb_rect_pack.hpp>

#define STB_TRUETYPE_IMPLEMENTATION
#include <lib_rendering/common/stb/stb_truetype.hpp>

using namespace lib::rendering;

namespace
{
constexpr int on_edge_value = 128;
constexpr int padding = 3;

constexpr float pixel_dist_scale = 32.f;

    struct font_internal_metrics_t
    {
        uint8_t* data = nullptr;

        int x_offset = 0;
        int y_offset = 0;

        int width = 0;
        int height = 0;
    };
}  // namespace

font_loader::font_loader(const uint8_t* font_data, float weight, float height)
{
	stbtt_fontinfo font_info = {};

	if (stbtt_InitFont(&font_info, font_data, 0) == 0)
	{
		lib_log_e("font_loader: failed to load font wop wop");
		assert(false);
	}

	// get scale we need to get the desired height
	const auto scale = stbtt_ScaleForPixelHeight(&font_info, height);

    int max_width = 0;
    int max_height = 0;

    // rect we will pack soon
	std::array<stbrp_rect, 127 - 32> packed_rect_rects = {};
	std::array<font_internal_metrics_t, 127 - 32> internal_metrics = {};

	// get metrics for all our drawable characters
	// ascii values 32 - 126 are all our drawable UTF8 characters
	for (uint8_t character = 32; character < 127; character++)
	{
		auto& rect = packed_rect_rects.at(character - 32);
		auto& metric = internal_metrics.at(character - 32);

		metric.data = stbtt_GetCodepointSDF(
			&font_info,
			scale,
			character,
			padding,
			on_edge_value,
			pixel_dist_scale,
			&metric.width,
			&metric.height,
			&metric.x_offset,
			&metric.y_offset);

        rect.w = metric.width;
        rect.h = metric.height;

		max_width = std::max(max_width, metric.width);
		max_height = std::max(max_height, metric.height);
	}

	// 10 x 10 gives us 100 code points, we only need 95
	_width = max_width * 10;
	_height = max_height * 10;

	// generate position for each rect by using packer
	stbrp_context rect_pack_context = {};
	std::array<stbrp_node, 127 - 32> rect_pack_nodes = {};

	stbrp_init_target(&rect_pack_context, _width, _height, rect_pack_nodes.data(), rect_pack_nodes.size());
	stbrp_pack_rects(&rect_pack_context, packed_rect_rects.data(), packed_rect_rects.size());

	// generate atlas
	_data = new uint8_t[_width * _height * 4];

	const auto data_as_uint32 = reinterpret_cast<uint32_t*>(_data);
	std::fill_n(data_as_uint32, _width * _height, 0x00000000);

	for (uint8_t character = 32; character < 127; character++)
	{
		const auto& metric = internal_metrics.at(character - 32);
		const auto& rect = packed_rect_rects.at(character - 32);

		// only draw packed rects
		if (metric.width == 0 || metric.height == 0 || !rect.was_packed)
		{
			continue;
		}

		for (int y = 0; y < metric.height; y++)
		{
			for (int x = 0; x < metric.width; x++)
			{
				// colors written as ABGR
				const auto data_index = (_width * (rect.y + y)) + rect.x + x;
				const auto bitmap_index = (metric.width * y) + x;

				if (const auto val = metric.data[bitmap_index]; val > 0)
				{
					data_as_uint32[data_index] = 0x00FFFFFF;
					data_as_uint32[data_index] |= (val << 24);
				}
			}
		}

		// free codepoint from memory
		stbtt_FreeSDF(metric.data, nullptr);
	}
}

font_loader::~font_loader()
{
    delete[] _data;
}

const uint8_t* font_loader::get_byte_buffer() const
{
	return _data;
}

int font_loader::get_width() const
{
	return _width;
}

int font_loader::get_height() const
{
	return _height;
}
