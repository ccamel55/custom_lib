#pragma once

#include <singleton.hpp>

#include <array>


namespace lib::backend {

    using input_key = uint32_t;

    //! base class for input handling
    class backend_input_base : public shared_singleton<backend_input_base> {
    public:
        //! returns whether \param key is currently pressed
        virtual bool key_down(input_key key) = 0;

        //! returns whether \param key was just pressed
        virtual bool key_pressed(input_key key) = 0;

        //! returns whether \param key was just released
        virtual bool key_released(input_key key) = 0;
    protected:
        input_key _current_key = 0;
        bool _current_state = false;

        int _scroll_state = 0;

        std::array<bool, 256> _key_state{};
        std::array<bool, 256> _prev_key_state{};
    };
}