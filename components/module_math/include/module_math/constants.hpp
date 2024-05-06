#pragma once

namespace lib
{
// IEEE-754 specifies that floats have 21 significant bits (assuming that a float is 32 bits), this can give us
// approximately (just under) 7 decimal digits.
constexpr float PI = 3.1415926535897932;
constexpr float PI_2 = PI * 2.0;
constexpr float PI_HALF = PI / 2.0;
}