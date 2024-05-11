#include <module_math/linear_algebra.hpp>
#include <module_math/constants.hpp>

using namespace lib::math;

constexpr float lib::math::radian_to_degree(float radian) {
    return (radian * 180.f) / PI;
}

constexpr float lib::math::degree_to_radian(float degree) {
    return (degree * PI) / 180.f;
}

