#pragma once

#include <expected>
#include <filesystem>
#include <string>

namespace lib::system {

    //! Get full path to running executable.
    //! \returns Full path to current executable
    [[nodiscard]] std::expected<std::filesystem::path, std::string> GetExecutablePath();
}