#pragma once

#include <string>

#include <module_core/type/bitflag.hpp>
#include <module_core/type/color.hpp>
#include <module_core/type/point/point2D.hpp>

namespace lib::render
{
enum font_flag: lib::bitflag_t
{
    // Bit flag
    FONT_FLAG_NONE          = 0,
    FONT_FLAG_ALIGN_LEFT    = 0,
    FONT_FLAG_ALIGN_RIGHT   = 1 << 0,
    FONT_FLAG_CENTER_X      = 1 << 1,
    FONT_FLAG_CENTER_Y      = 1 << 2,
    FONT_FLAG_OUTLINE       = 1 << 3,

    // Combination flags
    FONT_FLAG_CENTERED      = FONT_FLAG_CENTER_X | FONT_FLAG_CENTER_Y,
};

//! Interface passed to all render callbacks.
template<typename Base>
class RenderConsumer_Base {
public:
    void some_func() {
        return static_cast<Base*>(this)->internal_some_func();
    }
};
}