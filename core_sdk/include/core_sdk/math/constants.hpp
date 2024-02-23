#pragma once

namespace lib::math
{
// IEEE-754 specifies that floats have 21 significant bits (assuming that a float is 32 bits), this can give us
// aproximately (just under) 7 decimal digits.
constexpr float pi = 3.1415926535897932;
constexpr float pi_2 = pi * 2.0;
constexpr float pi_half = pi / 2.0;
}