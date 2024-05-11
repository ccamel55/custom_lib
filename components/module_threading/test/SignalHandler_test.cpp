#include <catch2/catch_test_macros.hpp>

#include <module_threading/SignalHandler.hpp>

#include <condition_variable>

TEST_CASE("Signal handler - true", "[threading]") {
    REQUIRE(true);
}

TEST_CASE("Signal handler - blocking", "[threading]") {
    int counter = 0;
    lib::threading::SignalHandler<int> handler(lib::threading::signal_invoke_mode::BLOCKING);

    REQUIRE(counter == 0);
    REQUIRE(handler.empty());

    const auto id = handler.try_emplace([&](int c){
        counter += c;
    });

    REQUIRE(id.has_value());
    REQUIRE(handler.size() == 1);

    handler.invoke(1);
    REQUIRE(counter == 1);

    REQUIRE(handler.try_erase(id.value()) == true);
    REQUIRE(handler.empty());
}