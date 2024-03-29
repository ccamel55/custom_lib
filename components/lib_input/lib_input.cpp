#include <lib_input/lib_input.hpp>

using namespace lib::input;

void input_handler::add_input(const input_t& input)
{
	if (input.key == key_button::none)
	{
		return;
	}

	const auto key = static_cast<uint8_t>(input.key);

	switch (input.key)
	{
	case key_button::mouse_move: {
		const auto& cursor_position = std::get<lib::point2Di>(input.state);

		_cursor_delta = cursor_position - _cursor_position;
		_cursor_position = cursor_position;
		break;
	}
	case key_button::mouse_scroll: {
		_scroll_delta = std::get<lib::point2Di>(input.state);
		break;
	}
	default: {
		_last_key = input.key;

		if ( std::get<bool>(input.state))
		{
			_key_state.at(key).add(down | pressed);
			_key_state.at(key).remove(up);
		}
		else
		{
			_key_state.at(key).add(up | released);
			_key_state.at(key).remove(down);
		}
		break;
	}
	}

	for (const auto& receiver : _input_callbacks)
	{
		const auto& [type, callback] = receiver;

		if (input.type.has(type))
		{
			callback(*this);
		}
	}

	// reset some states
	_scroll_delta = {};
	_cursor_delta = {};

	// pressed and released should only ever be set once
	_key_state.at(key).remove(pressed | released);
}

void input_handler::register_callback(uint32_t type, std::function<void(const input_callback_handler&)>&& callback)
{
	_input_callbacks.emplace_back(input_callback_t{ static_cast<input_type>(type), std::move(callback) });
}

key_button input_handler::get_last_key() const
{
	return _last_key;
}

const lib::bitflag& input_handler::get_key_state(key_button key) const
{
	return _key_state.at(static_cast<uint8_t>(key));
}

const lib::point2Di& input_handler::get_cursor_position() const
{
	return _cursor_position;
}

const lib::point2Di& input_handler::get_cursor_delta() const
{
	return _cursor_delta;
}

const lib::point2Di& input_handler::get_scroll_delta() const
{
	return _scroll_delta;
}
