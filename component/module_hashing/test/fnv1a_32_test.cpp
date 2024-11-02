#include <catch2/catch_test_macros.hpp>
#include <module_hashing/fnv1a_32.hpp>

TEST_CASE("Fnv1a32", "[hashing]") {
    constexpr auto expected_hash_1 = 0x6c068f17;
    constexpr auto expected_hash_2 = 0xfb19d205;

    SECTION("Compile time evaluation") {
        constexpr lib::hashing::fnv1a_32_t test_string_1_hash   = lib::hashing::detail::fnv1a_32_ct("hello poo poo");
        constexpr lib::hashing::fnv1a_32_t test_string_2_hash   = lib::hashing::detail::fnv1a_32_ct("testtest");

        REQUIRE(test_string_1_hash == expected_hash_1);
        REQUIRE(test_string_2_hash == expected_hash_2);

        REQUIRE(test_string_1_hash != test_string_2_hash);
    }

    SECTION("Runtime evaluation") {
        std::string test_string_runtime_1 = "hello poo poo";
        std::string test_string_runtime_2 = "testtest";

        REQUIRE(lib::hashing::fnv1a_32(test_string_runtime_1.c_str()) == expected_hash_1);
        REQUIRE(lib::hashing::fnv1a_32(test_string_runtime_2.c_str()) == expected_hash_2);

        REQUIRE(
            lib::hashing::fnv1a_32(test_string_runtime_1.c_str()) !=
            lib::hashing::fnv1a_32(test_string_runtime_2.c_str())
        );
    }
}
