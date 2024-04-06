#include <catch2/catch_test_macros.hpp>
#include <core_sdk/templates/serialization.hpp>

namespace
{
	struct example_data_struct
	{
		int my_int_1 = 0;
		long my_long_1 = 0l;
		float my_float_1 = 0.f;
		double my_double_1 = 0.0;

		// a constexpr static function named get_metadata must be defined for each class that wishes to be/
		// serializable
		static constexpr auto get_metadata()
		{
			return std::tuple{
				serializable(example_data_struct, my_int_1)
				serializable(example_data_struct, my_long_1)
				serializable(example_data_struct, my_float_1)
				serializable(example_data_struct, my_double_1)
			};
		}
	};
}

TEST_CASE("Check size and value", "[serialization]")
{
	// make sure all 4 elements exist
	REQUIRE(lib::get_metadata_size<example_data_struct>() == 4);

	// make sure each index has the correct value
	const auto metadata = example_data_struct::get_metadata();

	REQUIRE(std::get<0>(metadata).member_name == "my_int_1");
	REQUIRE(std::get<1>(metadata).member_name == "my_long_1");
	REQUIRE(std::get<2>(metadata).member_name == "my_float_1");
	REQUIRE(std::get<3>(metadata).member_name == "my_double_1");
}