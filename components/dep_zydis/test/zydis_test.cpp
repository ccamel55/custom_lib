#include <catch2/catch_test_macros.hpp>
#include <dep_zydis/zydis.hpp>

#include <iostream>

TEST_CASE("Decode - simple", "[dep_zydis]") {
    constexpr ZyanU8 data[] = {
        0x48, 0x8b, 0x05,
        0xe0, 0x88, 0x90, 0x00
    };

    // Random base address chosen,
    uintptr_t base_address = 0x007FFFFF00000000 - sizeof(data);

    ZyanUSize offset                         = 0;
    ZydisDisassembledInstruction instruction = {};

    int count = 0;

    // Intel syntax ASM - should only have one instruction
    while (ZYAN_SUCCESS(ZydisDisassembleIntel(
        ZYDIS_MACHINE_MODE_LONG_64,
        base_address,
        data + offset,
        sizeof(data) - offset,
        &instruction
    ))) {
        std::cout << instruction.text << "\n";
        REQUIRE("mov rax, [0x007FFFFF009088E0]" == std::string(instruction.text));

        offset += instruction.info.length;
        base_address += instruction.info.length;

        count += 1;
    }

    REQUIRE(1 == count);
}