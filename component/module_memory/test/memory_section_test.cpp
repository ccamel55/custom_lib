#include <catch2/catch_test_macros.hpp>
#include <module_memory/type/memory_section.hpp>

#include <vector>

TEST_CASE("Memory Section - basic", "[memory]") {
    std::vector<int> data(100, 69);
    std::span<std::byte> data_span(reinterpret_cast<std::byte*>(data.data()), data.size() * sizeof(int));

    const auto memory_section = lib::memory::memory_section(data_span);

    REQUIRE(memory_section.base() == lib::memory::address(data.data()));
    REQUIRE(memory_section.size<int>() == data.size());
    REQUIRE(memory_section.contains(lib::memory::address(&data.at(1))));
}

