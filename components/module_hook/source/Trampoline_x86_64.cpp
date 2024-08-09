#include <module_hook/Trampoline.hpp>

#ifdef ARCH_x86_64

#include <cassert>
#include <limits>
#include <span>

#include <dep_capstone/capstone.hpp>

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

enum class JMP_type {
    RELATIVE,
    ABSOLUTE
};

[[nodiscard]] JMP_type find_jmp_type(
    const lib::memory::address& from,
    const lib::memory::address& target
) {
    const auto rip = from.raw + sizeof(JMP_relative_t);

    const auto min_address = std::min<uintptr_t>(
        0,
        rip + std::numeric_limits<int32_t>::min()
    );

    const auto max_address = std::max<uintptr_t>(
        std::numeric_limits<uintptr_t>::max(),
        rip + std::numeric_limits<int32_t>::max()
    );

    // If we can reach our target then use relative, otherwise use absolute
    if (target.raw >= min_address && target.raw <= max_address) {
        return JMP_type::RELATIVE;
    }

    return JMP_type::ABSOLUTE;
}
}

constexpr uint8_t trampoline::minimum_code_cave_size() {
    return LONGEST_INSTRUCTION_SIZE
           + sizeof(JMP_relative_t)    // patched bytes
           + sizeof(JMP_absolute_t)    // JMP to hooked function
           + sizeof(JMP_absolute_t);   // JMP to rest of original function;
}

std::optional<lib::memory::address> trampoline::get_code_cave(
    std::optional<lib::memory::address>& function_address,
    size_t size_bytes
) {
    // Todo: implement
    return std::nullopt;
}

size_t trampoline::follow_jumps(lib::memory::address& function_address, size_t max_jumps) {
    size_t jumps = 0;

    for (; jumps < max_jumps; jumps++) {
        const uint8_t* function_address_ptr = function_address.ptr<uint8_t>();

        // Check for JMP OP codes and move to those addresses if they exist
        if (function_address_ptr[0] == relative_8_jmp.OP_0) {
            const JMP_relative_8_t* jmp_rel_8_ptr = function_address.ptr<JMP_relative_8_t>();
            function_address = function_address_ptr + sizeof(JMP_relative_8_t) + jmp_rel_8_ptr->OPERAND_0;
        }
        else if (function_address_ptr[0] == relative_jmp.OP_0) {
            const JMP_relative_t* jmp_rel_ptr = function_address.ptr<JMP_relative_t>();
            function_address = function_address_ptr + sizeof(JMP_relative_t) + jmp_rel_ptr->OPERAND_0;
        }
        else if (function_address_ptr[0] == absolute_jmp.OP_0 &&
                 function_address_ptr[1] == absolute_jmp.OP_1) {
            const JMP_absolute_t* jmp_abs_ptr = function_address.ptr<JMP_absolute_t>();

            #ifdef ARCH_x86_64
            // Absolute address is stored at ([RIP] + offset)
            lib::memory::address absolute_address(function_address_ptr + offsetof(JMP_absolute_t, OPERAND_1) + jmp_abs_ptr->OPERAND_0);
            function_address = absolute_address.get<uintptr_t>();
            #else
            // Absolute address is OPERAND_0
            function_address = jmp_abs_ptr->OPERAND_0;
            #endif
        }
        else {
            break;
        }
    }

    return jumps;
}

std::vector<uint8_t> trampoline::get_patched_bytes(const lib::memory::address& function_address) {
    std::vector<uint8_t> patched_bytes = {};

    csh handle = {};
    cs_insn* instance = nullptr;

    size_t offset = 0;
    size_t instruction_count = 0;

    const auto close = [&] {
        if (instance) {
            cs_free(instance, instruction_count);
        }
        cs_close(&handle);
    };

    if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK) {
        return patched_bytes;
    }

    if (cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON) != CS_ERR_OK) {
        close();
        return patched_bytes;
    }

    constexpr uint8_t JMP_SIZE = sizeof(JMP_relative_t);
    constexpr uint8_t BYTES_TO_LOAD = LONGEST_INSTRUCTION_SIZE + JMP_SIZE;

    instruction_count = cs_disasm(
        handle,
        function_address.ptr<uint8_t>(),
        BYTES_TO_LOAD,
        function_address.raw,
        0,
        &instance
    );

    bool found_rip = false;

    for (size_t i = 0; i < instruction_count; i++) {
        const cs_insn& instruction = instance[i];
        offset += instruction.size;

        // If we come across an instruction that uses "rip" the nope the fuck
        // out of there coz we gonna break shit if we patch it.
        // Todo: handle RIP later
        for (uint8_t j = 0; j < instruction.detail->groups_count; j++) {
            if (instruction.detail->groups[j] == X86_GRP_BRANCH_RELATIVE) {
                found_rip = true;
                break;
            }
        }

        for (uint8_t j = 0; j < instruction.detail->x86.op_count; j++) {
            const cs_x86_op& operand = instruction.detail->x86.operands[j];
            if (operand.type == X86_OP_MEM && operand.mem.base == X86_REG_RIP) {
                found_rip = true;
                break;
            }
        }

        if (found_rip) {
            break;
        }

        // Smallest number after/equal to JMP patch size
        if (offset >= JMP_SIZE) {
            break;
        }
    }

    close();

    // Todo: handle RIP later
    if (found_rip) {
        return patched_bytes;
    }

    // Copy over bytes include those that need to be NOPed later,
    // patched_bytes.size() >= JMP_SIZE
    const std::span<uint8_t> instructions(function_address.ptr<uint8_t>(), offset);
    patched_bytes = std::vector(instructions.begin(), instructions.end());

    return patched_bytes;
}

std::vector<uint8_t> trampoline::create_trampoline(
    const std::vector<uint8_t>& patched_bytes,
    const lib::memory::address& trampoline_address,
    const lib::memory::address& function_address
) {
    assert(patched_bytes.size() >= sizeof(JMP_relative_t));

    // copy over patched bytes first, then find jmp instruction address
    std::vector<uint8_t> trampoline(patched_bytes.begin(), patched_bytes.end());

    const lib::memory::address jump_address(trampoline_address.raw + patched_bytes.size());
    const auto jump_type = find_jmp_type(jump_address, function_address);

    switch (jump_type) {
        case JMP_type::RELATIVE:
        {
            const auto rip = jump_address.raw + sizeof(JMP_relative_t);
            const auto offset = static_cast<int32_t>(function_address.raw - rip);

            JMP_relative_t jmp_relative = {};
            jmp_relative.OPERAND_0 = offset;

            const auto jmp_relative_ptr = reinterpret_cast<const uint8_t*>(&jmp_relative);
            trampoline.insert(trampoline.end(), jmp_relative_ptr, jmp_relative_ptr + sizeof(jmp_relative_ptr));

            break;
        }
        case JMP_type::ABSOLUTE:
        {
            JMP_absolute_t jmp_absolute = {};

            #ifdef ARCH_x86_64
            jmp_absolute.OPERAND_0 = 0x00000000;
            jmp_absolute.OPERAND_1 = function_address.raw;
            #else
            jmp_absolute.OPERAND_0 = function_address.raw;
            #endif

            const auto jmp_absolute_ptr = reinterpret_cast<uint8_t*>(&jmp_absolute);
            trampoline.insert(trampoline.end(), jmp_absolute_ptr, jmp_absolute_ptr + sizeof(jmp_absolute));

            break;
        }
    }

    return trampoline;
}

std::vector<uint8_t> trampoline::create_jmp_proxy(
    const lib::memory::address& proxy_address,
    const lib::memory::address& hooked_function_address
) {
    // copy over patched bytes first, then find jmp instruction address
    std::vector<uint8_t> proxy = {};
    const auto jump_type = find_jmp_type(proxy_address, hooked_function_address);

    switch (jump_type) {
        case JMP_type::RELATIVE:
        {
            const auto rip = proxy_address.raw + sizeof(JMP_relative_t);
            const auto offset = static_cast<int32_t>(hooked_function_address.raw - rip);

            JMP_relative_t jmp_relative = {};
            jmp_relative.OPERAND_0 = offset;

            const auto jmp_relative_ptr = reinterpret_cast<const uint8_t*>(&jmp_relative);
            proxy.insert(proxy.end(), jmp_relative_ptr, jmp_relative_ptr + sizeof(jmp_relative_ptr));

            break;
        }
        case JMP_type::ABSOLUTE:
        {
            JMP_absolute_t jmp_absolute = {};

            #ifdef ARCH_x86_64
            jmp_absolute.OPERAND_0 = 0x00000000;
            jmp_absolute.OPERAND_1 = hooked_function_address.raw;
            #else
            jmp_absolute.OPERAND_0 = hooked_function_address.raw;
            #endif

            const auto jmp_absolute_ptr = reinterpret_cast<uint8_t*>(&jmp_absolute);
            proxy.insert(proxy.end(), jmp_absolute_ptr, jmp_absolute_ptr + sizeof(jmp_absolute));

            break;
        }
    }

    return proxy;
}

std::vector<uint8_t> trampoline::create_jmp_patch(
    const lib::memory::address& function_address,
    const lib::memory::address& proxy_address,
    uint8_t patch_size
) {
    // Check that we can jump to our proxy using relative jump
    std::vector<uint8_t> jump_patch = {};
    JMP_relative_t jmp_relative = {};

    assert(patch_size >= sizeof(jmp_relative));

    const auto jump_type = find_jmp_type(function_address, proxy_address);

    if (jump_type != JMP_type::RELATIVE) {
        return jump_patch;
    }

    const auto rip = function_address.raw + sizeof(jmp_relative);
    const auto offset = static_cast<int32_t>(proxy_address.raw - rip);

    jmp_relative.OPERAND_0 = offset;

    // Write as byte
    const uint8_t* jmp_relative_bytes = reinterpret_cast<uint8_t*>(&jmp_relative);
    jump_patch.insert(jump_patch.end(), jmp_relative_bytes, jmp_relative_bytes + sizeof(jmp_relative));

    // NOP any extra bytes/bytes left in instructions we overwrite
    std::vector<uint8_t> nop(patch_size - sizeof(jmp_relative), 0x90);
    jump_patch.insert(jump_patch.end(), nop.begin(), nop.end());

    return jump_patch;
}

#endif

