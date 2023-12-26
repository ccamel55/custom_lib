#pragma once

#include <lib_gui/interface/ui_base_element.hpp>

namespace lib::gui
{
namespace window
{
	enum ui_window_flags: bitflag_t
	{
		window_none = 0 << 0,
		window_pinned = 1 << 0,
		window_blocked = 1 << 1,
		window_dragging = 1 << 2,
	};
}

//! used to restrict some functions to only accept windows (aka form or drag bar)
class ui_base_window: public ui_base_element
{
public:
	explicit ui_base_window(const std::string& name, const lib::point2Di& pos, const lib::point2Di& size);

	void refresh(bitflag update, const void* data) override;

public:
	[[nodiscard]] const std::string& get_name() const;
	void set_name(const std::string& name);

	bitflag& window_flags();

protected:
	std::string _name;
	bitflag _window_flags;

};
}