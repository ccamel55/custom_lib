#pragma once

#include <module_core/type/matrix/matrix4x4.hpp>
#include <module_core/type/point/point3D.hpp>
#include <module_core/type/vector/vector3D.hpp>

namespace lib::math {

//! Create a view matrix that looks at another point
//! \param eye eye origin
//! \param target target origin
//! \param up axis which represent up, defaults to { 0, 0, 0 }
//! \returns affine view matrix that looks at `target`
[[nodiscard]] matrix4x4f view_look_at(
    const point3Df& eye,
    const point3Df& target,
    const point3Df& up = { 0, 0, 1 }
);

//! Create a view matrix from an eye position and view direction
//! \param eye eye origin
//! \param rotation euler angle for camera's view. These must be clamped (+/- 90, +/- 180, +/- 45)
//! \returns affine view matrix that looks with `rotation`
[[nodiscard]] matrix4x4f view_first_person(
    const point3Df& eye,
    const vector3D& rotation
);

//! Create a view matrix that looks around an object
//! \param target target origin
//! \param rotation euler angle for rotation around `target`
//! \param offset distance away from the `target`
//! \returns affine view matrix that rotates around the `target`
[[nodiscard]] matrix4x4f view_arc_ball(
    const point3Df& target,
    const vector3D& rotation,
    const point3Df& offset
);

}