#pragma once
#include <cstdint>

namespace lib::backend::render
{
	struct batch_t
	{
		batch_t() :
			_vertex_count(0), _primitive_type(0), _texture_id(0)
		{
		}

		explicit batch_t(int primitive_type, int texture_id = -1) :
			_vertex_count(0), _primitive_type(primitive_type), _texture_id(texture_id)
		{
		}

		int _vertex_count;
		int _primitive_type;
		int _texture_id;
	};
}