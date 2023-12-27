#include <lib_gui/interface/ui_base_element.hpp>

using namespace lib::gui;

ui_base_element::ui_base_element(const lib::point2Di& pos, const lib::point2Di& size)
	: _flags(ui_flag::flag_none) , _size(size), _position(pos)
{

}

lib::bitflag& ui_base_element::flags()
{
	return _flags;
}

const lib::point2Di& ui_base_element::get_size() const
{
	return _size;
}

void ui_base_element::set_size(const lib::point2Di& size)
{
	_size = size;
}

const lib::point2Di& ui_base_element::get_position() const
{
	return _position;
}

void ui_base_element::set_position(const lib::point2Di& position)
{
	_position = position;
}
