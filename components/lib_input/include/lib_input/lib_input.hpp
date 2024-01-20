#pragma once

#include <array>
#include <functional>
#include <variant>

#include <lib_input/input_callback_handler.hpp>

namespace lib::input
{
struct input_t
{
    bitflag type = input_type::none;
    key_button key = key_button::none;

    std::variant<bool, lib::point2Di> state = {};
};

struct input_callback_t
{
    // what type of input to listen to
    input_type type = input_type::none;

    // something that will be called when the input is updated
    std::function<void(const input_callback_handler&)> callback = nullptr;
};

//! Input handler class
class input_handler final : input_callback_handler
{
public:
    //! Add an input to our handler
    void add_input(const input_t& input);

    //! Register a callback to be called when an input is updated
    void register_callback(uint32_t type, std::function<void(const input_callback_handler&)>&& callback);

protected:
    // Implement interface functions
    [[nodiscard]] const bitflag& get_key_state(key_button key) const override;
    [[nodiscard]] key_button get_last_key() const override;

    [[nodiscard]] const point2Di& get_cursor_position() const override;
    [[nodiscard]] const point2Di& get_cursor_delta() const override;

    [[nodiscard]] const point2Di& get_scroll_delta() const override;

private:
    point2Di _cursor_position = {};
    point2Di _cursor_delta = {};
    point2Di _scroll_delta = {};

    // save input state for all keys
    key_button _last_key = key_button::none;
    std::array<bitflag, static_cast<size_t>(key_button::num_keys)> _key_state = {};

    std::vector<input_callback_t> _input_callbacks = {};

};
}