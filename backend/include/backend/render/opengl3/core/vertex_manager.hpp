#pragma once

#include <array>

#include <backend/render/common/vertex.hpp>
#include <backend/render/common/render_batch.hpp>
#include <backend/render/opengl3/core/vertex_layout_manager.hpp>

namespace lib::backend::gl3
{
	class vertex_manager
	{
	public:
		vertex_manager();
		vertex_manager(size_t max_vertices, uint32_t size_of_vertex, vertex_layout_manager& vertex_layout);

		~vertex_manager();

		//! Bind Vertices to render state.
		void bind() const;

		//! Unbind vertices to render state.
		void unbind() const;

		//! Bind vertices to buffer.
		void bind_buffer_data(const std::array<render::vertex_2d_t, render::MAX_VERTEX_COUNT>& vertices) const;

	private:
		uint32_t _vertex_buffer_id = 0;
		uint32_t _vertex_array_id = 0;
	};
}