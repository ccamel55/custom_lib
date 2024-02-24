#pragma once

#include <core_sdk/logger.hpp>
#include <lib_rendering/common/render_command.hpp>
#include <lib_rendering/common/types.hpp>

namespace lib::rendering
{
// defaults
inline const static lib::point2Di default_window_size = lib::point2Di{1280, 720};
inline constexpr uint8_t texture_pixel_size = 4;

static_assert(sizeof(uint32_t) == texture_pixel_size);

// sometimes if this is too small we might end up with some weird artifacts
inline constexpr uint8_t opaque_texture_width = 2;
inline constexpr uint8_t opaque_texture_height = 2;

// this should be done based off opaque_texture_width and opaque_texture_height but making it dynamic
// seems way to overkill for this purpose lol, (given as RGBA thus 4 x width x height)
inline std::vector<uint8_t> opaque_texture_data =
{
	0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff,
};

// must be implemented by render_api impl class
struct render_api_data_t;

//! all render API implementations must inherit this class
class render_api_base
{
public:
	explicit render_api_base(bool flush_buffers)
		: _flush_buffers(flush_buffers)
	{
	}

	virtual ~render_api_base() = default;

	//! bind a texture atlas using data of width and height, texture is RGBA
	virtual void bind_atlas(const uint8_t* data, int width, int height) = 0;

	//! Update screen size, this should be called from the renderer.
	virtual void update_screen_size(const lib::point2Di& window_size) = 0;

	//! draw the frame buffer to the screen
	virtual void draw(const render_command& render_command) = 0;

protected:
	bool _flush_buffers;
	lib::point2Di _window_size = {};
};
}  // namespace lib::rendering