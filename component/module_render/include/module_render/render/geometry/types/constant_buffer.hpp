#pragma once

#include <dep_glm/glm.hpp>
#include <dep_nvrhi/nvrhi.hpp>

namespace lib::render::detail {

// Constant buffer used to render geometry
struct constant_buffer_t {
    glm::f32mat4x4 view_matrix;
    float _padding[4 * 4 * 3] = {};
};

static_assert(
    sizeof(constant_buffer_t) == nvrhi::c_ConstantBufferOffsetSizeAlignment,
    "sizeof(ConstantBufferEntry) must be 256 bytes"
);

}