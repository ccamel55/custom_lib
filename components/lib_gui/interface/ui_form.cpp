#include <lib_gui/interface/ui_form.hpp>

using namespace lib::gui;

ui_form::ui_form(const std::string& name, const lib::point2Di& size)
	: ::ui_base_window(name, {100, 100}, size)
{

}

ui_form::~ui_form()
{

}

void ui_form::draw(lib::rendering::renderer& render)
{
	if (!flags().has(ui_flag::flag_visible))
	{
		return;
	}

	const auto& pos = get_position();
	const auto& size = get_size();

	// draw main form
	render.draw_rect_filled(pos, size, form::color_fill);
	render.draw_rect_gradient_v_filled(
		pos,
		{size.x, form::title_size_y},
		form::color_fill_gradient,
		form::color_fill);

	// draw control render regin
	render.draw_rect_filled(
		pos + form::contents_start_offset,
		size - form::contents_end_offset,
		form::color_contents);

	// draw pin
	render.draw_rect_filled(
		pos + form::pin_offset - form::pin_size / 2,
		form::pin_size,
		window_flags().has(window::ui_window_flags::window_pinned) ? color::green : color::red);

	// form name pinned box
	render.draw_font(
		pos + lib::point2Di(form::pin_offset.x + form::pin_size.x + 5, form::title_size_y / 2 - 1),
		color::white, font::title, get_name(), rendering::centered_y);
}

void ui_form::input(const lib::input::input_handler& input)
{
	if (!flags().has(ui_flag::flag_visible) || !flags().has(ui_flag::flag_active))
	{
		return;
	}

	const auto& pos = get_position();
	const auto& size = lib::point2Di(get_size().x, form::title_size_y);

	const auto& left_mouse = input.get_key_state(input::key_button::mouseleft);

	if (left_mouse.has(input::pressed))
	{
		if (is_in_rect(
			input.get_cursor_position(),
			pos + form::pin_offset - form::pin_size / 2,
			form::pin_size))
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
				std::clamp<int>(pos.x + mouseDelta.x, 0, context::screen_size.x - get_size().x),
				std::clamp<int>(pos.y + mouseDelta.y, 0, context::screen_size.y - get_size().y)
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