#include <catch2/catch_test_macros.hpp>

#include <module_core/types/point/point2D.hpp>
#include <module_core/types/point/point3D.hpp>
#include <module_core/types/point/point4D.hpp>

TEST_CASE("Point 2D - Initialisation", "[core]") {
    lib::point2Di point_2d_i_1(1, 2);

    REQUIRE(point_2d_i_1.x == 1);
    REQUIRE(point_2d_i_1.y == 2);

    lib::point2Di point_2d_i_2(3.0, 4.0);

    REQUIRE(point_2d_i_2.x == 3);
    REQUIRE(point_2d_i_2.y == 4);

    lib::point2Df point_2d_f_1(1, 2);

    REQUIRE(point_2d_f_1.x == 1.f);
    REQUIRE(point_2d_f_1.y == 2.f);

    lib::point2Df point_2d_f_2(3.0, 4.0);

    REQUIRE(point_2d_f_2.x == 3.f);
    REQUIRE(point_2d_f_2.y == 4.f);
}

TEST_CASE("Point 3D - Initialisation", "[core]") {
    lib::point3Di point_3d_i_1(1, 2, 3);

    REQUIRE(point_3d_i_1.x == 1);
    REQUIRE(point_3d_i_1.y == 2);
    REQUIRE(point_3d_i_1.z == 3);

    lib::point3Di point_3d_i_2(1.0, 2.0, 3.0);

    REQUIRE(point_3d_i_2.x == 1);
    REQUIRE(point_3d_i_2.y == 2);
    REQUIRE(point_3d_i_2.z == 3);

    lib::point3Df point_3d_f_1(1, 2, 3);

    REQUIRE(point_3d_f_1.x == 1.f);
    REQUIRE(point_3d_f_1.y == 2.f);
    REQUIRE(point_3d_f_1.z == 3.f);

    lib::point3Df point_3d_f_2(1.0, 2.0, 3.0);

    REQUIRE(point_3d_f_2.x == 1.f);
    REQUIRE(point_3d_f_2.y == 2.f);
    REQUIRE(point_3d_f_2.z == 3.f);
}

TEST_CASE("Point 4D - Initialisation", "[core]") {
    lib::point4Di point_4d_i_1(1, 2, 3, 4);

    REQUIRE(point_4d_i_1.x == 1);
    REQUIRE(point_4d_i_1.y == 2);
    REQUIRE(point_4d_i_1.z == 3);
    REQUIRE(point_4d_i_1.w == 4);

    lib::point4Di point_4d_i_2(1.0, 2.0, 3.0, 4.0);

    REQUIRE(point_4d_i_2.x == 1);
    REQUIRE(point_4d_i_2.y == 2);
    REQUIRE(point_4d_i_2.z == 3);
    REQUIRE(point_4d_i_2.w == 4);

    lib::point4Df point_4d_f_1(1, 2, 3, 4);

    REQUIRE(point_4d_f_1.x == 1.f);
    REQUIRE(point_4d_f_1.y == 2.f);
    REQUIRE(point_4d_f_1.z == 3.f);
    REQUIRE(point_4d_f_1.w == 4.f);

    lib::point4Df point_4d_f_2(1.0, 2.0, 3.0, 4.0);

    REQUIRE(point_4d_f_2.x == 1.f);
    REQUIRE(point_4d_f_2.y == 2.f);
    REQUIRE(point_4d_f_2.z == 3.f);
    REQUIRE(point_4d_f_2.w == 4.f);
}