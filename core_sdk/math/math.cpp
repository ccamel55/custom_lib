#include <core_sdk/math/math.hpp>

using namespace lib::math;

floating_point lib::math::radian_to_degree(floating_point radian)
{
    return (radian * 180.0) / pi;
}

floating_point lib::math::degree_to_radian(floating_point degree)
{
    return (degree * pi) / 180.0;
}

