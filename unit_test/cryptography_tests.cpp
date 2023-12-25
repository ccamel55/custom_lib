#include <catch2/catch_test_macros.hpp>
#include <lib_hashing/fnv1a.hpp>

namespace hash = lib::hashing;

TEST_CASE("FNV-1A", "[hashing]")
{
	constexpr auto test_string_1 = "hello poo poo";
	constexpr auto expected_hash_1 = 0x6c068f17;

	constexpr auto test_string_2 = "testtest";
	constexpr auto expected_hash_2 = 0xfb19d205;

	SECTION("compile time evaluation")
	{
		REQUIRE(hash::fnv1a::get_compile_time(test_string_1) == expected_hash_1);
		REQUIRE(hash::fnv1a::get_compile_time(test_string_2) == expected_hash_2);

		REQUIRE(hash::fnv1a::get_compile_time(test_string_1) != hash::fnv1a::get_compile_time(test_string_2));
	}

	SECTION("run time evaluation")
	{
		REQUIRE(hash::fnv1a::get(test_string_1) == expected_hash_1);
		REQUIRE(hash::fnv1a::get(test_string_2) == expected_hash_2);

		REQUIRE(hash::fnv1a::get(test_string_1) != hash::fnv1a::get(test_string_2));
	}
}

TEST_CASE("CRC-32 - todo", "[hashing]")
{
	SECTION("compile time evaluation")
	{
		REQUIRE(true);
	}

	SECTION("run time evaluation")
	{
		REQUIRE(true);
	}
}

TEST_CASE("MD-5 - todo", "[hashing]")
{
	SECTION("compile time evaluation")
	{
		REQUIRE(true);
	}

	SECTION("run time evaluation")
	{
		REQUIRE(true);
	}
}