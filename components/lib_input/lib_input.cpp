#include <lib_input/lib_input.hpp>

#include <cassert>

using namespace lib::input;

input_handler::~input_handler()
{
	// unregister this handler from all receivers
	for (const auto& receiver : _input_receivers)
	{
		if (const auto receiver_ptr = receiver.lock(); receiver_ptr)
		{
			receiver_ptr->register_handler(nullptr);
		}
	}
}

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

		_cursor_delta = cursor_position - _cursor_position;
		_cursor_position = cursor_position;
		break;
	}
	case key_button::mouse_scroll: {
		_scroll_delta = std::get<lib::point2Df>(input.state);
		break;
	}
	default: {
		_last_key = input.key;

		if ( std::get<bool>(input.state))
		{
			_key_state.at(key) = (down | pressed);
		}
		else
		{
			_key_state.at(key) = (up | released);
		}
		break;
	}
	}

	for (const auto& receiver : _input_receivers)
	{
		if (const auto receiver_ptr = receiver.lock(); receiver_ptr)
		{
			receiver_ptr->emit_update(input.type);
		}
	}

	// reset some states
	_scroll_delta = {};
	_cursor_delta = {};

	// pressed and released should only ever be set once
	_key_state.at(key) &= ~(pressed | released);
}

void input_handler::register_receiver(std::weak_ptr<input_receiver>&& receiver)
{
	// move weak ptr into our vector
	const auto receiver_ptr = receiver.lock();
	assert(receiver_ptr != nullptr);

	// register this class as handler then add to vector
	receiver_ptr->register_handler(this);
	_input_receivers.emplace_back(std::move(receiver));
}

[[nodiscard]] key_button input_handler::get_last_key() const
{
	return _last_key;
}

input_state input_handler::get_key_state(key_button key) const
{
	return static_cast<input_state>(_key_state.at(static_cast<uint8_t>(key)));
}

const lib::point2Df& input_handler::get_cursor_position() const
{
	return _cursor_position;
}

const lib::point2Df& input_handler::get_cursor_delta() const
{
	return _cursor_delta;
}

const lib::point2Df& input_handler::get_scroll_delta() const
{
	return _scroll_delta;
}

