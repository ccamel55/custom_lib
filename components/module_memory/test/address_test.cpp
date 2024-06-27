#include <catch2/catch_test_macros.hpp>
#include <module_memory/type/address.hpp>

namespace
{
struct test_struct_t {
    int value_1 = 0;
    int value_2 = 1;
};
}

TEST_CASE("Address - basic", "[memory]") {
    test_struct_t some_struct = {};
    test_struct_t* some_struct_ptr = &some_struct;

    REQUIRE(some_struct.value_1 == 0);
    REQUIRE(some_struct.value_2 == 1);

    REQUIRE(some_struct_ptr != nullptr);

    // Check that raw is same as ptr
    lib::memory::address variable_address(some_struct_ptr);
    lib::memory::address variable_address_2(some_struct_ptr);

    REQUIRE(variable_address.ptr<test_struct_t>() == some_struct_ptr);

    REQUIRE(variable_address.get<test_struct_t>().value_1 == 0);
    REQUIRE(variable_address.get<test_struct_t>().value_2 == 1);

    REQUIRE(variable_address.get<int>() == 0);
    REQUIRE(variable_address.offset(sizeof(int)).get<int>() == 1);

    // Mutate and test
    variable_address.get<test_struct_t>().value_1 = 10;
    REQUIRE(variable_address_2.get<test_struct_t>().value_1 == 10);
}

TEST_CASE("Address - dereference", "[memory]") {
    test_struct_t some_struct = {};
    test_struct_t* some_struct_ptr = &some_struct;
    test_struct_t** some_struct_ptr_ptr = &some_struct_ptr;

    lib::memory::address variable_address(some_struct_ptr_ptr);

    REQUIRE(variable_address.get<test_struct_t*>()->value_1 == 0);
    REQUIRE(variable_address.get<test_struct_t*>()->value_2 == 1);

    const auto deref = variable_address.dereference();

    REQUIRE(deref.get<test_struct_t>().value_1 == 0);
    REQUIRE(deref.get<test_struct_t>().value_2 == 1);
}