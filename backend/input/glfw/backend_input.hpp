#pragma once

#include <backend/input/backend_input_base.hpp>

namespace lib::backend {

    class backend_input : public backend_input_base {
    public:
        void update_input_state(input_key key, uint32_t state) override;
        bool key_down(input_key key) override;
        bool key_pressed(input_key key) override;
        bool key_released(input_key key) override;
    };
}