#pragma once

#include <expected>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace lib::system {

    //! Get full path to running executable.
    //! \returns Full path to current executable
    [[nodiscard]] std::expected<std::filesystem::path, std::string> get_executable_path();

    //! Read file on disk as bytes
    //! \param path file path
    //! \returns file as vector of bytes
    [[nodiscard]] std::vector<char> read_file_as_bytes(const std::filesystem::path& path);
}