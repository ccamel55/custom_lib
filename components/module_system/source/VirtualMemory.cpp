#include <module_system/VirtualMemory.hpp>

#include <exception>

namespace
{
std::optional<uint32_t> virtual_page_size = std::nullopt;
}

#if defined(PLATFORM_WIN32)

#include <Windows.h>

#include <cassert>

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

    assert(size_bytes > 0);

    const DWORD flags = get_flags_from_type(ProtectionType::READ_WRITE);
    LPVOID address = VirtualAlloc(nullptr, size_bytes, MEM_RESERVE, flags);

    assert(address);

    _virtual_memory = std::span {
        reinterpret_cast<std::byte*>(address),
        size_bytes
    };
}

VirtualMemory::~VirtualMemory() {
    const bool result = VirtualFree(_virtual_memory.base().ptr<void>(), 0, MEM_RELEASE);
    assert(result);
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
#elif defined(PLATFORM_LINUX)

#include <unistd.h>
#include <sys/mman.h>

using namespace lib::system;

namespace
{
[[nodiscard]] int get_flags_from_type(const ProtectionType type) {
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
        default:
            break;
    }
    return PROT_READ;
}
}

uint32_t lib::system::vm_get_page_size() {
    if (!virtual_page_size.has_value()) {
        virtual_page_size = sysconf(_SC_PAGESIZE);
    }
    return virtual_page_size.value();
}

std::optional<lib::memory::memory_section> lib::system::vm_allocate(const size_t size) {
    if (size <= 0) {
        return std::nullopt;
    }

    const int flags = get_flags_from_type(ProtectionType::READ_WRITE);
    void* address = mmap(nullptr, size, flags, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    return std::span {
        static_cast<std::byte*>(address),
        size
    };
}

bool lib::system::vm_deallocate(const memory::memory_section& memory_section) {
    if (memory_section.base().ptr<void>() == nullptr ||
        memory_section.size() <= 0) {
        return false;
    }

    return munmap(
        memory_section.base().ptr<void>(),
        memory_section.size()
    ) == 0;
}

bool lib::system::vm_commit([[maybe_unused]] const memory::memory_section& memory_section) {
    if (memory_section.base().ptr<void>() == nullptr ||
        memory_section.size() <= 0) {
        return false;
    }

    // Linux commits on first write :D
    return true;
}

bool lib::system::vm_uncommit(const memory::memory_section& memory_section) {
    if (memory_section.base().ptr<void>() == nullptr ||
        memory_section.size() <= 0) {
        return false;
    }

    return madvise(
        memory_section.base().ptr<void>(),
        memory_section.size(),
        MADV_DONTNEED
    ) == 0;
}

bool lib::system::vm_protect(
    const memory::memory_section& memory_section,
    const ProtectionType protection_type
) {
    if (memory_section.base().ptr<void>() == nullptr ||
        memory_section.size() <= 0) {
        return false;
    }

    return mprotect(
        memory_section.base().ptr<void>(),
        memory_section.size(),
        get_flags_from_type(protection_type)
    ) == 0;
}

bool lib::system::vm_lock(const memory::memory_section& memory_section) {
    if (memory_section.base().ptr<void>() == nullptr ||
        memory_section.size() <= 0) {
        return false;
    }

    return mlock(
        memory_section.base().ptr<void>(),
        memory_section.size()
    ) == 0;
}

bool lib::system::vm_unlock(const memory::memory_section& memory_section) {
    if (memory_section.base().ptr<void>() == nullptr ||
        memory_section.size() <= 0) {
        return false;
    }

    return munlock(
        memory_section.base().ptr<void>(),
        memory_section.size()
    ) == 0;
}

#endif