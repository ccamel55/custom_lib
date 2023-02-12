#include <common/math/math_lib.hpp>

using namespace lib::common;

float math_lib::random_float(float min, float max) {
    const auto r = (float)rand() / (float)RAND_MAX;
    return min + r * (max - min);
}

int math_lib::random_int(int min, int max) {
    return min + rand() % (( max + 1 ) - min);
}