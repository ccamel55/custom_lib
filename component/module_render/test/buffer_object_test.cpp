#include <catch2/catch_test_macros.hpp>

#include <module_render/Render.hpp>
#include <module_render/types/buffer_object.hpp>

#include <iostream>

TEST_CASE("Render - Buffer object valid", "[render]") {

    const lib::render::buffer_object_t original_buffer(nullptr, nullptr, 0, 100);

    //
    // Create a new buffers
    //

    const auto buffer_1 = lib::render::buffer_object_t::create(
        original_buffer,
        0,
        50
    );

    REQUIRE(buffer_1.has_value() == true);
    REQUIRE(buffer_1->offset()   == 0);
    REQUIRE(buffer_1->size()     == 50);

    const auto buffer_2 = lib::render::buffer_object_t::create(
        original_buffer,
        50,
        50
    );

    REQUIRE(buffer_2.has_value() == true);
    REQUIRE(buffer_2->offset()   == 50);
    REQUIRE(buffer_2->size()     == 50);

    //
    // Create another buffer lower down
    //

    const auto buffer_3 = lib::render::buffer_object_t::create(
        buffer_1.value(),
        0,
        50
    );

    REQUIRE(buffer_3.has_value() == true);
    REQUIRE(buffer_3->offset()   == 0);
    REQUIRE(buffer_3->size()     == 50);

    const auto buffer_4 = lib::render::buffer_object_t::create(
        buffer_2.value(),
        0,
        50
    );

    REQUIRE(buffer_4.has_value() == true);
    REQUIRE(buffer_2->offset()   == 50);
    REQUIRE(buffer_2->size()     == 50);
}

TEST_CASE("Render - Buffer object invalid", "[render]") {

    const lib::render::buffer_object_t original_buffer(nullptr, nullptr, 0, 10);

    {
        const auto buffer_1 = lib::render::buffer_object_t::create(
            original_buffer,
            0,
            11
        );

        REQUIRE(buffer_1.has_value() == false);
    }

    {
        const auto buffer_1 = lib::render::buffer_object_t::create(
            original_buffer,
            2,
            10
        );

        REQUIRE(buffer_1.has_value() == false);
    }

    {
        const auto buffer_1 = lib::render::buffer_object_t::create(
            original_buffer,
            1,
            9
        );

        REQUIRE(buffer_1.has_value() == true);

        const auto buffer_2 = lib::render::buffer_object_t::create(
            original_buffer,
            0,
            0
        );

        REQUIRE(buffer_2.has_value() == false);
    }
}
