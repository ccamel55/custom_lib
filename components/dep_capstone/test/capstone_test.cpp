#include <catch2/catch_test_macros.hpp>
#include <dep_capstone/capstone.hpp>

#include <iostream>

TEST_CASE("dep-capstone - basic x86", "[dep-capstone]") {

    constexpr uint8_t data[] = {
        0x48, 0x8b, 0x05,
        0xe0, 0x88, 0x90, 0x00
    };

    csh capstone_handle;
    cs_insn* capstone_instance;

    size_t count = 0;

    REQUIRE(cs_open(CS_ARCH_X86, CS_MODE_64, &capstone_handle) == CS_ERR_OK);
    REQUIRE(cs_option(capstone_handle, CS_OPT_DETAIL, CS_OPT_ON) == CS_ERR_OK);

    count = cs_disasm(capstone_handle, data, sizeof(data), 0x007FFFFF00000000, 0, &capstone_instance);

    REQUIRE(count > 0);

    size_t i = 0;
    for (i = 0; i < count; i++) {
        const auto& instance = capstone_instance[i];
        auto instruction = std::string(instance.mnemonic) + " " + std::string(instance.op_str);

        std::cout << instruction << "\n";
        REQUIRE("mov rax, qword ptr [rip + 0x9088e0]" == instruction);
        REQUIRE(7 == instance.size);

        const auto operand_count = instance.detail->x86.op_count;
        std::cout << "\tnumber of operands: " << static_cast<int>(operand_count) << "\n";

        for (size_t n = 0; n < operand_count; n++) {
            const cs_x86_op& operand = instance.detail->x86.operands[n];
            switch (operand.type) {
                case X86_OP_REG:
                    std::cout << "\t\top: " << cs_reg_name(capstone_handle, operand.reg) << "\n";
                    break;
                case X86_OP_IMM:
                    std::cout << "\t\top: " << operand.imm << "\n";
                    break;
                case X86_OP_MEM:
                    std::cout << "\t\top (mem): \n";

                    if (operand.mem.segment != X86_REG_INVALID) {
                        std::cout << "\t\t\tsegment: " << cs_reg_name(capstone_handle, operand.mem.segment) << "\n";
                    }

                    if (operand.mem.base != X86_REG_INVALID) {
                        std::cout << "\t\t\tbase: " << cs_reg_name(capstone_handle, operand.mem.base) << "\n";
                    }

                    if (operand.mem.index != X86_REG_INVALID) {
                        std::cout << "\t\t\tindex " << cs_reg_name(capstone_handle, operand.mem.index) << "\n";
                    }

                    std::cout << "\t\t\tscale: " << operand.mem.scale << "\n";
                    std::cout << "\t\t\tdisplacement: " << operand.mem.disp << "\n";

                    break;
                default:
                    assert(false);
            }
        }
    }

    REQUIRE(i == 1);

    cs_free(capstone_instance, count);
    cs_close(&capstone_handle);
}

TEST_CASE("dep-capstone - basic arm64", "[dep-capstone]") {

    constexpr uint8_t data[] = {
        0x20, 0x08, 0x00, 0xb1
    };

    csh capstone_handle;
    cs_insn* capstone_instance;

    size_t count = 0;

    REQUIRE(cs_open(CS_ARCH_ARM64, CS_MODE_ARM, &capstone_handle) == CS_ERR_OK);
    count = cs_disasm(capstone_handle, data, sizeof(data), 0x007FFFFF00000000, 0, &capstone_instance);

    REQUIRE(count > 0);

    size_t i = 0;
    for (i = 0; i < count; i++) {
        auto instruction = std::string(capstone_instance[i].mnemonic) + " " + std::string(capstone_instance[i].op_str);
        std::cout << instruction << "\n";

        REQUIRE("adds x0, x1, #2" == instruction);
        REQUIRE(4 == capstone_instance[i].size);
    }

    REQUIRE(i == 1);

    cs_free(capstone_instance, count);
    cs_close(&capstone_handle);
}
