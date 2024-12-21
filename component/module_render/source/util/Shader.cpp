#include <module_render/util/Shader.hpp>

#include <fstream>

using namespace lib::render;

namespace {
    [[nodiscard]] std::vector<char> read_file_as_bytes(const std::filesystem::path& path) {
        std::ifstream file_stream;
        file_stream.open(path, std::ios::binary);

        if (file_stream.fail()) {
            return {};
        }

        std::vector<char> result;

        constexpr size_t CHUNK_SIZE = 1024;
        std::array<char, CHUNK_SIZE> chunk;

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

    [[nodiscard]] bool is_path_valid(const std::filesystem::path& path) {
        return is_directory(path) == false
            && path.extension() == ".spv";
    }
}

ShaderFactory::ShaderFactory(
    const std::shared_ptr<logger::Logger>& logger,
    const nvrhi::DeviceHandle& device
)
    : _logger(logger)
    , _device(device) {

}

std::expected<nvrhi::ShaderHandle, std::string> ShaderFactory::create_shader(
    const std::filesystem::path& path,
    const nvrhi::ShaderType type
) const {
    if (!exists(path)) {
        return std::unexpected("Shader file does not exist: " + path.string());
    }

    const auto absolute_path = absolute(path);

    if (!is_path_valid(absolute_path)) {
       return std::unexpected("Shader file is not valid, ensure shader file has extension: '.spv'");
    }

    const auto bytes = read_file_as_bytes(absolute_path);

    if (bytes.empty()) {
        return std::unexpected("Failed to read shader file from disk");
    }

    nvrhi::ShaderDesc shader_description;
    {
        shader_description.shaderType = type;

        shader_description.entryName    = "main";
        shader_description.debugName    = path.filename();
    }

    return _device->createShader(shader_description, bytes.data(), bytes.size());
}

// size_t ShaderFactory::cache_size() const {
//     return std::accumulate(
//         _shader_cache.begin(),
//         _shader_cache.end(),
//         0,
//         [](const size_t a, const std::pair<std::filesystem::path, std::vector<uint8_t>>& b) {
//             return a + b.second.size();
//         }
//     );
// }

// void ShaderFactory::clear_cache() {
//     _shader_cache.clear();
// }

