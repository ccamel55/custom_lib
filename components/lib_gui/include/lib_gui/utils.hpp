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
            (point._x >= position._x) &&
            (point._y >= position._y) &&
            (point._x <= position._x + size._x) &&
            (point._y <= position._y + size._y);
    }
}