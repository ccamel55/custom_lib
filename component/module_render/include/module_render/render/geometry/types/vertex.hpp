#pragma once

#include <dep_nvrhi/nvrhi.hpp>

#include <module_core/type/point/point2D.hpp>
#include <module_core/type/point/point3D.hpp>

#include <module_core/type/color.hpp>

#include <vector>

namespace lib::render::detail {

constexpr size_t MAX_VERTICES   = 1 << 16;              // 64k
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
        position.x = x;
        position.y = y;
        position.z = z;

        uv.s = u;
        uv.t = v;

        col.r = r;
        col.g = g;
        col.b = b;
        col.a = a;
    }

    [[nodiscard]] constexpr static std::array<nvrhi::VertexAttributeDesc, 3> attributes() {
        return {
            nvrhi::VertexAttributeDesc()
                .setName("POSITION")
                .setFormat(nvrhi::Format::RGB32_FLOAT)
                .setArraySize(1)
                .setBufferIndex(0)
                .setOffset(offsetof(vertex_t, position))
                .setElementStride(sizeof(vertex_t)),

            nvrhi::VertexAttributeDesc()
                .setName("UV")
                .setFormat(nvrhi::Format::RG32_FLOAT)
                .setArraySize(1)
                .setBufferIndex(0)
                .setOffset(offsetof(vertex_t, uv))
                .setElementStride(sizeof(vertex_t)),

            nvrhi::VertexAttributeDesc()
                .setName("COLOR")
                .setFormat(nvrhi::Format::RGBA8_UNORM)
                .setArraySize(1)
                .setBufferIndex(0)
                .setOffset(offsetof(vertex_t, col))
                .setElementStride(sizeof(vertex_t))
        };
    }

    point3Df position       = {};
    point2Df uv             = {};
    color col               = {};
};

}