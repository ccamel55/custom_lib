#pragma once

#include <dep_glm/glm.hpp>

#include <array>

namespace lib::render::detail {

constexpr size_t MAX_VERTICES   = 1 << 16;                // 64k
constexpr size_t MAX_INDICES    = MAX_VERTICES * 2;     // 64k * 2

// Index buffer type
using index_t = uint32_t;

// Vertex type for 2d and 3d geometry
struct vertex_t {
    glm::float32_t position[3];
    glm::float32_t uv[2];
    glm::float32_t color[4];
};

using vertex_array_t    = std::array<vertex_t, MAX_VERTICES>;
using index_array_t     = std::array<index_t, MAX_INDICES>;

}