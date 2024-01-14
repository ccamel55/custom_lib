#pragma once

#include <core_sdk/logger.hpp>
#include <lib_rendering/common/render_command.hpp>
#include <lib_rendering/common/types.hpp>

namespace lib::rendering
{
//! all render API implementations must inherit this class
class render_api_base
{
public:
	virtual ~render_api_base() = default;

	//! bind a texture atlas using data of width and height, texture is RGBA
	virtual void bind_atlas(const uint8_t* data, int width, int height) = 0;

	//! Update screen size, this should be called from the renderer.
	virtual void update_screen_size(const lib::point2Di& window_size) = 0;

	//! draw our render command to the frame buffer
	virtual void update_frame_buffer(const render_command& render_command) = 0;

	//! draw the frame buffer to the screen
	virtual void draw_frame_buffer() = 0;
};
}  // namespace lib::rendering