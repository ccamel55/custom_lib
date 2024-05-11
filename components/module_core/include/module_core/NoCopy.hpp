#pragma once

namespace lib
{
class NoCopy {
public:
    // Redundant, but makes clear that we can only create and destroy
    NoCopy() = default;
    ~NoCopy() = default;

    // Copy constructors have been deleted. BAD
    NoCopy(const NoCopy&) = delete;
    NoCopy(const NoCopy&&) = delete;

    void operator=(const NoCopy&) = delete;
    void operator=(const NoCopy&&) = delete;

};
}  // namespace lib