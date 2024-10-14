#include <catch2/catch_test_macros.hpp>

#include <module_memory/ByteSearch.hpp>
#include <module_memory/ByteSearchHelper.hpp>

TEST_CASE("Byte Search - conversion", "[memory]") {

    std::string_view test_1 = "48 89 5C 24 10 48 89 6C 24 ? 18 56 57 41 54 41 56 41 57 48 83 EC 30 48 8B F1 48 8B 05 E0";
    std::string_view test_2 = " 48  89 5C 24 10 48 89 6C 24  ?? 18 56 57 41 54 41 56 41 57 48 83 EC 30 48 8B F1 48 8B 05 E0 ";
    std::string_view test_3 = "48 89 5C 24 10 48 89 6C 24 ????      18 56 57 41 54 41 56 41 57 48 83 EC 30 48 8B F1 48 8B 05 E0";

    REQUIRE(lib::memory::bytes_from_ida_pattern(test_1) == lib::memory::bytes_from_ida_pattern(test_2));
    REQUIRE(lib::memory::bytes_from_ida_pattern(test_2) == lib::memory::bytes_from_ida_pattern(test_3));
}

TEST_CASE("Byte Search - scan", "[memory]") {

    std::vector<uint8_t> some_bytes = {
       0x88, 0x22, 0xea, 0xff, 0xed, 0xea, 0xef, 0x21, 0x4e, 0x5f, 0xb8, 0x4b, 0x6a, 0xd9, 0xcc, 0x54, 0x46, 0xbc, 0x85, 0x3d, 0xa3, 0xeb,
       0xef, 0xfd, 0x54, 0x85, 0x2e, 0x92, 0xae, 0xd0, 0xd2, 0x86, 0xb6, 0x56, 0x31, 0xba, 0x98, 0x2b, 0xcb, 0x2a, 0x6a, 0x45, 0x9b, 0xb2,
       0x27, 0xfe, 0x8a, 0x9b, 0x6c, 0x09, 0x09, 0x47, 0x6e, 0x9e, 0xf6, 0x2b, 0x65, 0x5c, 0x72, 0x08, 0x49, 0x13, 0x14, 0x5c, 0x76, 0x9e,
       0xcd, 0x98, 0x86, 0xc6, 0x79, 0xbe, 0x74, 0x94, 0xd6, 0x20, 0x59, 0xf8, 0x8a, 0x6f, 0x25, 0xc8, 0xe4, 0x57, 0xc7, 0x44, 0x90, 0x1c,
       0x0a, 0xa6, 0x30, 0xf3, 0x79, 0xfa, 0xea, 0x1c, 0x06, 0xbb, 0xea, 0xcb, 0x68, 0x2b, 0x46, 0x1f, 0x33, 0xa3, 0x21, 0xb7, 0xf9, 0x88,
       0x7c, 0xce, 0x0d, 0x8a, 0x5d, 0xcd, 0x01, 0x03, 0x56, 0x73, 0x43, 0x51, 0x54, 0x37, 0x11, 0x10, 0x78, 0x30, 0xf4, 0xc2, 0x28, 0x5e,
       0xc4, 0x4b, 0xa2, 0x29, 0x76, 0xd7, 0x01, 0x69, 0x48, 0x43, 0xca, 0x53, 0xbd, 0x03, 0xf8, 0x72, 0xbf, 0xf3, 0x4a, 0x05, 0x45, 0x60,
       0x1a, 0x2d, 0xd1, 0xec, 0x9d, 0xef, 0x11, 0x0e, 0xd4, 0x9d, 0x2f, 0x0e, 0x0a, 0x06, 0xbd, 0xaf, 0x9a, 0x0d, 0xbd, 0x93, 0x6b, 0x5f,
       0xdb, 0x94, 0x13, 0x10, 0x70, 0xad, 0x0d, 0x4d, 0x67, 0x5a, 0xb0, 0xaa, 0x24, 0xaa, 0x9b, 0x27, 0x74, 0x97, 0xcc, 0x55, 0x8a, 0x52,
       0x40, 0x2d, 0x19, 0xa9, 0x27, 0x6c, 0xe0, 0xf1, 0xd8, 0x84, 0x72, 0x89, 0x4f, 0x15, 0x64, 0x80, 0xb3, 0x01, 0x3f, 0x88, 0x12, 0x6e,
       0x8a, 0xd3, 0x8a, 0xd3, 0x8a, 0xd3, 0x8a, 0xd3, 0x8a, 0x5a, 0xd9, 0xef, 0xae, 0xec, 0x50, 0x6d, 0xc7, 0x36, 0xa5, 0x9d, 0x08, 0x62,
       0xc3, 0x50, 0xe4, 0x1f, 0x8c, 0xd6, 0x6d, 0x02, 0x18, 0x61, 0x7f, 0x70, 0x17, 0x27
    };

    std::span<std::byte> bye_span = {reinterpret_cast<std::byte*>(some_bytes.data()), some_bytes.size()};
    lib::memory::memory_section section(bye_span);

    lib::memory::ByteSearch search(section);

    // Pattern for start of bytes (should return address of index 0)
    const auto result_1 = search.find(lib::memory::bytes_from_ida_pattern("88 22 ea ff ed ea ef 21 4e 5f b8 4b 6a"));
    const auto result_2 = search.find(lib::memory::bytes_from_ida_pattern("88 22 ea ?? ed ea ef 21 ? 5f b8 4b 6a"));

    REQUIRE(result_1.has_value());
    REQUIRE(result_2.has_value());

    REQUIRE(result_2.value() == result_2.value());
    REQUIRE(result_2.value().ptr<uint8_t>() == &some_bytes.at(0));

    // Invalid pattern
    const auto result_3 = search.find(lib::memory::bytes_from_ida_pattern("ff ff ff ff ff ff ff ff"));
    REQUIRE(!result_3.has_value());

    // Partial match (should equal address at 225 not 219)
    const auto result_4 = search.find(lib::memory::bytes_from_ida_pattern("8a d3 8a d3 8a d3 8a 5a"));
    REQUIRE(result_4.has_value());
    REQUIRE(result_4.value().ptr<uint8_t>() == &some_bytes.at(222));
}

