#include <module_hook/Trampoline.hpp>

#ifdef ARCH_x86_64

#include <limits>

using namespace lib::hook;

namespace
{
// Don't align this struct otherwise our OP code/operands might be in wrong address/byte
// see https://www.felixcloutier.com/x86/jmp for opcode reference
#pragma pack(push, 1)
struct JMP_relative_8_t{
    const uint8_t OP_0  = 0xEB; // '0xEb'   = jmp relative short
    int8_t OPERAND_0    = 0x00; // '0x00'   = offset from 'rip'
};

struct JMP_relative_t {
    const uint8_t OP_0  = 0xE9; // '0xE9'       = jmp relative near
    int32_t OPERAND_0   = 0x00;	// '0x00000000' = offset from 'rip' (+- 2Gb)
};

struct JMP_absolute_t {
    const uint8_t OP_0  = 0xFF;
    const uint8_t OP_1  = 0x25; // '0xFF' '0x25'    = jmp ptr qword
    uint32_t OPERAND_0  = 0x00; // '0x00000000'     = offset to JMP address / absolute address in 32 bit mode

    #ifdef ARCH_x86_64
    uint64_t OPERAND_1  = 0x00; // '0xAddress'      = address to jump too, only used in 64 bit mode
    #endif
};
#pragma pack(pop)

// Correct OP code and OPERANDS for JMP template
constexpr JMP_relative_8_t relative_8_jmp   = {};
constexpr JMP_relative_t relative_jmp       = {};
constexpr JMP_absolute_t absolute_jmp       = {};

// Largest instruction is 15 (0xF) bytes long
constexpr uint8_t LONGEST_INSTRUCTION_SIZE  = 0x0F;

// Worst case size of our JMP instructions!
constexpr uint8_t CODE_CAVE_SIZE = LONGEST_INSTRUCTION_SIZE
    + sizeof(JMP_relative_t)    // patched bytes
    + sizeof(JMP_absolute_t)    // JMP to hooked function
    + sizeof(JMP_absolute_t);   // JMP to rest of original function

enum class JMP_type {
    RELATIVE,
    ABSOLUTE
};

[[nodiscard]] JMP_type find_jmp_type(
    const lib::memory::address& from,
    const lib::memory::address& target
) {
    const auto min_address = std::min<uintptr_t>(
        0,
        from.raw - std::numeric_limits<int32_t>::min()
    );

    const auto max_address = std::max<uintptr_t>(
        std::numeric_limits<uintptr_t>::max(),
        from.raw + std::numeric_limits<int32_t>::max()
    );

    // If we can reach our target then use relative, otherwise use absolute
    if (target.raw >= min_address ||
        target.raw <= max_address)
    {
        return JMP_type::RELATIVE;
    }

    return JMP_type::ABSOLUTE;
}
}

std::optional<lib::memory::address> trampoline::find_code_cave(std::optional<lib::memory::address>& function_address) {

}

bool trampoline::is_function_hooked(const lib::memory::address &function_address) {
    // Note: this also assumes we aren't directly doing an absolute JMP to our hooked function
    // If first OP code is relative JMP OP code then we can assume
    // that the function is hooked already since no call convention will ever produce a signature like this.
    const JMP_relative_t* relative_jmp_cast = function_address.ptr<JMP_relative_t>();

    if (relative_jmp_cast->OP_0 == relative_jmp.OP_0)
    {
        return true;
    }

    return false;
}

size_t trampoline::follow_jumps(lib::memory::address& function_address, size_t max_jumps) {
    return 0;
}

std::vector<uint8_t> trampoline::get_patched_bytes(const lib::memory::address& function_address) {
    std::vector<uint8_t> patched_bytes = {};
    return patched_bytes;
}

std::vector<uint8_t> trampoline::create_trampoline(
    const std::vector<uint8_t> &patched_bytes,
    const lib::memory::address &trampoline_address,
    const lib::memory::address &function_address
) {

}

std::vector<uint8_t> trampoline::create_jmp_proxy(
    const lib::memory::address &proxy_address,
    const lib::memory::address &hooked_function_address
) {

}

std::vector<uint8_t> trampoline::create_jmp_patch(
    const lib::memory::address &function_address,
    const lib::memory::address &proxy_address
) {

}

#endif

