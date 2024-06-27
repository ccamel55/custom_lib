#include <catch2/catch_test_macros.hpp>

#include <module_core/types/vector/vector2D.hpp>
#include <module_core/types/vector/vector3D.hpp>

TEST_CASE("Vector 2D - Initialisation", "[core]") {
    lib::vector2D point_2d_i_1(1, 2);

    REQUIRE(point_2d_i_1.x == 1);
    REQUIRE(point_2d_i_1.y == 2);

    lib::vector2D point_2d_i_2(3.0, 4.0);

    REQUIRE(point_2d_i_2.x == 3);
    REQUIRE(point_2d_i_2.y == 4);
}

TEST_CASE("Vector 3D - Initialisation", "[core]") {
    lib::vector3D point_3d_i_1(1, 2, 3);

    REQUIRE(point_3d_i_1.x == 1);
    REQUIRE(point_3d_i_1.y == 2);
    REQUIRE(point_3d_i_1.z == 3);

    lib::vector3D point_3d_i_2(1.0, 2.0, 3.0);

    REQUIRE(point_3d_i_2.x == 1);
    REQUIRE(point_3d_i_2.y == 2);
    REQUIRE(point_3d_i_2.z == 3);
}