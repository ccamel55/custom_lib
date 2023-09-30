#pragma once
#include <array>
#include <backend/render/opengl3/types/vertex.hpp>
#include <backend/render/opengl3/types/vertex_batch.hpp>

namespace lib::backend::opengl3
{
// todo: find optimal max vertices.
constexpr auto MAX_VERTICES = 1024;

struct batch_t
{
	// used to get index of next entry
	size_t vertex_count;
	size_t batch_count;

	// todo: figure out what is worst case batch count based on max vertices.
	std::array<vertex_t, MAX_VERTICES> vertices;
	std::array<vertex_batch_t, MAX_VERTICES / 3> vertex_batch;
};
}  // namespace lib::backend::opengl3
