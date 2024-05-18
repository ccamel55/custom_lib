#pragma once

namespace lib
{
class NoCopy {
public:
    // Copy constructors have been deleted. BAD
    NoCopy(const NoCopy&) = delete;
    NoCopy(const NoCopy&&) = delete;

    void operator=(const NoCopy&) = delete;
    void operator=(const NoCopy&&) = delete;

};
}  // namespace lib