#include <module_system/filesystem.hpp>

#include <array>

using namespace lib::system;

#ifdef CAMEL_PLATFORM_WINDOWS
#include <Windows.h>
#else
#include <climits>
#include <unistd.h>
#endif

std::expected<std::filesystem::path, std::string> lib::system::get_executable_path() {

#ifdef CAMEL_PLATFORM_WINDOWS
    char path_buffer[MAX_PATH];
    if (GetModuleFileNameA(nullptr, &path_buffer[0], std::size(path_buffer)) == 0) {
        return std::unexpected("Could not get executable path");
    }
#else
    char path_buffer[PATH_MAX] = {};

    // If we don't have a path then we can say that our working directory is the current working directory
    if (readlink("/proc/self/exe", path_buffer, std::size(path_buffer)) <= 0) {
        if (getcwd(path_buffer, std::size(path_buffer)) == nullptr) {
            return std::unexpected("Could not get executable path");
        }
    }
#endif

    return std::filesystem::path(path_buffer);
}

std::vector<char> lib::system::read_file_as_bytes(const std::filesystem::path& path) {

    std::ifstream file_stream;
    file_stream.open(path, std::ios::binary);

    if (file_stream.fail()) {
        return {};
    }

    std::vector<char> result;

    constexpr size_t CHUNK_SIZE = 1024;
    std::array<char, CHUNK_SIZE> chunk = {};

    while (true) {
        const size_t read_size = file_stream.readsome(chunk.data(), CHUNK_SIZE);

        if (read_size <= 0) {
            break;
        }

        result.insert(
            result.end(),
            chunk.begin(),
            chunk.begin() + read_size
        );
    }

    return result;
}
