#include <catch2/catch_test_macros.hpp>

#include <module_core/Singleton.hpp>

namespace
{
class SingletonClass : public lib::Singleton<SingletonClass> {
public:
    [[nodiscard]] int get_counter() const {
        return _counter;
    }

    [[nodiscard]] int increment_counter() {
        return ++_counter;
    }

private:
    int _counter = 0;

};
}

TEST_CASE("Singleton - test", "[core]") {
    REQUIRE(SingletonClass::get().get_counter() == 0);
    REQUIRE(SingletonClass::get().increment_counter() == 1);

    REQUIRE(SingletonClass::get().get_counter() == 1);
    REQUIRE(SingletonClass::get().increment_counter() == 2);
}
