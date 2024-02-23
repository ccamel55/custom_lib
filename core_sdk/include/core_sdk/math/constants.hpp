#pragma once

namespace lib::math
{
// Change precision depending on what we set, by default we just use floats since on x86 the compiler will swap to
// doubles when it thinks it will be faster. Some other architectures might prefer floats by default.
#ifdef DEF_LIB_MATH_USE_DOUBLE_PRECISION_on
    using floating_point = double;
#else
    using floating_point = float;
#endif

// IEEE-754 specifies that floats have 21 significant bits (assuming that a float is 32 bits), this can give us
// aproximately (just under) 7 decimal digits. Doubles have 53 significant bits (assuming that a double is 64 bits),
// this can give us aproximately (just under) 16 decimal digits.
constexpr floating_point pi = 3.1415926535897932;
constexpr floating_point pi_2 = pi * 2.0;
constexpr floating_point pi_half = pi / 2.0;
}