#pragma once

#include <array>

#include <module_core/type/color.hpp>
#include <module_render/geometry/Geometry_2D.hpp>

namespace lib::gui {

enum Style_Color {
    Gui_Col_Background_Primary,
    Gui_Col_Background_Secondary,

    Gui_Col_Border_Primary,
    Gui_Col_Border_Secondary,

    Gui_Col_Accent_Primary,
    Gui_Col_Accent_Secondary,

    Gui_Col_Text_Primary,

    Style_Color_Count // MUST BE LAST
};

enum Style_Font {
    Gui_Font_H1,
    Gui_Font_P,

    Style_Font_Count // MUST BE LAST
};

//! Default gui style provider
class StyleProvider {
public:
    explicit StyleProvider(render::Geometry_2D& geometry_2d);

public:
    std::array<color, Style_Color_Count> col;
    std::array<render::geometry::Font_Id, Style_Font_Count> font;

};

}