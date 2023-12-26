#pragma once

#include <lib_gui/defines/global_defines.hpp>

namespace lib::gui
{
//! Base class for all UI elements
class ui_base_element
{
protected:
    ui_flag _flags = flag_none;
    ui_type _type = type_none;

    lib::point2Di _size = {};
    lib::point2Di _position = {};
};
}