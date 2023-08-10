#pragma once

#include <vector>
#include <glad/glad.h>

namespace lib::backend::gl3
{
	struct vertex_layout_t
	{
		vertex_layout_t() :
			_count(0), _offset(0), _param_type(0), _normalized(false)
		{
		}

		vertex_layout_t(uint32_t count, uint32_t offset, GLenum type, bool normalized) :
			_count(count), _offset(offset), _param_type(type), _normalized(normalized)
		{
		}

		uint32_t _count;
		uint32_t _offset;
		GLenum _param_type;
		bool _normalized;
	};

	class vertex_layout_manager
	{
	public:
		//! Add a new member that belongs to our vertex struct.
		void add_layout(uint32_t count, GLenum type, bool normalized);

		//! Get the layout of our vertex.
		[[nodiscard]] const std::vector<vertex_layout_t>& get_layouts() const;

	private:
		uint32_t _current_offset = 0;
		std::vector<vertex_layout_t> _layouts = {};
	};
}