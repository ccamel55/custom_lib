#include <module_system/VirtualMemory.hpp>

#include <exception>

namespace
{
std::optional<uint32_t> virtual_page_size = std::nullopt;
}

#ifdef PLATFORM_WIN32

#include <Windows.h>

using namespace lib::system;

namespace
{
[[nodiscard]] DWORD get_flags_from_type(ProtectionType type) {
    switch (type) {
        case ProtectionType::READ:
            return PAGE_READONLY;
        case ProtectionType::READ_WRITE:
            return PAGE_READWRITE;
        case ProtectionType::EXECUTE:
            return PAGE_EXECUTE;
        case ProtectionType::EXECUTE_READ:
            return PAGE_EXECUTE_READ;
        case ProtectionType::EXECUTE_READ_WRITE:
            return PAGE_EXECUTE_READWRITE;
    }
}
}

uint32_t lib::system::detail::vm_get_page_size() {
    if (!virtual_page_size.has_value()) {
        // Fetch from WINAPI
        SYSTEM_INFO system_info = {};
        GetSystemInfo(&system_info);

        virtual_page_size = system_info.dwPageSize;
    }

    return virtual_page_size.value();
}

VirtualMemory::VirtualMemory(size_t size_bytes)
    : _virtual_memory({}) {

    if (size_bytes <= 0) {
        throw std::exception("size_bytes must be larger than 0");
    }

    const DWORD flags = get_flags_from_type(ProtectionType::READ_WRITE);
    LPVOID address = VirtualAlloc(nullptr, size_bytes, MEM_RESERVE, flags);

    if (!address) {
        throw std::exception("Could not allocated virtual memory");
    }

    _virtual_memory = std::span {
        reinterpret_cast<std::byte*>(address),
        size_bytes
    };
}

VirtualMemory::~VirtualMemory() {
    VirtualFree(_virtual_memory.base().ptr<void>(), 0, MEM_RELEASE);
}

bool VirtualMemory::commit(lib::system::ProtectionType type) const {
    const DWORD flags = get_flags_from_type(type);
    LPVOID result = VirtualAlloc(
        _virtual_memory.base().ptr<void>(),
        _virtual_memory.size(),
        MEM_COMMIT,
        flags
    );

    return result != nullptr;
}

bool VirtualMemory::uncommit() const {
    const bool result = VirtualFree(
        _virtual_memory.base().ptr<void>(),
        _virtual_memory.size(),
        MEM_DECOMMIT
    );

    return result;
}

VirtualProtection::VirtualProtection(
    const lib::memory::memory_section &section,
    lib::system::ProtectionType type,
    bool restore_previous_protection
)   : _section(section)
    , _restore_previous_protection(restore_previous_protection)
    , _previous_protection(0) {

    DWORD previous_flags = 0;
    const DWORD flags = get_flags_from_type(type);

    const bool result = VirtualProtect(
        _section.base().ptr<void>(),
        _section.size(),
        flags,
        &previous_flags
    );

    if (!result) {
        throw std::exception("Could not apply virtual memory protection");
    }

    _previous_protection = static_cast<uint32_t>(previous_flags);
}

VirtualProtection::~VirtualProtection() {
    if (!_restore_previous_protection) {
        return;
    }

    [[maybe_unused]] DWORD previous_flags = 0;
    [[maybe_unused]] const bool result = VirtualProtect(
        _section.base().ptr<void>(),
        _section.size(),
        static_cast<DWORD>(_previous_protection),
        &previous_flags
    );
}
#elifdef PLATFORM_LINUX

#include <sys/mman.h>

using namespace lib::system;

namespace
{
[[nodiscard]] int get_flags_from_type(ProtectionType type) {
    switch (type) {
        case ProtectionType::READ:
            return PROT_READ;
        case ProtectionType::READ_WRITE:
            return PROT_READ | PROT_WRITE;
        case ProtectionType::EXECUTE:
            return PROT_EXEC;
        case ProtectionType::EXECUTE_READ:
            return PROT_EXEC | PROT_READ;
        case ProtectionType::EXECUTE_READ_WRITE:
            return PROT_EXEC | PROT_READ | PROT_WRITE;
    }
}
}

uint32_t lib::system::detail::vm_get_page_size() {
    if (!virtual_page_size.has_value()) {
        virtual_page_size = sysconf(_SC_PAGESIZE);
    }

    return virtual_page_size.value();
}

// Todo lazy fucker
VirtualMemory::VirtualMemory(size_t size_bytes)
    : _virtual_memory({}) {
}

VirtualMemory::~VirtualMemory() {
}

bool VirtualMemory::commit(lib::system::ProtectionType type) const {
    return false;
}

bool VirtualMemory::uncommit() const {
    return false;
}

VirtualProtection::VirtualProtection(
    const lib::memory::memory_section &section,
    lib::system::ProtectionType type,
    bool restore_previous_protection
)   : _section(section)
    , _restore_previous_protection(restore_previous_protection)
    , _previous_protection(0) {
}

VirtualProtection::~VirtualProtection() {
}


#endif