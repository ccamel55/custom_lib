#pragma once

#include <array>
#include <functional>
#include <variant>
#include <vector>

#include <module_input/InputObserver.hpp>

namespace lib::input
{
// Alias correct type
class Input;
using InputObserver = InputObserver_Base<Input>;

//! Input handler
class Input : public InputObserver {
    friend class InputObserver_Base;

public:
    //! Send a new input to our handler
    //! \param type input type
    //! \param key key that was affected
    //! \param state the state of the key
    void add_input(bitflag type, key key, const std::variant<bool, lib::point2Di>& state);

    //! Register a new callback function that gets invoked when a key is updated
    //! \param type input type to listen for
    //! \param callback callback that is invoked
    void register_callback(bitflag type, std::function<void(const InputObserver&)>&& callback);

private:
    struct input_callback_t {
        bitflag type                                        = INPUT_TYPE_NONE;
        std::function<void(const InputObserver&)> callback  = nullptr;
    };

    [[nodiscard]] bitflag internal_get_state(key key) const;
    [[nodiscard]] key internal_last_key() const;
    [[nodiscard]] const point2Di& internal_cursor_position() const;
    [[nodiscard]] const point2Di& internal_cursor_delta() const;
    [[nodiscard]] const point2Di& internal_scroll_delta() const;

private:
    lib::point2Di _cursor_position  = {};
    lib::point2Di _cursor_delta     = {};
    lib::point2Di _scroll_delta     = {};

    key _last_key                                                       = key::NONE;
    std::array<bitflag, static_cast<size_t>(key::NUM_KEYS)> _key_state  = {};

    std::vector<input_callback_t> _callbacks    = {};

};
}