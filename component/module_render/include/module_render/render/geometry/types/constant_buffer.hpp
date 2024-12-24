#pragma once

#include <dep_nvrhi/nvrhi.hpp>

#include <module_core/type/matrix/matrix4x4.hpp>

namespace lib::render::detail {

// Constant buffer used to render geometry
// Note: DON'T FUCK WITH THE ORDERING
struct constant_buffer_t {
    matrix4x4f mvp_matrix           = {};
    matrix4x4f model_matrix         = {};
    matrix4x4f view_matrix          = {};
    matrix4x4f projection_matrix    = {};
};

static_assert(
    sizeof(constant_buffer_t) % nvrhi::c_ConstantBufferOffsetSizeAlignment == 0,
    "sizeof(ConstantBufferEntry) must be 256 bytes"
);

}