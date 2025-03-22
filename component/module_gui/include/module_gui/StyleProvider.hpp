#pragma once

#include <array>

#include <module_core/type/color.hpp>

namespace lib::gui {

enum Style_Color {
    Gui_Col_Background_Primary,
    Gui_Col_Background_Secondary,

    Gui_Col_Border_Primary,
    Gui_Col_Border_Secondary,

    Gui_Col_Accent_Primary,
    Gui_Col_Accent_Secondary,

    Style_Color_Count // MUST BE LAST
};

//! Default gui style provider
class StyleProvider {
public:
    StyleProvider();

public:
    std::array<color, Style_Color_Count> col;

};

}