#include <lib_gui/lib_gui.hpp>

#include <cassert>

#include <lib_gui/resources/fonts/tahoma.hpp>
#include <lib_gui/resources/fonts/tahomabd.hpp>
#include <ranges>

using namespace lib::gui;

gui::gui(std::weak_ptr<lib::input::input_handler>&& input_handler, std::weak_ptr<lib::rendering::renderer>&& renderer)
{
	_input_handler = std::move(input_handler);
	_renderer = std::move(renderer);

	// register input callbacks
	const auto& input_handler_ptr = _input_handler.lock();
	assert(input_handler_ptr != nullptr);

	input_handler_ptr->register_callback(
		lib::input::mouse | lib::input::keyboard,
		[&](const lib::input::input_callback_handler& processor)
	{
		if (_windows.empty()) [[unlikely]]
		{
			return;
		}

		// toggle visiblity flag for all windows
		if (processor.get_key_state(lib::gui::key::toggle_visibility).has(input::pressed))
		{
			toggle_visibility();
		}

		if (_show_windows &&
			processor.get_key_state(input::key_button::mouseleft).has(input::pressed) &&
			!is_in_rect(processor.get_cursor_position(), _active_window->get_position(), _active_window->get_size()))
		{
			// try update the active window
			for (auto it = _windows.begin() + 1; it != _windows.end(); ++it)
			{
				if (const auto& window = *it;
					is_in_rect(processor.get_cursor_position(), window->get_position(), window->get_size()))
				{
					set_active(window);
					std::rotate(_windows.begin(), it, it + 1);

					// pass input to the new window
					_active_window->input(processor);
					break;
				}
			}

			return;
		}

		// get windows in reverse to get active window first
		for (const auto& window : _windows)
		{
			window->input(processor);
		}
	});

	// register fonts and draw callbacks
	const auto& renderer_ptr = _renderer.lock();
	assert(renderer_ptr != nullptr);

	lib::gui::context::screen_size = renderer_ptr->get_window_size();

	lib::gui::font::title = renderer_ptr->add_font(tahoma_bold_font.data(), 16);
	lib::gui::font::heading = renderer_ptr->add_font(tahoma_bold_font.data(), 14);
	lib::gui::font::text = renderer_ptr->add_font(tahoma_font.data(), 14);

	renderer_ptr->register_callback([&](lib::rendering::render_callback_handler& render)
	{
		if (_windows.empty()) [[unlikely]]
		{
			return;
		}

		// get windows in reverse to get active window first
		for (const auto& window : std::ranges::reverse_view(_windows))
		{
			window->draw(render);
		}
	});
}

void gui::toggle_visibility()
{
	_show_windows = !_show_windows;

	for (const auto& window: _windows)
	{
		window->refresh(ui_update::update_visibility, &_show_windows);
	}
}

void gui::show_windows()
{
	if (_show_windows)
	{
		return;
	}

	toggle_visibility();
}

void gui::hide_windows()
{
	if (!_show_windows)
	{
		return;
	}

	toggle_visibility();
}

bool gui::is_visible() const
{
	return _show_windows;
}

void gui::set_active(const std::shared_ptr<ui_base_window>& active_window)
{
	_active_window = active_window;

	for (const auto& window: _windows)
	{
		const auto is_active = window == _active_window;
		window->refresh(lib::gui::update_active, &is_active);
	}
}