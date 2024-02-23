#include <core_sdk/math/math.hpp>
#include <core_sdk/math/constants.hpp>

using namespace lib::math;

float lib::math::radian_to_degree(float radian)
{
    return (radian * 180.0) / pi;
}

float lib::math::degree_to_radian(float degree)
{
    return (degree * pi) / 180.0;
}

