#include <lib_input/input_processor.hpp>

using namespace lib::input;

key_button input_processor::get_last_key() const
{
	return _last_key;
}

input_state input_processor::get_key_state(key_button key) const
{
	return static_cast<input_state>(_key_state.at(static_cast<uint8_t>(key)));
}

const lib::point2Df& input_processor::get_cursor_position() const
{
	return _cursor_position;
}

const lib::point2Df& input_processor::get_cursor_delta() const
{
	return _cursor_delta;
}

const lib::point2Df& input_processor::get_scroll_delta() const
{
	return _scroll_delta;
}

