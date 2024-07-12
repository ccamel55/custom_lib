#include <module_hook/InlineHook.hpp>

#include <span>
#include <Windows.h>

#include <dep_zydis/zydis.hpp>

namespace
{

// NOTE: IM A RETARD THIS IS x86 I AM ON x64
// TODO: convert into X64 using an ABSOLUTE JMP '0xEE'

// https://www.felixcloutier.com/x86/jmp
// "jmp rel32" == [0xE9, [4]] == sizeof(5)
constexpr size_t JMP_SIZE       = 0x5;

// largest instruction is 15 bytes long
constexpr size_t BYTES_TO_LOAD  = JMP_SIZE + 0x0F;

std::vector<uint8_t> dirty_bytes(lib::memory::address original_function) {
    // Todo: check that our function is at least 5 bytes in size
    ZyanUSize base_address                      = original_function.raw;
    ZyanUSize offset                            = 0;
    ZydisDisassembledInstruction instruction    = {};

    while (ZYAN_SUCCESS(ZydisDisassembleIntel(
        ZYDIS_MACHINE_MODE_LONG_64,
        original_function.raw,
        original_function.ptr<uint8_t>() + offset,
        BYTES_TO_LOAD - offset,
        &instruction
    ))) {
        offset += instruction.info.length;
        base_address += instruction.info.length;

        // If we come across an instruction that uses "rip" the nope the fuck
        // out of there coz we gonna break shit if we patch it.
        // todo: implement fix for "rip" instructions
        for (size_t i = 0; i < instruction.info.operand_count; i++) {
            const auto operand = instruction.operands[i];

            if (operand.type != ZydisOperandType::ZYDIS_OPERAND_TYPE_REGISTER) {
                continue;
            }

            const auto reg_value = operand.reg.value;

            if (reg_value == ZydisRegister::ZYDIS_REGISTER_IP ||
                reg_value == ZydisRegister::ZYDIS_REGISTER_EIP ||
                reg_value == ZydisRegister::ZYDIS_REGISTER_RIP) {
                return {};
            }
        }

        // Smallest number after/equal to 5
        if (offset >= JMP_SIZE) {
            break;
        }
    }

    std::span<uint8_t> instructions(original_function.ptr<uint8_t>(), offset);
    std::vector<uint8_t> dirty_bytes(instructions.begin(), instructions.end());

    return dirty_bytes;
}

std::vector<uint8_t> generate_instruction_patch(
    uint8_t dirty_byte_size,
    int32_t hooked_function_offset
) {
    // https://www.felixcloutier.com/x86/jmp
    std::vector<uint8_t> patch_bytes(JMP_SIZE, 0x0);
    patch_bytes[0] = 0xE9;

    const auto offset_ptr = reinterpret_cast<uint8_t*>(&hooked_function_offset);
    std::copy(offset_ptr, offset_ptr + sizeof(int32_t), patch_bytes.begin() + 1);

    // Fill in remaining overwritten bytes with NOP
    std::vector<uint8_t> nop(dirty_byte_size - JMP_SIZE, 0x90);
    patch_bytes.insert(patch_bytes.end(), nop.begin(), nop.end());

    return patch_bytes;
}

std::vector<uint8_t> generate_trampoline(
    const std::vector<uint8_t>& patch_bytes,
    int32_t trampoline_function_offset
) {
    // https://www.felixcloutier.com/x86/jmp
    std::vector<uint8_t> jmp_bytes(JMP_SIZE, 0x0);
    jmp_bytes[0] = 0xE9;

    const auto offset_ptr = reinterpret_cast<uint8_t*>(&trampoline_function_offset);
    std::copy(offset_ptr, offset_ptr + sizeof(int32_t), jmp_bytes.begin() + 1);

    std::vector<uint8_t> trampoline_bytes(patch_bytes);
    trampoline_bytes.insert(trampoline_bytes.end(), jmp_bytes.begin(), jmp_bytes.end());

    return trampoline_bytes;
}

bool is_address_executable(lib::memory::address address) {
    constexpr auto EXEC_FLAGS =
        PAGE_EXECUTE |
        PAGE_EXECUTE_READ |
        PAGE_EXECUTE_READWRITE |
        PAGE_EXECUTE_WRITECOPY;

    MEMORY_BASIC_INFORMATION mi = {};
    VirtualQuery(address.cast<LPVOID>(), &mi, sizeof(mi));

    return (mi.State == MEM_COMMIT && (mi.Protect & EXEC_FLAGS));
}
}

// https://malwaretech.com/2015/01/inline-hooking-for-programmers-part-1.html
using namespace lib::hook;

bool InlineHook::emplace(
    lib::memory::address original_function,
    lib::memory::address hooked_function,
    lib::memory::address &trampoline_function
) {
    // We will patch in a "jmp rel32" instruction which means we can do
    // at most (original_function + 1 + 2,147,483,647) bytes away or
    // at most (original_function + 1 - 2,147,483,648) bytes behind.
    const auto min_address = original_function.raw + 1 + 2^31 - 1;
    const auto max_address = original_function.raw + 1 - 2 ^ 32;

    if (!is_address_executable(original_function) ||
        !is_address_executable(hooked_function) ||
        hooked_function.raw < min_address ||
        hooked_function.raw > max_address) {
        return false;
    }

    // 1) Find out how many bytes we need to replace
    const auto bytes = dirty_bytes(original_function);

    if (bytes.empty()) {
        return false;
    }

    // 2) Find out how many NOPs we need to insert
    const auto hooked_function_offset = static_cast<int32_t>(hooked_function.raw - original_function.raw);
    const auto patch = generate_instruction_patch(bytes.size(), hooked_function_offset);

    if (patch.empty()) {
        return false;
    }

    // 3) Generate trampoline function to call original
    // note: need to deal with [rip] instructions if we overwrite, should be very unlikely.
    // todo: alloc memory in executable page for our patch and calculate it's offset to our original function.


    return true;
}

bool InlineHook::pop(lib::memory::address original_function) {
    return false;
}

bool InlineHook::apply() {
    return false;
}

bool InlineHook::restore() {
    return false;
}