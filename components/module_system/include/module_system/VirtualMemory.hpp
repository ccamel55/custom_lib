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

// Internal use only...
namespace detail
{
//! Get the current page size in bytes
//! \return page size in bytes
[[nodiscard]] uint32_t vm_get_page_size();
}

//! RAII wrapper for dealing with virtual memory
class VirtualMemory {
public:
    //! \param size_bytes size of virtual memory in bytes
    explicit VirtualMemory(size_t size_bytes);
    ~VirtualMemory();

    //! Commit a section of virtual memory to physical memory.
    //! \param type protection type to be applied to virtual memory
    //! \return true if commit occurred, false otherwise.
    [[nodiscard]] bool commit(ProtectionType type) const;

    //! Uncommit a section of virtual memory from physical memory.
    //! \return true if uncommit occurred, false otherwise.
    [[nodiscard]] bool uncommit() const;

private:
    lib::memory::memory_section _virtual_memory;

};

//! RAII helper for dealing with custom page protection
class VirtualProtection {
public:
    //! \param section virtual memory section we want to alter protection for
    //! \param type type of protection
    //! \param restore_previous_protection whether or not to restore previous protection type on destruction
    VirtualProtection(
        const lib::memory::memory_section& section,
        ProtectionType type,
        bool restore_previous_protection = true
    );

    ~VirtualProtection();

private:
    lib::memory::memory_section _section;

    bool _restore_previous_protection   = true;
    uint32_t _previous_protection       = 0;

};
}
