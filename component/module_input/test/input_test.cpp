#include <catch2/catch_test_macros.hpp>
#include <module_input/Input.hpp>

#include <atomic>

TEST_CASE("Input - Basic", "[input]") {
    lib::input::Input input_handler = {};

    class InputObserver final: public lib::input::InputPass {
    public:
        void update_input(const lib::bitflag type, const lib::input::InputObserver& input) override {
            if (type != lib::input::INPUT_TYPE_MOUSE) {
                return;
            }

            if (input.get_state(lib::input::key::MOUSE_LEFT).has(lib::input::BUTTON_STATE_PRESSED)) {
                press_counter += 1;
            }

            if (input.get_state(lib::input::key::MOUSE_LEFT).has(lib::input::BUTTON_STATE_PRESSED)) {
                release_counter += 1;
            }
        }

        std::atomic_int press_counter   = 0;
        std::atomic_int release_counter = 0;
    };

    InputObserver obs = {};
    input_handler.emplace_pass(&obs);

    // Should not increment counter since its of type keyboard
    input_handler.add_input(lib::input::INPUT_TYPE_KEYBOARD, lib::input::key::MOUSE_LEFT, true);
    input_handler.add_input(lib::input::INPUT_TYPE_KEYBOARD, lib::input::key::MOUSE_LEFT, false);

    REQUIRE(obs.press_counter   == 0);
    REQUIRE(obs.release_counter == 0);

    // Should not increment counter since it's the wrong input type
    input_handler.add_input(lib::input::INPUT_TYPE_MOUSE, lib::input::key::MOUSE_RIGHT, true);
    input_handler.add_input(lib::input::INPUT_TYPE_MOUSE, lib::input::key::MOUSE_RIGHT, false);

    REQUIRE(obs.press_counter   == 0);
    REQUIRE(obs.release_counter == 0);

    input_handler.add_input(lib::input::INPUT_TYPE_MOUSE, lib::input::key::MOUSE_LEFT, true);
    input_handler.add_input(lib::input::INPUT_TYPE_MOUSE, lib::input::key::MOUSE_LEFT, false);

    REQUIRE(obs.press_counter   == 1);
    REQUIRE(obs.release_counter == 1);

    input_handler.add_input(lib::input::INPUT_TYPE_MOUSE, lib::input::key::MOUSE_LEFT, true);
    input_handler.add_input(lib::input::INPUT_TYPE_MOUSE, lib::input::key::MOUSE_LEFT, false);

    REQUIRE(obs.press_counter   == 2);
    REQUIRE(obs.release_counter == 2);

    input_handler.add_input(lib::input::INPUT_TYPE_MOUSE, lib::input::key::MOUSE_LEFT, true);
    input_handler.add_input(lib::input::INPUT_TYPE_MOUSE, lib::input::key::MOUSE_LEFT, false);

    REQUIRE(obs.press_counter   == 3);
    REQUIRE(obs.release_counter == 3);
}
