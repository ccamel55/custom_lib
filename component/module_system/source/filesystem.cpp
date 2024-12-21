#include <module_system/filesystem.hpp>

using namespace lib::system;

#ifdef CAMEL_PLATFORM_WINDOWS
# error Implement for windows
// https://stackoverflow.com/questions/1528298/get-path-of-executable
#else
#include <climits>
#include <unistd.h>
#endif

std::expected<std::filesystem::path, std::string> lib::system::GetExecutablePath() {

#ifdef CAMEL_PLATFORM_WINDOWS

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
