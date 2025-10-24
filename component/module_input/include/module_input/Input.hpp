#pragma once

#include <array>
#include <list>
#include <variant>

#include <module_input/InputObserver.hpp>

namespace lib::input
{
// Alias correct type
class Input;
using InputObserver = InputObserver_Base<Input>;

class InputPass {
public:
    virtual ~InputPass() = default;
    virtual void update_input(bitflag type, const InputObserver& input) = 0;

};

//! Input handler
class Input : public InputObserver {
    friend class InputObserver_Base;

public:
    //! Send a new input to our handler
    //! \param type input type
    //! \param key key that was affected
    //! \param state the state of the key
    void add_input(bitflag type, key key, const std::variant<bool, point2Di>& state);

    //! Add new input callback pass
    //! \param pass pointer to pass instance
    void emplace_pass(InputPass* pass);

    //! Erase input callback pass
    //! \param pass pointer to pass to erase
    void erase_pass(InputPass* pass);

private:
    [[nodiscard]] bitflag internal_get_state(key key) const;
    [[nodiscard]] key internal_last_key() const;
    [[nodiscard]] const point2Di& internal_cursor_position() const;
    [[nodiscard]] const point2Di& internal_cursor_delta() const;
    [[nodiscard]] const point2Di& internal_scroll_delta() const;

private:
    point2Di _cursor_position  = {};
    point2Di _cursor_delta     = {};
    point2Di _scroll_delta     = {};

    key _last_key                                                       = key::NONE;
    std::array<bitflag, static_cast<size_t>(key::NUM_KEYS)> _key_state  = {};

    std::list<InputPass*> _passes = {};

};
}