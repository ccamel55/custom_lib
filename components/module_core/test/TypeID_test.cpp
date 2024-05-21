#include <catch2/catch_test_macros.hpp>

#include <module_core/type_id.hpp>

TEST_CASE("TypeID - basic", "[core]") {
    const auto int_id = lib::type_id<int>();
    const auto float_id = lib::type_id<float>();
    const auto double_id = lib::type_id<double>();

    REQUIRE(int_id == lib::type_id<int>());
    REQUIRE(float_id == lib::type_id<float>());
    REQUIRE(double_id == lib::type_id<double>());

    REQUIRE(int_id != float_id);
    REQUIRE(float_id != double_id);
}