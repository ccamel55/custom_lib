#pragma once

#include <vector>
#include <array>

#include <backend/render/common/batch.hpp>
#include <backend/render/common/vertex.hpp>

namespace lib::backend::render
{
	constexpr size_t MAX_VERTEX_COUNT = 1024 * 4;

	struct render_batch_t
	{
		render_batch_t() :
			_total_vertex_count(0), _vertices({}), _batches({})
		{
		}

		size_t _total_vertex_count;
		std::array<vertex_2d_t, MAX_VERTEX_COUNT> _vertices;
		std::vector<batch_t> _batches;
	};
}