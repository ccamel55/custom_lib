#include <core_sdk/math/intersection.hpp>

using namespace lib::math;

bool lib::math::does_intersect_plane(
    const ray& ray,
    const point3Df& plane_origin,
    const vector3D& plane_normal)
{
    // check the diretion of the
    const auto ray_normal_dot = ray.direction.dot(plane_normal);

    if (ray_normal_dot <= std::numeric_limits<float>::epsilon())
    {
        return false;
    }

    // distance the ray must travel to intersect the plane
    const auto t = ((plane_origin - ray.origin).dot(plane_normal)) / ray_normal_dot;
    return t <= ray.scalar;
}

bool lib::math::does_intersect_aabb(
    const ray& ray,
    const point3Df& min,
    const point3Df& max)
{
    // shout out https://gamedev.stackexchange.com/a/18459 for the function :)
    const auto ray_unit_direction = vector3D(1.f, 1.f, 1.f) / ray.direction;

    const auto t1 = (min - ray.origin) * ray_unit_direction;
    const auto t2 = (max - ray.origin) * ray_unit_direction;

    const auto t_min = std::max(
        std::max(std::min(t1.x, t2.x),std::min(t1.y, t2.y)),
        std::min(t1.z, t2.z));

    const auto t_max = std::min(
        std::min(std::max(t1.x, t2.x),std::max(t1.y, t2.y)),
        std::max(t1.z, t2.z));

    // under 0 means we interect with aabb but it's behind us
    if (t_max < std::numeric_limits<float>::epsilon())
    {
        return false;
    }

    // if min > max then we don't intersect aabb
    if (t_min > t_max)
    {
        return false;
    }

    // make sure our ray is long enough to hit the aabb
    return t_min <= ray.scalar;
}

bool lib::math::does_interset_traingle(
    const ray& ray,
    const point3Df& a,
    const point3Df& b,
    const point3Df& c)
{
    // moller trumborn ray triangle intersection.
    // first we pick two corners and get the angle to the third corner
    const auto v0 = vector3D(b - a);
    const auto v1 = vector3D(c - a);

    // then find the perpindicular angle between our ray and one of these angles and find the distance from to the
    // last corner
    const auto p_vec = ray.direction.cross(v1);
    const auto determinant = v0.dot(p_vec);

    if (determinant <= std::numeric_limits<float>::epsilon())
    {
        return false;
    }

    const auto determinant_inverse = 1.f / determinant;

    const auto t_vec = vector3D(ray.origin - a);
    const auto u = t_vec.dot(p_vec) * determinant_inverse;

    if (u < 0.f || u > 1.f)
    {
        return false;
    }

    const auto q_vec  = t_vec.cross(v0);
    const auto v = ray.direction.dot(q_vec) * determinant_inverse;

    if (v < 0.f || u + v > 1.f)
    {
        return false;
    }

    const auto t = v1.dot(q_vec) * determinant_inverse;
    return t <= ray.scalar;
}

bool lib::math::does_intersect_disk(
    const ray& ray,
    const point3Df& plane_origin,
    const vector3D& plane_normal,
    float radius)
{
    // perform a ray plane intersection test, then derive if a ray lives inside a circle by solving for the radius
    // of the intersection point relative to the plane's origin
    const auto ray_normal_dot = ray.direction.dot(plane_normal);

    if (ray_normal_dot <= std::numeric_limits<float>::epsilon())
    {
        return false;
    }

    // distance the ray must travel to intersect the plane
    const auto t = ((plane_origin - ray.origin).dot(plane_normal)) / ray_normal_dot;
    const auto p = ray.origin + (ray.direction * t);

    return vector3D(p - plane_origin).length_sqr() <= radius * radius;
}

bool lib::math::does_intersect_sphere(
    const ray& ray,
    const point3Df& sphere_origin,
    float radius)
{
    // t is the distance between the sphere origin and the ray.
    const auto t = (sphere_origin - ray.origin).dot(ray.direction);
    const auto p = ray.origin + (ray.direction * t);

    // to find t1 and t2 (first and second intersection points) we need to find x and y in x^2 + y^2 = r^2.
    // x = +- sqrt(r^2  - y^2), y = (s - ray.point).length
    const auto y = vector3D(sphere_origin - p).length_sqr();
    const auto r2 = radius * radius;

    // if we are to far from the radius then BYE BYE
    if (y >= r2)
    {
        return false;
    }

    const auto x = std::sqrt(r2 - y);
    const auto t1 = t - x;

    // make sure our intersection point is closer than our ray length
    return t1 <= ray.scalar;
}

bool lib::math::does_intersect_capsule(
    const ray& ray,
    const point3Df& a,
    const point3Df& b,
    float radius)
{
    // I have no idea wtf this does coz the ill fucker who made this didn't label or comment shit.
    const auto distance_segment_to_segment = [](
        const point3Df& s1,
        const point3Df& s2,
        const point3Df& k1,
        const point3Df& k2) -> float
    {
        const auto u = s2 - s1;
        const auto v = k2 - k1;
        const auto w = s1 - k1;

        const float a = u.dot(u);
        const float b = u.dot(v);
        const float c = v.dot(v);
        const float d = u.dot(w);
        const float e = v.dot(w);

        const float D = a * c - b * b;
        float sN, sD = D;
        float tN, tD = D;

        if (D < std::numeric_limits<float>::epsilon())
        {
            sN = 0.f;
            sD = 1.f;
            tN = e;
            tD = c;
        }
        else
        {
            sN = (b*e - c*d);
            tN = (a*e - b*d);

            if (sN < 0.f)
            {
                sN = 0.f;
                tN = e;
                tD = c;
            }
            else if (sN > sD)
            {
                sN = sD;
                tN = e + b;
                tD = c;
            }
        }

        if (tN < 0.f)
        {
            if (-d < 0.f)
            {
                sN = 0.f;
            }
            else if (-d > a)
            {
                sN = sD;
            }
            else
            {
                sN = -d;
                sD = a;
            }

            tN = 0.f;
        }
        else if (tN > tD)
        {
            if ((-d + b) < 0.f)
            {
                sN = 0.f;
            }
            else if ((-d + b) > a)
            {
                sN = sD;
            }
            else
            {
                sN = (-d + b);
                sD = a;
            }

            tN = tD;
        }

        const auto sc = (abs(sN) < std::numeric_limits<float>::epsilon() ? 0.f : sN / sD);
        const auto tc = (abs(tN) < std::numeric_limits<float>::epsilon() ? 0.f : tN / tD);

        return vector3D(w + (u * sc) - (v * tc)).length();
    };

    return distance_segment_to_segment(ray.origin, ray.get_point(), a, b) < radius;
}






