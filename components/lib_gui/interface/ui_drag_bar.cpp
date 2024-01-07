#include <lib_gui/interface/ui_drag_bar.hpp>

using namespace lib::gui;

ui_drag_bar::ui_drag_bar(const std::string& name)
	: ::ui_base_window(name, {100, 100}, drag_bar::size)
{

}

ui_drag_bar::~ui_drag_bar()
{

}

void ui_drag_bar::draw(lib::rendering::renderer& render)
{
	if (!flags().has(ui_flag::flag_visible))
	{
		return;
	}

	const auto& pos = get_position();
	const auto& size = drag_bar::size;

	// background
	render.draw_rect_gradient_v_filled(pos, size, drag_bar::color_fill_gradient, drag_bar::color_fill);

	// draw pin
	render.draw_rect_filled(
		pos + drag_bar::pin_offset - drag_bar::pin_size / 2,
		drag_bar::pin_size,
		window_flags().has(window::ui_window_flags::window_pinned) ? color::green : color::red);

	// form name pinned box
	render.draw_font(
		pos + lib::point2Di(drag_bar::pin_offset.x + drag_bar::pin_size.x + 5, size.y / 2 - 1),
		color::white, font::title, get_name(), rendering::centered_y);
}

void ui_drag_bar::input(const lib::input::input_handler& input)
{
	if (!flags().has(ui_flag::flag_visible) || !flags().has(ui_flag::flag_active))
	{
		return;
	}

	const auto& pos = get_position();
	const auto& size = drag_bar::size;

	const auto& left_mouse = input.get_key_state(input::key_button::mouseleft);

	if (left_mouse.has(input::pressed))
	{
		if (is_in_rect(
			input.get_cursor_position(),
			pos + drag_bar::pin_offset - drag_bar::pin_size / 2,
			drag_bar::pin_size))
		{
			window_flags().toggle(window::ui_window_flags::window_pinned);
		}
		else if (is_in_rect(input.get_cursor_position(), pos, size))
		{
			window_flags().add(window::ui_window_flags::window_dragging);
		}
	}
	else if (window_flags().has(window::window_dragging))
	{
		if (left_mouse.has(input::down))
		{
			// move current form and children
			const auto& mouseDelta = input.get_cursor_delta();
			const lib::point2Di bound_pos =
			{
				std::clamp<int>(pos.x + mouseDelta.x, 0, context::screen_size.x - size.x),
				std::clamp<int>(pos.y + mouseDelta.y, 0, context::screen_size.y - size.y)
			};

			set_position(bound_pos);
			refresh(update_position, &bound_pos);
		}
		else
		{
			window_flags().remove(window::ui_window_flags::window_dragging);
		}
	}
}