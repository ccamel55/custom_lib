#include <catch2/catch_test_macros.hpp>

#include <module_threading/Timer.hpp>

TEST_CASE("Timer - one shot", "[threading]") {
    lib::threading::Timer timer(
        lib::threading::timer_mode::ONE_SHOT,
        std::chrono::milliseconds(50)
    );

    int counter = 0;

    const auto callback = [&]{
        counter += 1;
    };

    timer.emplace(std::move(callback));

    REQUIRE(counter == 0);

    timer.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(250));

    REQUIRE(counter == 1);

    timer.reset();

    REQUIRE(counter == 1);

    timer.clear();
    timer.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(250));

    REQUIRE(counter == 1);
}

TEST_CASE("Timer - reloading", "[threading]") {
    lib::threading::Timer timer(
        lib::threading::timer_mode::RELOADING,
        std::chrono::milliseconds(50)
    );

    int counter = 0;

    const auto callback = [&]{
        counter += 1;
    };

    timer.emplace(std::move(callback));

    REQUIRE(counter == 0);

    timer.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(250));

    REQUIRE(counter > 1);
    const auto old_counter = counter;

    timer.reset();

    REQUIRE(counter == old_counter);

    timer.clear();
    timer.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(250));

    REQUIRE(counter == old_counter);
}

TEST_CASE("Timer - empty", "[threading]") {
    lib::threading::Timer timer(
        lib::threading::timer_mode::RELOADING,
        std::chrono::milliseconds(50)
    );

    timer.reset();
}