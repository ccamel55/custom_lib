#pragma once

#include <backend/render/common/render_command.hpp>
#include <backend/render/common/types.hpp>

#include <vector>

namespace lib::backend::render
{
//! all render API implementations must inherit this class
class render_api_base
{
public:
	virtual ~render_api_base() = default;

	//! create a texture and a texture to the texture atlas
	//! \return id for texture, so that we can retrieve its properties for use later
	virtual texture_id add_texture() = 0;

	//! Update screen size, this should be called from the renderer.
	virtual void update_screen_size(const common::point2Di& window_size) = 0;

	//! render our render_command_t instance
	virtual void draw_render_command(const render_command& render_command) = 0;

protected:
	//! contains all our texture information, in api impl there must be another
	//! vector of same size that indexes each textures handle for the api
	std::vector<texture_properties_t> _texture_properties = {};
};
}  // namespace lib::backend::render