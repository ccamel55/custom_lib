#include <module_render/util/ShaderFactory.hpp>
#include <module_system/filesystem.hpp>

#include <fstream>

using namespace lib::render;

namespace {
    [[nodiscard]] bool is_path_valid(const std::filesystem::path& path) {
        return is_directory(path) == false
            && path.extension() == ".spv";
    }
}

ShaderFactory::ShaderFactory(nvrhi::IDevice* device)
    : _device(device) {

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

    std::vector<char> shader_bytes;

    if (_cache.contains(absolute_path)) {
        shader_bytes = _cache.at(absolute_path);
    }
    else {
        shader_bytes = lib::system::read_file_as_bytes(absolute_path);

        if (shader_bytes.empty()) {
            return std::unexpected("Failed to read shader file from disk");
        }

        _cache[absolute_path] = shader_bytes;
    }

    nvrhi::ShaderDesc shader_description;
    {
        shader_description.shaderType = type;

        shader_description.entryName    = "main";
        shader_description.debugName    = path.filename();
    }

    return _device->createShader(shader_description, shader_bytes.data(), shader_bytes.size());
}

void ShaderFactory::clear_cache() {
    _cache.clear();
}
