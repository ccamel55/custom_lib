#include <catch2/catch_test_macros.hpp>

#include <common/types/point/point2D.hpp>
#include <common/types/point/point3D.hpp>
#include <common/types/point/point4D.hpp>

#include <common/types/vector/vector2D.hpp>
#include <common/types/vector/vector3D.hpp>

namespace types = lib::common;

TEST_CASE("Points", "[math]")
{
	types::point2Di point_2d_i_1(1, 2);
	types::point2Di point_2d_i_2(3.0, 4.0);
	types::point2Df point_2d_f_1(1, 2);
	types::point2Df point_2d_f_2(3.0, 4.0);

	types::point3Di point_3d_i_1(1, 2, 3);
	types::point3Di point_3d_i_2(1.0, 2.0, 3.0);
	types::point3Df point_3d_f_1(1, 2, 3);
	types::point3Df point_3d_f_2(1.0, 2.0, 3.0);

	types::point4Di point_4d_i_1(1, 2, 3, 4);
	types::point4Di point_4d_i_2(1.0, 2.0, 3.0, 4.0);
	types::point4Df point_4d_f_1(1, 2, 3, 4);
	types::point4Df point_4d_f_2(1.0, 2.0, 3.0, 4.0);

	SECTION("creation")
	{
		// only test float and integer variants, they all use the same template code so if one works the other
		// will also likely work.

		// point 2d
		REQUIRE(point_2d_i_1._x == 1);
		REQUIRE(point_2d_i_1._y == 2);

		REQUIRE(point_2d_i_2._x == 3);
		REQUIRE(point_2d_i_2._y == 4);

		REQUIRE(point_2d_f_1._x == 1.f);
		REQUIRE(point_2d_f_1._y == 2.f);

		REQUIRE(point_2d_f_2._x == 3.f);
		REQUIRE(point_2d_f_2._y == 4.f);

		// point 3d
		REQUIRE(point_3d_i_1._x == 1);
		REQUIRE(point_3d_i_1._y == 2);
		REQUIRE(point_3d_i_1._z == 3);

		REQUIRE(point_3d_i_2._x == 1);
		REQUIRE(point_3d_i_2._y == 2);
		REQUIRE(point_3d_i_2._z == 3);

		REQUIRE(point_3d_f_1._x == 1.f);
		REQUIRE(point_3d_f_1._y == 2.f);
		REQUIRE(point_3d_f_1._z == 3.f);

		REQUIRE(point_3d_f_2._x == 1.f);
		REQUIRE(point_3d_f_2._y == 2.f);
		REQUIRE(point_3d_f_2._z == 3.f);

		// point 4d
		REQUIRE(point_4d_i_1._x == 1);
		REQUIRE(point_4d_i_1._y == 2);
		REQUIRE(point_4d_i_1._z == 3);
		REQUIRE(point_4d_i_1._w == 4);

		REQUIRE(point_4d_i_2._x == 1);
		REQUIRE(point_4d_i_2._y == 2);
		REQUIRE(point_4d_i_2._z == 3);
		REQUIRE(point_4d_i_2._w == 4);

		REQUIRE(point_4d_f_1._x == 1.f);
		REQUIRE(point_4d_f_1._y == 2.f);
		REQUIRE(point_4d_f_1._z == 3.f);
		REQUIRE(point_4d_f_1._w == 4.f);

		REQUIRE(point_4d_f_2._x == 1.f);
		REQUIRE(point_4d_f_2._y == 2.f);
		REQUIRE(point_4d_f_2._z == 3.f);
		REQUIRE(point_4d_f_2._w == 4.f);
	};

	SECTION("logical operators")
	{
		// point 2d
		REQUIRE(point_2d_i_1 == point_2d_i_1);
		REQUIRE(point_2d_i_1 != point_2d_i_2);

		// point 3d
		REQUIRE(point_3d_i_1 == point_3d_i_1);
		REQUIRE((point_3d_i_1 != point_3d_i_2) == false);

		// point 4d
		REQUIRE(point_4d_i_1 == point_4d_i_1);
		REQUIRE((point_4d_i_1 != point_4d_i_1) == false);
	};
}

TEST_CASE("Vectors - todo", "[math]")
{

}