#pragma once

#include <module_core/type/color.hpp>

#include <module_core/type/point/point2D.hpp>
#include <module_core/type/point/point4D.hpp>

namespace lib::gui {

namespace detail {

struct styler_colors_t {
    lib::color backgroundPrimary;
    lib::color backgroundSecondary;

    lib::color borderPrimary;
    lib::color borderSecondary;

    lib::color accentPrimary;
    lib::color accentSecondary;
};

}

//! Gui style provider
class StyleProvider {
public:
    virtual ~StyleProvider() = default;



};

}