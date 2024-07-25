#pragma once

#include <cstdint>
#include <optional>

#include <module_system/system.hpp>

namespace lib::system
{
//! Memory protection types that can be applied.
enum class ProtectionType {
    READ,
    READ_WRITE,
    EXECUTE,
    EXECUTE_READ,
    EXECUTE_READ_WRITE
};

//! Get the current page size in bytes
//! \return page size in bytes
[[nodiscard]] uint32_t vm_get_page_size();

//! Allocate some virtual memory
//! \param size size of virtual memory allocation in bytes
//! \return std::nullopt if alloc failed, otherwise memory section representing allocated memory
std::optional<memory::memory_section> vm_allocate(size_t size);

//! Allocate some virtual memory with a specific protection type
//! Allocate some virtual memory
//! \param size size of virtual memory allocation in bytes
//! \param protection_type memory protection type
//! \return std::nullopt if alloc failed, otherwise memory section representing allocated memory
std::optional<memory::memory_section> vm_allocate(size_t size, ProtectionType protection_type);

//! Deallocate virtual memory.
//! \param memory_section section of memory to deallocate.
//! \return true if success false otherwise
bool vm_deallocate(const memory::memory_section& memory_section);

//! Commit virtual memory into physical memory
//! \param memory_section section of memory to commit.
//! \return true if success false otherwise
bool vm_commit(const memory::memory_section& memory_section);

//! Uncommit virtual memory into physical memory
//! \param memory_section section of memory to uncommit.
//! \return true if success false otherwise
bool vm_uncommit(const memory::memory_section& memory_section);

//! Apply a specific protection type to virtual memory.
//! \param memory_section section of memory to apply protection type too.
//! \param protection_type protection type to apply.
//! \return true if protection could be applied false otherwise
bool vm_protect(const memory::memory_section& memory_section, ProtectionType protection_type);

//! Lock a section of virtual memory into physical memory.
//! \param memory_section section of memory to lock.
//! \return true if success false otherwise
bool vm_lock(const memory::memory_section& memory_section);

//! Unlock a section of virtual memory from physical memory, allowing the system to swap it out if needed.
//! \param memory_section section of memory to unlock.
//! \return true if success false otherwise
bool vm_unlock(const memory::memory_section& memory_section);
}
