#pragma once

#include <module_core/macro/file_include.hpp>
#include LIB_INCLUDE(memory, CAMEL_PLATFORM)

namespace lib::system {
namespace concepts {
//! Concept that 'system' implementation MUST satisfy
template<typename T>
concept System = requires {
    { T::vm_page_size() }                                           -> std::same_as<uint64_t>;
    { T::vm_allocate(size_t{}, VmProtectionType{}) }                -> std::same_as<std::optional<memory::memory_section>>;
    { T::vm_deallocate(memory::memory_section{}) }                  -> std::same_as<bool>;
    { T::vm_commit(memory::memory_section{}) }                      -> std::same_as<bool>;
    { T::vm_uncommit(memory::memory_section{}) }                    -> std::same_as<bool>;
    { T::vm_protect(memory::memory_section{}, VmProtectionType{}) } -> std::same_as<bool>;
    { T::vm_lock(memory::memory_section{}) }                        -> std::same_as<bool>;
    { T::vm_unlock(memory::memory_section{}) }                      -> std::same_as<bool>;
};
}

// Alias correct version and make sure concept is satisfied.
using memory = impl::virtual_memory_base<impl::backend::memory::CAMEL_PLATFORM>;
static_assert(concepts::System<memory>, "'system' does not satisfy concept");
}