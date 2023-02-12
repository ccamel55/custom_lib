#pragma once

#include <random>

namespace lib::common {

    class math_lib {
    public:
        static float random_float(float min, float max);
        static int random_int(int min, int max);
    };
}