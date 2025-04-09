#include <module_gui/StyleProvider.hpp>

using namespace lib::gui;

StyleProvider::StyleProvider(render::Geometry_2D& geometry_2d) {
    //
    // DEFAULT COLORS
    //

    col[Gui_Col_Background_Primary]     = color(50, 50, 50, 220);
    col[Gui_Col_Background_Secondary]   = color(40, 40, 40, 220);

    col[Gui_Col_Border_Primary]         = color(150, 150, 150);
    col[Gui_Col_Border_Secondary]       = color(30, 30, 30);

    col[Gui_Col_Accent_Primary]         = color(255, 145, 25);
    col[Gui_Col_Accent_Secondary]       = color(200, 165, 105);

    col[Gui_Col_Text_Primary]           = color(255, 255, 255);

    //
    // DEFAULT FONTS
    //

    font[Gui_Font_H1]   = geometry_2d.add_font("arial.ttf", 16).value();
    font[Gui_Font_P]    = font[Gui_Font_H1];
}
