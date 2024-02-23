#pragma once

#include <core_sdk/types/vector/vector3D.hpp>

namespace lib
{
class ray
{
public:
    constexpr ray() = default;

    //! Construct a ray.
    //! \param origin Origin or the ray.
    //! \param direction direction of the ray.
    constexpr ray(const point3Df& origin, const vector3D& direction)
        : origin(origin), direction(direction)
    {
    }

    //! Scale the direction of a ray.
    //! \param scalar is the new unit length of the ray.
    void rescale(float scalar)
    {
        // first normalize back to 1, then scale by scalar
        direction = direction.normalised() * scalar;
    }

public:
    point3Df origin = {};
    vector3D direction = {};

};
}