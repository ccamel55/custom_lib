#pragma once

#include <vector>

namespace lib::render::detail {

constexpr size_t MAX_VERTICES   = 1 << 16;                // 64k
constexpr size_t MAX_INDICES    = MAX_VERTICES * 2;     // 64k * 2

// Index buffer type
using index_t = uint32_t;

// Vertex type for 2d and 3d geometry
struct vertex_t {

    constexpr vertex_t() = default;
    constexpr vertex_t(
        const glm::float32_t x,
        const glm::float32_t y,
        const glm::float32_t z,
        const glm::float32_t u,
        const glm::float32_t v,
        const glm::uint8_t r = 255,
        const glm::uint8_t g = 255,
        const glm::uint8_t b = 255,
        const glm::uint8_t a = 255
    ) {
        position[0] = x;
        position[1] = y;
        position[2] = z;

        uv[0] = u;
        uv[1] = v;

        color[0] = r;
        color[1] = g;
        color[2] = b;
        color[3] = a;
    }

    glm::float32_t position[3] = {};
    glm::float32_t uv[2] = {};
    glm::uint8_t color[4] = {};
};

using vertex_array_t    = std::vector<vertex_t>;
using index_array_t     = std::vector<index_t>;

}