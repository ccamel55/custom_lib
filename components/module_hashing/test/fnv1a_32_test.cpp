#include <catch2/catch_test_macros.hpp>
#include <module_hashing/fnv1a_32.hpp>

TEST_CASE("FNV1A", "[hashing]") {
    constexpr auto test_string_1 = "hello poo poo";
    constexpr auto expected_hash_1 = 0x6c068f17;

    constexpr auto test_string_2 = "testtest";
    constexpr auto expected_hash_2 = 0xfb19d205;

    SECTION("Compile time evaluation")
    {
        REQUIRE(lib::hashing::fnv1a_32(test_string_1) == expected_hash_1);
        REQUIRE(lib::hashing::fnv1a_32(test_string_2) == expected_hash_2);

        REQUIRE(lib::hashing::fnv1a_32(test_string_1) != lib::hashing::fnv1a_32(test_string_2));
    }

    SECTION("Runtime evaluation")
    {
        std::string test_string_runtime_1 = "hello poo poo";
        std::string test_string_runtime_2 = "testtest";

        REQUIRE(lib::hashing::fnv1a_32(test_string_runtime_1.c_str()) == expected_hash_1);
        REQUIRE(lib::hashing::fnv1a_32(test_string_runtime_2.c_str()) == expected_hash_2);

        REQUIRE(lib::hashing::fnv1a_32(test_string_runtime_1.c_str()) !=
                lib::hashing::fnv1a_32(test_string_runtime_2.c_str()));
    }
}