#pragma once

#include <unistd.h>
#include <sys/mman.h>

#include "memory_null.hpp"

namespace lib::system::impl {
namespace backend::memory {
//! linux implementation
struct linux : null {};
}

namespace detail::memory {
//! Get mmap flags from 'VmProtectionType' enum
[[nodiscard]] constexpr int vm_protection_flags_linux(const VmProtectionType protection_type) {
    switch (protection_type) {
        case VmProtectionType::READ:
            return PROT_READ;
        case VmProtectionType::READ_WRITE:
            return PROT_READ | PROT_WRITE;
        case VmProtectionType::EXECUTE:
            return PROT_EXEC;
        case VmProtectionType::EXECUTE_READ:
            return PROT_EXEC | PROT_READ;
        case VmProtectionType::EXECUTE_READ_WRITE:
            return PROT_EXEC | PROT_READ | PROT_WRITE;
        default:
            break;
    }
    return PROT_READ;
}
}

//! linux specialization of memory
template<>
struct virtual_memory_base<backend::memory::linux> {
    [[nodiscard]] static uint64_t vm_page_size() {
        return sysconf(_SC_PAGESIZE);
    }

    [[nodiscard]] static std::optional<memory::memory_section> vm_allocate(
        const size_t size_bytes,
        const VmProtectionType protection_type = VmProtectionType::READ_WRITE
    ) {
        if (size_bytes <= 0) {
            return std::nullopt;
        }

        const int flags = detail::memory::vm_protection_flags_linux(protection_type);
        void* address = mmap(nullptr, size_bytes, flags, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

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

        return munmap(
            memory.base().ptr<void>(),
            memory.size<>()
        ) == 0;
    }

    [[nodiscard]] static bool vm_commit(const memory::memory_section& memory) {
        if (memory.base().ptr<void>() == nullptr ||
            memory.size() <= 0) {
            return false;
        }

        // Linux commits on first write :D
        return true;
    }

    [[nodiscard]] static bool vm_uncommit(const memory::memory_section& memory) {
        if (memory.base().ptr<void>() == nullptr ||
            memory.size() <= 0) {
            return false;
        }

        return madvise(
            memory.base().ptr<void>(),
            memory.size(),
            MADV_DONTNEED
        ) == 0;
    }

    [[nodiscard]] static bool vm_protect(
        const memory::memory_section& memory,
        const VmProtectionType protection_type
    ) {
        if (memory.base().ptr<void>() == nullptr ||
            memory.size() <= 0) {
            return false;
        }

        return mprotect(
            memory.base().ptr<void>(),
            memory.size(),
            detail::memory::vm_protection_flags_linux(protection_type)
        ) == 0;
    }

    [[nodiscard]] static bool vm_lock(const memory::memory_section& memory) {
        if (memory.base().ptr<void>() == nullptr ||
            memory.size() <= 0) {
            return false;
        }

        return mlock(
            memory.base().ptr<void>(),
            memory.size()
        ) == 0;
    }

    [[nodiscard]] static bool vm_unlock(const memory::memory_section& memory) {
        if (memory.base().ptr<void>() == nullptr ||
            memory.size() <= 0) {
            return false;
        }

        return munlock(
            memory.base().ptr<void>(),
            memory.size()
        ) == 0;
    }
};
}
