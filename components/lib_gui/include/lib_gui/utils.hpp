#pragma once

#include <core_sdk/types/point/point2D.hpp>

namespace lib::gui
{
    [[nodiscard]] inline bool is_in_rect(
        const lib::point2Di& point,
        const lib::point2Di& position,
        const lib::point2Di& size)
    {
        return
            (point.x >= position.x) &&
            (point.y >= position.y) &&
            (point.x <= position.x + size.x) &&
            (point.y <= position.y + size.y);
    }
}