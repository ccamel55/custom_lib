#pragma once
#include <array>
#include <common/signal_handler.hpp>
#include <common/types/point/point2D.hpp>

namespace lib::backend
{
constexpr auto ASCII_KEY_CODE_COUNT = 256;

enum class input_state : uint8_t
{
	change,	 // used for scroll and mouse move events, when id is scroll, x and y is left and right scroll respectively
	down,
	up,
	pressed,
	released,
};

//! base class for input handling
class input_handler_base
{
public:
	virtual ~input_handler_base() = default;
	virtual void update_state(void* data) = 0;

public:
	//! passes the parameters \a identifier, \a mouse_position and \a input_state
	common::signal_handler<int, common::point2Di, input_state> on_input_change = {};

private:
	std::array<bool, ASCII_KEY_CODE_COUNT> _current_key_state = {};
	std::array<bool, ASCII_KEY_CODE_COUNT> _previous_key_state = {};

	common::point2Di _mouse_position = {};
	int _scroll_position = 0;
};
}  // namespace lib::backend