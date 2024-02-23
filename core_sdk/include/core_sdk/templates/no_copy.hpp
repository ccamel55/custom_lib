#pragma once

namespace lib
{
class no_copy
{
public:
    // Redundent, but makes clear that we can only create and destroy
    no_copy() = default;
    ~no_copy() = default;

private:
    // Copy constructors have been deleted. BAD
    no_copy(const no_copy&) = delete;
    no_copy(const no_copy&&) = delete;

    void operator=(const no_copy&) = delete;
    void operator=(const no_copy&&) = delete;

};
}  // namespace lib