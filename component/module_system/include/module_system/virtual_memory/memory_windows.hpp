#pragma once

#include <cstdint>
#include <optional>

#include <Windows.h>

#include <module_memory/type/address.hpp>
#include <module_memory/type/memory_section.hpp>

#include "memory_null.hpp"

namespace lib::system::impl {
namespace backend::memory {
//! linux implementation
struct windows : null {};
}

namespace detail::memory {
//! Get flags from 'VmProtectionType' enum
[[nodiscard]] constexpr DWORD vm_protection_flags_win32(const VmProtectionType protection_type) {
    switch (protection_type) {
        case VmProtectionType::READ:
            return PAGE_READONLY;
        case VmProtectionType::READ_WRITE:
            return PAGE_READWRITE;
        case VmProtectionType::EXECUTE:
            return PAGE_EXECUTE;
        case VmProtectionType::EXECUTE_READ:
            return PAGE_EXECUTE_READ;
        case VmProtectionType::EXECUTE_READ_WRITE:
            return PAGE_EXECUTE_READWRITE;
    }
    return PAGE_READONLY;
}
}

//! windows specialization of memory
template<>
struct virtual_memory_base<backend::memory::windows> {
    [[nodiscard]] static uint64_t vm_page_size() {
        SYSTEM_INFO system_info = {};
        GetSystemInfo(&system_info);

        // todo: cache this shit
        return system_info.dwPageSize;
    }

    [[nodiscard]] static std::optional<memory::memory_section> vm_allocate(
        const size_t size_bytes,
        const VmProtectionType protection_type = VmProtectionType::READ_WRITE
    ) {
        if (size_bytes <= 0) {
            return std::nullopt;
        }

        const DWORD flags = detail::memory::vm_protection_flags_win32(protection_type);
        LPVOID address = VirtualAlloc(nullptr, size_bytes, MEM_RESERVE, flags);

        return memory::memory_section {
            static_cast<std::byte*>(address),
            size_bytes
        };
    }

    [[nodiscard]] static bool vm_deallocate(const memory::memory_section& memory) {
        if (memory.base().ptr<void>() == nullptr ||
            memory.size() <= 0) {
            return false;
        }

        return VirtualFree(memory.base().ptr<void>(), 0, MEM_RELEASE);
    }

    [[nodiscard]] static bool vm_commit(const memory::memory_section& memory) {
        if (memory.base().ptr<void>() == nullptr ||
            memory.size() <= 0) {
            return false;
        }

        return VirtualAlloc(
            memory.base().ptr<void>(),
            memory.size(),
            MEM_COMMIT,
            PAGE_READWRITE
        ) != nullptr;
    }

    [[nodiscard]] static bool vm_uncommit(const memory::memory_section& memory) {
        if (memory.base().ptr<void>() == nullptr ||
            memory.size() <= 0) {
            return false;
        }

        return VirtualFree(
            memory.base().ptr<void>(),
            memory.size(),
            MEM_DECOMMIT
        );
    }

    [[nodiscard]] static bool vm_protect(
        const memory::memory_section& memory,
        const VmProtectionType protection_type
    ) {
        if (memory.base().ptr<void>() == nullptr ||
            memory.size() <= 0) {
            return false;
        }

        DWORD old_protect = 0;
        const DWORD flags = detail::memory::vm_protection_flags_win32(protection_type);

        return VirtualProtect(
            memory.base().ptr<void>(),
            memory.size(),
            flags,
            &old_protect
        );
    }

    [[nodiscard]] static bool vm_lock(const memory::memory_section& memory) {
        if (memory.base().ptr<void>() == nullptr ||
            memory.size() <= 0) {
            return false;
        }

        return VirtualLock(
            memory.base().ptr<void>(),
            memory.size()
        );
    }

    [[nodiscard]] static bool vm_unlock(const memory::memory_section& memory) {
        if (memory.base().ptr<void>() == nullptr ||
            memory.size() <= 0) {
            return false;
        }

        return VirtualUnlock(
            memory.base().ptr<void>(),
            memory.size()
        );
    }
};
}
