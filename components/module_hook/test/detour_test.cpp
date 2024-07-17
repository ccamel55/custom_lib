#include <catch2/catch_test_macros.hpp>
#include <module_hook/Detour.hpp>

namespace
{
__attribute__((noinline)) int get_value() {
    return 0;
}

__attribute__((noinline)) int get_value_hooked() {
    return 69;
}

using fn_t = int(__cdecl*)();

template<lib::hook::DetourApi T>
struct DetourConceptTest {
    T impl = {};
};
}

#if defined(_M_AMD64) || defined(__x86_64__)

#elif defined(_M_IX86) || defined(__i386__)

#elif defined(_M_ARM64) || defined(__aarch64__)

#endif

//TEST_CASE("Detour - Basic", "[module-hook]") {
//    DetourConceptTest<lib::hook::Detour> detour = {};
//
//    lib::memory::address function(reinterpret_cast<uintptr_t>(&get_value));
//    lib::memory::address replacement(reinterpret_cast<uintptr_t>(&get_value_hooked));
//
//    lib::memory::address original = {};
//
//    REQUIRE(get_value() == 0);
//
//    const auto id = detour.impl.intercept(function, replacement, original);
//    REQUIRE(id.has_value());
//
//    REQUIRE(get_value() == 69);
//    REQUIRE(original.cast<fn_t>()() == 0);
//
//    REQUIRE(detour.impl.restore(id.value()));
//
//    REQUIRE(get_value() == 0);
//    REQUIRE(original.cast<fn_t>()() == get_value());
//}