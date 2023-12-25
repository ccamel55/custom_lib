#pragma once

#include <vector>
#include <array>
#include <memory>
#include <variant>

#include <lib_input/input_receiver.hpp>

namespace lib::input
{
struct input_t
{
    input_type type = input_type::none;
    key_button key = key_button::none;

    std::variant<bool, lib::point2Df> state = {};
};

//! Input handler class
class input_handler
{
    friend class input_receiver;
public:
    input_handler() = default;
    ~input_handler();

    //! Add an input to our handler
    void add_input(const input_t& input);

    //! register a receiver to recieve inputs
    void register_receiver(std::weak_ptr<input_receiver>&& receiver);

private:
    // used by friend class input_receiver only
    [[nodiscard]] input_state get_key_state(key_button key) const;
    [[nodiscard]] key_button get_last_key() const;

    [[nodiscard]] const point2Df& get_cursor_position() const;
    [[nodiscard]] const point2Df& get_cursor_delta() const;
    [[nodiscard]] const point2Df& get_scroll_delta() const;

private:
    std::vector<std::weak_ptr<input_receiver>> _input_receivers = {};

    point2Df _cursor_position = {};
    point2Df _cursor_delta = {};
    point2Df _scroll_delta = {};

    // save input state for all keys
    key_button _last_key = key_button::none;
    std::array<uint32_t, static_cast<size_t>(key_button::num_keys)> _key_state = {};
};
}