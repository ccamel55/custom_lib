#include <module_math/matrix/view.hpp>

using namespace lib::math;

lib::matrix4x4f lib::math::view_look_at(
    const point3Df& eye,
    const point3Df& target,
    const point3Df& up
) {
    // GLM has this done for us! thanks GLM
    return lookAt(eye, target, up);
}

lib::matrix4x4f lib::math::view_first_person(
    const point3Df& eye,
    const vector3D& rotation
) {
    matrix4x4f result;
    {
        // Rotate by camera angle
        result = toMat4(glm::quat(rotation));

        // Translate by eye origin
        result = translate(result, eye);

        // Inverse to get view matrix
        result = inverse(result);
    }
    return result;
}

lib::matrix4x4f lib::math::view_arc_ball(
    const point3Df& target,
    const vector3D& rotation,
    const point3Df& offset
) {
    matrix4x4f result;
    {
        // Move the target back to the origin of the co-ordinate system
        result = translate(matrix4x4f(1), -target);

        // Rotate around camera
        result = toMat4(inverse(glm::quat(rotation))) * result;

        // Move offset away
        result = translate(result, -offset);
    }
    return result;
}
