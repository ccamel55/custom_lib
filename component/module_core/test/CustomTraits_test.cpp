#include <catch2/catch_test_macros.hpp>

#include <module_core/custom_traits.hpp>

#include <iostream>

TEST_CASE("Custom Traits - type id", "[core]") {
    const auto int_id    = lib::type_id<int>();
    const auto float_id  = lib::type_id<float>();
    const auto double_id = lib::type_id<double>();

    REQUIRE(int_id == lib::type_id<int>());
    REQUIRE(float_id == lib::type_id<float>());
    REQUIRE(double_id == lib::type_id<double>());

    REQUIRE(int_id != float_id);
    REQUIRE(float_id != double_id);
}

TEST_CASE("Custom Traits - is present in pack params", "[core]") {
    REQUIRE(
        lib::is_type_present_v<
        int,
        int,
        float,
        void,
        double> == true
    );

    REQUIRE(
        lib::is_type_present_v<
        int,
        float,
        void,
        double> == false
    );
}

TEST_CASE("Custom Traits - iterate integer sequence", "[core]") {
    std::integer_sequence<int, 1, 3, 5, 7, 9> odd_under_10 = { };
    std::vector<int> integrals                             = { };

    lib::iterate_integer_sequence(
        odd_under_10,
        [&](auto integral_constant) {
            REQUIRE(integral_constant % 2 == 1);
            integrals.emplace_back(integral_constant);
        }
    );

    REQUIRE(integrals.size() == odd_under_10.size());
}
