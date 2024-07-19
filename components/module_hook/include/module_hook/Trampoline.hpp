#pragma once

#include <module_system/definitions.hpp>
#include <module_memory/type/address.hpp>

#include <optional>
#include <vector>

namespace lib::hook::trampoline
{

//! Get code cave size
//! \return minimum size of our code cave in bytes
[[nodiscard]] constexpr uint8_t minimum_code_cave_size();

//! Find a code cave where we can write some proxy/trampoline instructions
//! \param function_address address we want to find a code cave near
//! \param size_bytes minimum size of code cave in bytes
//! \return address of our code cave.
[[nodiscard]] std::optional<lib::memory::address> get_code_cave(
    std::optional<lib::memory::address>& function_address,
    size_t size_bytes
);

//! Check for existing hooks, if they exist follow them until we reach a "final" function
//! \param function_address address of original function, this will be updated as we follow jumps
//! \param max_jumps maximum number of jumps we want to follow
//! \return number of jumps we followed
[[nodiscard]] size_t follow_jumps(lib::memory::address& function_address, size_t max_jumps);

//! Find the bytes that will be overwritten by our jmp patch
//! \param function_address address of original function we will be patching
//! \return bytes that will we overwritten by our JMP patch
[[nodiscard]] std::vector<uint8_t> get_patched_bytes(const lib::memory::address& function_address);

//! Create trampoline that will call the original function
//! \param patched_bytes bytes that will be overwritten by our jmp patch
//! \param trampoline_address address where the trampoline will be placed
//! \param function_address address in original function after our patched bytes
//! \return machine that will call the original version of our hooked function
[[nodiscard]] std::vector<uint8_t> create_trampoline(
    const std::vector<uint8_t>& patched_bytes,
    const lib::memory::address& trampoline_address,
    const lib::memory::address& function_address
);

//! Create jmp proxy that will actually jump to our hooked function
//! \param proxy_address address where our jmp proxy is located
//! \param hooked_function_address address of our hooked function
//! \return machine code that will jump to our hooked function
[[nodiscard]] std::vector<uint8_t> create_jmp_proxy(
    const lib::memory::address& proxy_address,
    const lib::memory::address& hooked_function_address
);

//! Create function patch that we will write into our function
//! \param function_address address of original function we will be patching
//! \param proxy_address address where our jmp proxy is located
//! \param patch_size how many bytes of instructions we overwrite
//! \return machine code that will jump to our proxy
[[nodiscard]] std::vector<uint8_t> create_jmp_patch(
    const lib::memory::address& function_address,
    const lib::memory::address& proxy_address,
    uint8_t patch_size
);
}
