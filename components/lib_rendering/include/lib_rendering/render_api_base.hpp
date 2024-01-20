#pragma once

#include <core_sdk/logger.hpp>
#include <lib_rendering/common/render_command.hpp>
#include <lib_rendering/common/types.hpp>

namespace lib::rendering
{
// defaults
inline constexpr lib::point2Di default_window_size = lib::point2Di{1280, 720};
inline constexpr uint8_t texture_pixel_size = 4;

static_assert(sizeof(uint32_t) == texture_pixel_size);

inline constexpr uint8_t frame_buffer_vertex_count = 6;
inline constexpr vertex_t frame_buffer_vertices[] =
{
	vertex_t{{-1, 1}, {255, 255, 255, 255}, {0, 1}},
	vertex_t{{1, 1}, {255, 255, 255, 255}, {1, 1}},
	vertex_t{{1, -1}, {255, 255, 255, 255}, {1, 0}},

	vertex_t{{1, -1}, {255, 255, 255, 255}, {1, 0}},
	vertex_t{{-1, -1}, {255, 255, 255, 255}, {0, 0}},
	vertex_t{{-1, 1}, {255, 255, 255, 255}, {0, 1}},
};

static_assert(sizeof(frame_buffer_vertices) == sizeof(vertex_t) * frame_buffer_vertex_count);

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

//! all render API implementations must inherit this class
class render_api_base
{
public:
	render_api_base(void* api_context, bool flush_buffers)
		: _api_context(api_context), _flush_buffers(flush_buffers)
	{
	}

	virtual ~render_api_base() = default;

	//! bind a texture atlas using data of width and height, texture is RGBA
	virtual void bind_atlas(const uint8_t* data, int width, int height) = 0;

	//! Update screen size, this should be called from the renderer.
	virtual void update_screen_size(const lib::point2Di& window_size) = 0;

	//! draw our render command to the frame buffer
	virtual void update_frame_buffer(const render_command& render_command) = 0;

	//! draw the frame buffer to the screen
	virtual void draw_frame_buffer() = 0;

protected:
	void* _api_context;
	bool _flush_buffers;

	lib::point2Di _window_size = {};
};
}  // namespace lib::rendering