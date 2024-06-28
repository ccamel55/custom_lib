#pragma once

#include <module_memory/type/address.hpp>
#include <module_memory/type/memory_section.hpp>

#include <Windows.h>

namespace lib::memory
{
//! Get the image DOS header from a win32 program.
//! \param address location in memory where the program is located
//! \return image dos header
[[nodiscard]] inline PIMAGE_DOS_HEADER get_dos_header(address address) {
    return address.cast<PIMAGE_DOS_HEADER>();
}

//! Get the image nt header from a win32 program
//! \param address location in memory where the program is located
//! \return image nt header
[[nodiscard]] inline PIMAGE_NT_HEADERS get_nt_header(address address) {
    const auto dos_header = get_dos_header(address);
    return address.offset(dos_header->e_lfanew).cast<PIMAGE_NT_HEADERS>();
}

//! Get a memory section representing a win32 program
//! \param address location in memory where program is located
//! \return memory section for the program
[[nodiscard]] inline memory_section get_memory_section(address address) {
    const auto nt_header = get_nt_header(address);
    const std::span<std::byte> span = { address.ptr<std::byte>(), nt_header->OptionalHeader.SizeOfImage };

    return memory_section(span);
}
}