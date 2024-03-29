#pragma once

#include <vector>
#include <lib_rendering/common/types.hpp>

struct stbrp_rect;

namespace lib::rendering
{
//! generates a texture atlas
class atlas_generator
{
public:
	atlas_generator();
	~atlas_generator();

	void reset();

	//! build the texture atlas with all current textures
	bool build_atlas();

	//! add a texture to our atlas, in the form of a byte array using the layout ABGR
	[[nodiscard]] texture_id add_texture(const std::vector<uint8_t>& data, int width, int height);

	//! get properties for a specific texture id
	[[nodiscard]] const texture_properties_t& get_texture_properties(texture_id id) const;

	[[nodiscard]] int get_width() const;
	[[nodiscard]] int get_height() const;
	[[nodiscard]] const std::vector<uint8_t>& get_byte_buffer() const;

private:
	std::vector<std::vector<uint8_t>> _texture_data = {};
	std::vector<uint8_t> _data = {};

	// uhhhg duplicate data, kms
	std::vector<stbrp_rect> _texture_rects = {};
	std::vector<texture_properties_t> _texture_properties = {};

	int _width = 0;
	int _height = 0;
};
}