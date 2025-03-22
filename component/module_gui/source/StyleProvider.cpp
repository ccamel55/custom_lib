#include <module_gui/StyleProvider.hpp>

using namespace lib::gui;

StyleProvider::StyleProvider() {
    col[Gui_Col_Background_Primary]     = color(50, 50, 50, 220);
    col[Gui_Col_Background_Secondary]   = color(40, 40, 40, 220);

    col[Gui_Col_Border_Primary]         = color(150, 150, 150);
    col[Gui_Col_Border_Secondary]       = color(30, 30, 30);

    col[Gui_Col_Accent_Primary]         = color(255, 180, 65);
    col[Gui_Col_Accent_Secondary]       = color(200, 165, 105);
}
