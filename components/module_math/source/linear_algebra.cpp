#include <module_math/linear_algebra.hpp>
#include <numbers>

using namespace lib::math;

constexpr float lib::math::radian_to_degree(float radian) {
    return static_cast<float>((radian * 180.f) / std::numbers::pi);
}

constexpr float lib::math::degree_to_radian(float degree) {
    return static_cast<float>((degree * std::numbers::pi) / 180.f);
}

