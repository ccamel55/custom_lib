#include <lib_input/lib_input.hpp>

using namespace lib::input;

void input_handler::add_input(const input_t& input)
{
	if (input.key == key_button::none || input.type == input_type::none)
	{
		return;
	}

	const auto key = static_cast<uint8_t>(input.key);

	switch (input.key)
	{
	case key_button::mouse_move: {
		const auto& cursor_position = std::get<lib::point2Df>(input.state);

		_input_processor._cursor_delta = cursor_position - _input_processor._cursor_position;
		_input_processor._cursor_position = cursor_position;
		break;
	}
	case key_button::mouse_scroll: {
		_input_processor._scroll_delta = std::get<lib::point2Df>(input.state);
		break;
	}
	default: {
		_input_processor._last_key = input.key;

		if ( std::get<bool>(input.state))
		{
			_input_processor._key_state.at(key) = (down | pressed);
		}
		else
		{
			_input_processor._key_state.at(key) = (up | released);
		}
		break;
	}
	}

	for (const auto& receiver : _input_callbacks)
	{
		const auto& [type, callback] = receiver;

		if (input.type & type)
		{
			callback(_input_processor);
		}
	}

	// reset some states
	_input_processor._scroll_delta = {};
	_input_processor._cursor_delta = {};

	// pressed and released should only ever be set once
	_input_processor._key_state.at(key) &= ~(pressed | released);
}

void input_handler::register_callback(input_type type, std::function<void(const input_processor&)>&& callback)
{
	_input_callbacks.emplace_back(input_callback_t{ type, std::move(callback) });
}