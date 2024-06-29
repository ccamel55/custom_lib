#include <catch2/catch_test_macros.hpp>
#include <dep_minhook/minhook.hpp>

namespace
{
__attribute__((noinline)) int get_value() {
    return 0;
}

__attribute__((noinline)) int get_value_hooked() {
    return 69;
}

using fn_t = int(__cdecl*)();
}

TEST_CASE("dep-minhook - Basic", "[dep-minhook]") {
    fn_t get_value_original = nullptr;

    REQUIRE(MH_Initialize() == MH_OK);
    REQUIRE(
        MH_CreateHook(
            reinterpret_cast<LPVOID>(&get_value),
            reinterpret_cast<LPVOID>(&get_value_hooked),
            reinterpret_cast<LPVOID*>(&get_value_original)
        ) == MH_OK
    );

    REQUIRE(get_value() == 0);

    REQUIRE(MH_EnableHook(reinterpret_cast<LPVOID>(&get_value)) == MH_OK);

    REQUIRE(get_value() == 69);
    REQUIRE(get_value_original() == 0);

    REQUIRE(MH_DisableHook(reinterpret_cast<LPVOID>(&get_value)) == MH_OK);

    REQUIRE(get_value() == 0);

    REQUIRE(MH_Uninitialize() == MH_OK);
}