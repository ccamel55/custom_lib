#include <lib_gui/interface/ui_base_window.hpp>

using namespace lib::gui;

ui_base_window::ui_base_window(const std::string& name, const lib::point2Di& pos, const lib::point2Di& size)
	: ::ui_base_element(pos, size), _name(name), _window_flags(window::window_none)
{

}

void ui_base_window::refresh(bitflag update, const void* data)
{
	if (update.has(update_visibility))
	{
		if (*static_cast<const bool*>(data))
		{
			flags().add(ui_flag::flag_visible);
		}
		else
		{
			flags().remove(ui_flag::flag_visible);
		}
	}

	if (update.has(update_active))
	{
		if (*static_cast<const bool*>(data))
		{
			flags().add(ui_flag::flag_active);
		}
		else
		{
			flags().remove(ui_flag::flag_active);
		}
	}
}

[[nodiscard]] const std::string& ui_base_window::get_name() const
{
	return _name;
}

void ui_base_window::set_name(const std::string& name)
{
	_name = name;
}

lib::bitflag& ui_base_window::window_flags()
{
	return _window_flags;
}
