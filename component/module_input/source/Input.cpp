#include <module_input/Input.hpp>

using namespace lib::input;

void Input::add_input(bitflag type, key key, const std::variant<bool, lib::point2Di>& state) {
    if (key == key::NONE) {
        return;
    }

    const auto key_int = static_cast<uint32_t>(key);

    switch (key) {
        case key::MOUSE_MOVE: {
            const auto& cursor_position = std::get<lib::point2Di>(state);

            _cursor_delta = cursor_position - _cursor_position;
            _cursor_position = cursor_position;

            break;
        }
        case key::MOUSE_SCROLL: {
            _scroll_delta = std::get<lib::point2Di>(state);
            break;
        }
        default: {
            _last_key = key;

            if ( std::get<bool>(state)) {
                _key_state.at(key_int).add(BUTTON_STATE_DOWN | BUTTON_STATE_PRESSED);
                _key_state.at(key_int).remove(BUTTON_STATE_UP);
            }
            else {
                _key_state.at(key_int).add(BUTTON_STATE_UP | BUTTON_STATE_RELEASED);
                _key_state.at(key_int).remove(BUTTON_STATE_DOWN);
            }

            break;
        }
    }

    for (const auto& receiver : _callbacks) {
        const auto& [callback_type, callback] = receiver;

        if (type.has(callback_type)) {
            callback(*this);
        }
    }

    // reset some states
    _cursor_delta = {};
    _scroll_delta = {};

    _key_state.at(key_int).remove(BUTTON_STATE_PRESSED | BUTTON_STATE_RELEASED);
}

void Input::register_callback(bitflag type, std::function<void(const InputObserver &)>&& callback) {
    input_callback_t input_callback = {};
    {
        input_callback.type         = type;
        input_callback.callback     = std::move(callback);
    }
    _callbacks.emplace_back(std::move(input_callback));
}

lib::bitflag Input::internal_get_state(key key) const {
    return _key_state.at(static_cast<uint32_t>(key));
}

key Input::internal_last_key() const {
    return _last_key;
}

const lib::point2Di& Input::internal_cursor_position() const {
    return _cursor_position;
}

const lib::point2Di& Input::internal_cursor_delta() const {
    return _cursor_delta;
}

const lib::point2Di& Input::internal_scroll_delta() const {
    return _scroll_delta;
}