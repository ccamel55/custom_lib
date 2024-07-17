#include <module_hook/InlineHook.hpp>

#include <limits>
#include <span>

#include <dep_capstone/capstone.hpp>
#include <dep_fmt/fmt.hpp>

/*
 * Hook strategy for x86
 *  - check if we can do relative jump (+- 2Gb)
 *  - if not, perform absolute JMP
 *
 * Hook strategy for x86_64
 * - perform absolute JMP
 */

// Todo: 32 bit support need to be tested and refined
// Note: also want to do some more sanity checks and deal with edge cases gracefully.
// Note: abstract away windows specific stuff we can create impls for linux in later
#include <Windows.h>

namespace
{
// No alignment on these bad boys!
// https://www.felixcloutier.com/x86/jmp
#pragma pack(push, 1)
#if defined(_M_X64) || defined(__x86_64__)
// X64 uses 0xFF 0x25 0x00000000 [ADDRESS:64]
// 64 bit indirect absolute JMP
struct JMP_X64 {
    uint8_t OP_0;       // 0xFF
    uint8_t OP_1;       // 0x25
    uint32_t OPERAND_0; // 0x00000000
    uint64_t OPERAND_1; // 0x0000000000000000
};
constexpr size_t JMP_SIZE       = sizeof(JMP_X64);
constexpr size_t BYTES_TO_LOAD  = JMP_SIZE + 0x0F;

// indirect absolute JMP to memory addresss
using jmp_operand_t = uint64_t;

#define JMP(address) JMP_X64 {  \
0xFF, 0x25,                     \
0x00000000,                     \
address                         \
}
#else
// X86 uses 0xE9 [OFFSET:32]
// 32 bit relative JMP
struct JMP_X32 {
   uint8_t OP_0;        // 0xE9
   uint32_t OPERAND_0;  // 0x00000000
};
constexpr size_t JMP_SIZE       = sizeof(JMP_X32);
constexpr size_t BYTES_TO_LOAD  = JMP_SIZE + 0x0F;

// relative JMP with 32bit offset
using jmp_operand_t = int32_t;

#define JMP(address) JMP_X32 {  \
0xE9,                           \
address                         \
}
#endif
#pragma pack(pop)

std::vector<uint8_t> dirty_bytes(lib::memory::address original_function) {
    // Todo: check that our function is at least JMP instruction in size.
    csh handle = {};
    cs_insn* instance = nullptr;

    if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK) {
        return {};
    }

    if (cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON) != CS_ERR_OK) {
        cs_close(&handle);
        return {};
    }

    size_t offset = 0;
    const size_t instruction_count = cs_disasm(
        handle,
        original_function.ptr<uint8_t>(),
        BYTES_TO_LOAD,
        original_function.raw,
        0,
        &instance
    );

    bool found_rip = false;
    for (size_t i = 0; i < instruction_count; i++) {
        const cs_insn& instruction = instance[i];
        offset += instruction.size;

        // If we come across an instruction that uses "rip" the nope the fuck
        // out of there coz we gonna break shit if we patch it.
        // todo: implement fix for "rip" instructions
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

    cs_free(instance, instruction_count);
    cs_close(&handle);

    if (found_rip) {
        return {};
    }

    std::span<uint8_t> instructions(original_function.ptr<uint8_t>(), offset);
    std::vector<uint8_t> dirty_bytes(instructions.begin(), instructions.end());

    return dirty_bytes;
}

std::vector<uint8_t> generate_instruction_patch(
    uint8_t dirty_byte_size,
    jmp_operand_t jmp_operand
) {
    // Write JMP instruction into register
    const auto jmp = JMP(jmp_operand);
    const auto jmp_asm_ptr = reinterpret_cast<const uint8_t*>(&jmp);

    std::vector<uint8_t> patch_bytes(JMP_SIZE, 0x0);
    std::copy(jmp_asm_ptr, jmp_asm_ptr + JMP_SIZE, patch_bytes.begin());

    // if overwritten zone has partially complete instruction, fill with NOP
    std::vector<uint8_t> nop(dirty_byte_size - JMP_SIZE, 0x90);
    patch_bytes.insert(patch_bytes.end(), nop.begin(), nop.end());

    return patch_bytes;
}

std::vector<uint8_t> generate_trampoline_asm(
    const std::vector<uint8_t>& patch,
    jmp_operand_t jmp_operand
) {
    const auto jmp = JMP(jmp_operand);
    const auto jmp_asm_ptr = reinterpret_cast<const uint8_t*>(&jmp);

    std::vector<uint8_t> trampoline_patch(patch);
    trampoline_patch.insert(trampoline_patch.end(), jmp_asm_ptr, jmp_asm_ptr + JMP_SIZE);

    return trampoline_patch;
}

// Todo: implement virtual page interface for platform independent page alloc/management
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

bool memory_shit(
    lib::memory::address address,
    size_t size,
    DWORD proc_flags,
    DWORD& original_flags
) {
    return VirtualProtect(address.cast<LPVOID >(), size, proc_flags, &original_flags);
}
}

// https://malwaretech.com/2015/01/inline-hooking-for-programmers-part-1.html
using namespace lib::hook;

bool InlineHook::emplace(
    lib::memory::address original_function,
    lib::memory::address hooked_function,
    lib::memory::address &trampoline_function
) {
    if (!is_address_executable(original_function) ||
        !is_address_executable(hooked_function)) {
        return false;
    }

    // On 64-bit systems we will use indirect JMP so there is no need
    // to check address offset
    #if defined(_M_X64) || defined(__x86_64__)
    const auto hooked_function_jmp = hooked_function.raw;
    #else
    // We will patch in a "jmp rel32" instruction which means we can do
    // at most (original_function + 1 + 2,147,483,647) bytes away or
    // at most (original_function + 1 - 2,147,483,648) bytes behind.
    const auto min_hooked_address = original_function.raw + 1 + std::numeric_limits<int32_t>::min();
    const auto max_hooked_address = original_function.raw + 1 + std::numeric_limits<int32_t>::max();

    if (hooked_function.raw < min_hooked_address ||
        hooked_function.raw > max_hooked_address) {
        return false;
    }

    const auto hooked_function_jmp = static_cast<int32_t>(hooked_function.raw - original_function.raw);
    #endif

    // 1) Find out how many bytes we need to replace
    const auto bytes = dirty_bytes(original_function);
    fmt::print("{:x}\n", fmt::join(bytes, " "));

    if (bytes.empty()) {
        return false;
    }

    // 2) Find out how many NOPs we need to insert
    const auto patch = generate_instruction_patch(bytes.size(), hooked_function_jmp);
    fmt::print("{:x}\n", fmt::join(patch, " "));

    if (patch.empty()) {
        return false;
    }

    // TODO: REMOVE ME
    {
        DWORD original_flags = 0;
        memory_shit(
            original_function,
            patch.size(),
            PAGE_EXECUTE_READWRITE,
            original_flags
        );

        std::copy(patch.begin(), patch.end(), original_function.ptr<uint8_t>());

        memory_shit(
            original_function,
            patch.size(),
            original_flags,
            original_flags
        );
    }

    // 3) Generate trampoline function to call original
    // note: need to deal with [rip] instructions if we overwrite, should be very unlikely.

    // TODO: REMOVE ME
    {
        // Alloc an executabl page for our trampoline :)
        // todo: write into same page if it's free, allocating a page per function is fucking stupid
        SYSTEM_INFO system_info = {};
        GetSystemInfo(&system_info);

        const auto page_size = system_info.dwPageSize;
        const auto page_buffer = static_cast<uint8_t*>(VirtualAlloc(nullptr, page_size, MEM_COMMIT, PAGE_READWRITE));

        trampoline_function = page_buffer;

        // todo: clean this shit up, we're just leaking memory like crazy RN
        //VirtualFree(page_buffer, 0, MEM_RELEASE);
    }

    #if defined(_M_X64) || defined(__x86_64__)
    const auto original_jmp_address = original_function.raw + JMP_SIZE;
    const auto trampoline_function_jmp = original_jmp_address;
    #else
    const auto original_jmp_address = original_function.raw + JMP_SIZE;
    const auto trampoline_function_jmp = static_cast<int32_t>(original_jmp_address - trampoline_function.raw);

    // We will patch in a "jmp rel32" instruction which means we can do
    // at most (original_jmp_address + 1 + 2,147,483,647) bytes away or
    // at most (original_jmp_address + 1 - 2,147,483,648) bytes behind.
    const auto min_trampoline_address = trampoline_function.raw + 1 + std::numeric_limits<int32_t>::min();
    const auto max_trampoline_address = trampoline_function.raw + 1 + std::numeric_limits<int32_t>::max();

    if (original_jmp_address min_trampoline_address ||
        original_jmp_address > max_trampoline_address) {
        return false;
    }
    #endif

    const auto trampoline_patch = generate_trampoline_asm(bytes, trampoline_function_jmp);
    fmt::print("{:x}\n", fmt::join(trampoline_patch, " "));

    if (trampoline_patch.empty()) {
        return false;
    }

    // TODO: REMOVE ME
    {
        // Write into executable region
        std::copy(trampoline_patch.begin(), trampoline_patch.end(), trampoline_function.ptr<uint8_t>());

        // mark the memory as executable
        DWORD original_flags;
        memory_shit(trampoline_function, trampoline_patch.size(), PAGE_EXECUTE_READ, original_flags);
    }

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