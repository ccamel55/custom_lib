#include <module_render/util/ShaderFactory.hpp>
#include <module_system/filesystem.hpp>

#include <fstream>
#include <utility>

using namespace lib::render;

namespace {
    [[nodiscard]] bool is_path_valid(const std::filesystem::path& path) {
        return is_directory(path) == false
            && path.extension() == ".spv";
    }
}

ShaderFactory::ShaderFactory(nvrhi::IDevice* device, std::filesystem::path  shader_folder)
    : _device(device)
    , _shader_folder(std::move(shader_folder)) {

}

std::expected<nvrhi::ShaderHandle, std::string> ShaderFactory::create_shader(
    std::filesystem::path path,
    const nvrhi::ShaderType type
) const {

    path = _shader_folder / path;

    if (!exists(path)) {
        return std::unexpected("Shader file does not exist: " + path.string());
    }

    const auto absolute_path = absolute(path);

    if (!is_path_valid(absolute_path)) {
       return std::unexpected("Shader file is not valid, ensure shader file has extension: '.spv'");
    }

    if (_cache.contains(absolute_path)) {
        return _cache.at(absolute_path);
    }

    const std::vector<char> shader_bytes = system::read_file_as_bytes(absolute_path);

    if (shader_bytes.empty()) {
        return std::unexpected("Failed to read shader file from disk");
    }

    nvrhi::ShaderDesc shader_description;
    {
        shader_description.shaderType = type;

        shader_description.entryName    = "main";
        shader_description.debugName    = path.filename().string();
    }

    return _cache[absolute_path] = _device->createShader(shader_description, shader_bytes.data(), shader_bytes.size());
}

void ShaderFactory::clear_cache() {
    _cache.clear();
}
