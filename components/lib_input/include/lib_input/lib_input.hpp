#pragma once

#include <functional>
#include <variant>

#include <lib_input/input_processor.hpp>

namespace lib::input
{
struct input_t
{
    input_type type = input_type::none;
    key_button key = key_button::none;

    std::variant<bool, lib::point2Df> state = {};
};

struct input_callback_t
{
    // what type of input to listen to
    input_type type = input_type::none;

    // something that will be called when the input is updated
    std::function<void(const input_processor&)> callback = nullptr;
};

//! Input handler class
class input_handler
{
public:
    //! Add an input to our handler
    void add_input(const input_t& input);

    //! Register a callback to be called when an input is updated
    void register_callback(input_type type, std::function<void(const input_processor&)>&& callback);

private:
    input_processor _input_processor = {};
    std::vector<input_callback_t> _input_callbacks = {};

};
}