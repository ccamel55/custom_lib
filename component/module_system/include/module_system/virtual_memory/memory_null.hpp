#pragma once

#include <cstdint>
#include <optional>

#include <module_memory/type/address.hpp>
#include <module_memory/type/memory_section.hpp>

namespace lib::system {
enum class VmProtectionType {
    READ,
    READ_WRITE,
    EXECUTE,
    EXECUTE_READ,
    EXECUTE_READ_WRITE
};

namespace impl {
namespace backend::memory {
//! Default implementation
struct null {};
}

//! Base memory type - will be specialised on for more functionality
template<typename Impl = backend::memory::null>
struct virtual_memory_base {};
}
}