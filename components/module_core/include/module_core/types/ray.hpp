#pragma once
#include <module_core/types/vector/vector3D.hpp>

namespace lib {
class ray {
public:
    constexpr ray() = default;

    //! Construct a ray.
    //! \param origin Origin or the ray.
    //! \param normalized_direction direction of the ray.
    //! \param scalar the unit length of the ray.
    constexpr ray(const point3Df& origin, const vector3D& normalized_direction, float scalar)
        : origin(origin)
        , direction(normalized_direction)
        , scalar(scalar) {
    }

    //! Returns the point in 3d space that the ray represents
    [[nodiscard]] point3Df get_point() const {
        return origin + (direction * scalar);
    }

public:
    point3Df origin    = { };
    vector3D direction = { };
    float scalar       = { };

};
}
