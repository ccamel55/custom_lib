#include <lib_input/input_receiver.hpp>
#include <lib_input/lib_input.hpp>

#include <cassert>

using namespace lib::input;

input_receiver::input_receiver(input_type type)
	: _input_handler(nullptr), _type(type), _input_callback(nullptr)
{
}

void input_receiver::register_input_callback(std::function<void()> callback)
{
	_input_callback = std::move(callback);
}

key_button input_receiver::get_last_key() const
{
	return _input_handler->get_last_key();
}

input_state input_receiver::get_key_state(key_button key) const
{
	return _input_handler->get_key_state(key);
}

const lib::point2Df& input_receiver::get_cursor_position() const
{
	return _input_handler->get_cursor_position();
}

const lib::point2Df& input_receiver::get_cursor_delta() const
{
	return _input_handler->get_cursor_delta();
}

const lib::point2Df& input_receiver::get_scroll_delta() const
{
	return _input_handler->get_scroll_delta();
}

void input_receiver::emit_update(input_type type) const
{
	// if at least one of the types match, then call the callback
	if (_type & type)
	{
		if (_input_callback != nullptr)
		{
			_input_callback();
		}
	}
}

void input_receiver::register_handler(input_handler* handler)
{
	assert(_input_handler == nullptr);

	_input_handler = handler;
}