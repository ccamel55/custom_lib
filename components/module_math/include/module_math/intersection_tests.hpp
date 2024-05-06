#pragma once

#include <module_core/types/Ray.hpp>
#include <module_core/types/vector/vector3D.hpp>
#include <module_core/types/point/point3D.hpp>

namespace lib
{
//! Simple ray plane intersection test.
//! \param Ray is the ray we want to cast
//! \param plane_origin is the origin of the plane
//! \param plane_normal is the normal of the plane
bool does_intersect_plane(
        const Ray& ray,
        const point3Df& plane_origin,
        const vector3D& plane_normal
);

//! Performs intersection test for an axis alined bounding box.
//! \param Ray is the ray we want to cast
//! \param min is the minimum co-ordinates of the bounding box
//! \param max is the maximum co-ordinates of the boudning box
bool does_intersect_aabb(
        const Ray& ray,
        const point3Df& min,
        const point3Df& max
);

//! Performs intersection test for a 3d traingle defined by a, b and c.
//! \param Ray is the ray we want to cast
//! \param a point a
//! \param b point b
//! \param c point c
bool does_intersect_triangle(
        const Ray& ray,
        const point3Df& a,
        const point3Df& b,
        const point3Df& c
);

//! Performs intersection test a disk.
//! \param Ray is the ray we want to cast
//! \param plane_origin is the origin of the disk
//! \param plane_normal is the normal of the disk
//! \param radius is the radius of the disk
bool does_intersect_disk(
        const Ray& ray,
        const point3Df& plane_origin,
        const vector3D& plane_normal,
        float radius
);

//! Performs intersection test on a sphere.
//! \param Ray is the ray we want to cast
//! \param sphere_origin is the origin of the sphere
//! \param radius is the radius of the sphere
bool does_intersect_sphere(
        const Ray& ray,
        const point3Df& sphere_origin,
        float radius
);

//! Performs intersection test for a capsule.
//! \param Ray is the ray we want to cast
//! \param a point a
//! \param b point b
//! \param radius is the radius of the capsule
bool does_intersect_capsule(
        const Ray& ray,
        const point3Df& a,
        const point3Df& b,
        float radius
);
}