#include <catch2/catch_test_macros.hpp>
#include <module_memory/type/vtable.hpp>

#include <memory>

namespace
{
class SomeVirtualClass {
public:
    virtual ~SomeVirtualClass() = default;

    virtual int func_1() {
        return 1;
    }

    virtual int func_2() {
        return 2;
    }

    virtual int func_3() {
        return 3;
    }
};
}

TEST_CASE("VTable - basic", "[memory]") {
    auto my_class = std::make_unique<SomeVirtualClass>();

    auto vtable_address = lib::memory::address(my_class.get());
    auto vtable = lib::memory::vtable(vtable_address);

    // Linux has slighly different VTable from windows
#ifdef _WIN32
    REQUIRE(vtable.size() == 5);

    using fn_sig = int(*)();

    // Index 0 is the destructor
    REQUIRE(vtable.get(0).has_value());

    // Wacky stuff here :)
    REQUIRE(vtable.get(1).value().cast<fn_sig>()() == my_class->func_1());
    REQUIRE(vtable.get(2).value().cast<fn_sig>()() == my_class->func_2());
    REQUIRE(vtable.get(3).value().cast<fn_sig>()() == my_class->func_3());
#else
    REQUIRE(vtable.size() == 5);

    using fn_sig = int(*)();

    // Index 0 is the destructor
    REQUIRE(vtable.get(0).has_value());
    REQUIRE(vtable.get(1).has_value());

    // Wacky stuff here :)
    REQUIRE(vtable.get(2).value().cast<fn_sig>()() == my_class->func_1());
    REQUIRE(vtable.get(3).value().cast<fn_sig>()() == my_class->func_2());
    REQUIRE(vtable.get(4).value().cast<fn_sig>()() == my_class->func_3());
#endif
}