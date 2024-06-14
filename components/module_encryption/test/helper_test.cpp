#include <catch2/catch_test_macros.hpp>
#include <module_encryption/helper/compile_time_seed.hpp>

namespace {
    constexpr uint32_t SOME_SEED_1 = lib::encryption::helper::seed_32();
    constexpr uint32_t SOME_SEED_2 = lib::encryption::helper::seed_32();
}

TEST_CASE("Encryption helper - compile time seed", "[encryption]") {
    constexpr uint32_t SOME_SEED_3 = lib::encryption::helper::seed_32();

    REQUIRE(SOME_SEED_1 == SOME_SEED_2);
    REQUIRE(SOME_SEED_2 == SOME_SEED_3);

    static_assert(SOME_SEED_1 == SOME_SEED_2);
    static_assert(SOME_SEED_2 == SOME_SEED_3);
}
