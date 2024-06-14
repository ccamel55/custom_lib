#pragma once

namespace lib {
class NoCopy {
public:
    // Copy constructors have been deleted. BAD
    NoCopy(const NoCopy&)  = delete;
    NoCopy(const NoCopy&&) = delete;

    void operator=(const NoCopy&)  = delete;
    void operator=(const NoCopy&&) = delete;

protected:
    NoCopy()  = default;
    ~NoCopy() = default;

};
} // namespace lib
