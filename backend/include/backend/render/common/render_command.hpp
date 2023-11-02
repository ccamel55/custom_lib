#pragma once

#include <backend/render/common/types.hpp>

namespace lib::backend::render
{
class render_api;

//! render batch of triangles we will pass onto the render API to process
class render_command
{
	friend class render_api;

public:
	render_command();

	//! called at the start or end of each frame to reset internal counters
	void reset();

	//! 1) called first
	//! add currently written vertices and indices to a render batch
	//! return start position of vertex buffer
	uint32_t prepare_batch(const common::point4Di& clipped_area);

	//! 2) called second
	//! resize vertices list and return iterator to start writing to vertices directly
	std::array<vertex_t, MAX_VERTICES>::iterator insert_vertices(uint32_t size);

	//! 3) called third
	//! resize indices list and return iterator to start writing to indices directly
	std::array<uint32_t, MAX_INDICES>::iterator insert_indices(uint32_t size);

private:
	uint32_t vertex_count = 0;
	std::array<vertex_t, MAX_VERTICES> vertices = {};

	uint32_t index_count = 0;
	std::array<uint32_t, MAX_INDICES> indices = {};

	uint32_t batch_count = 0;
	std::array<batch_t, 100> batches = {};

	std::array<batch_t, 100>::iterator current_batch = {};
};
}  // namespace lib::backend::render