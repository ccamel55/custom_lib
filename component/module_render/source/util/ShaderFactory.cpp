#include <module_render/util/ShaderFactory.hpp>
#include <module_system/filesystem.hpp>

#include <fstream>
#include <utility>

using namespace lib::render;

ShaderFactory::ShaderFactory(nvrhi::IDevice* device, std::filesystem::path  shader_folder)
    : _device(device)
    , _shader_folder(std::move(shader_folder)) {

}

std::expected<nvrhi::ShaderHandle, std::string> ShaderFactory::create_shader(
    std::filesystem::path file,
    const nvrhi::ShaderType type,
    const std::string& entry,
    const std::vector<ShaderMake::ShaderConstant>& constants
) {
    file = full_shader_path(file, entry);

    if (!exists(file) || is_directory(file)) {
        return std::unexpected("Shader file does not exist: " + file.string());
    }

    if (constants.size() > detail::MAX_CONSTANTS) {
        return std::unexpected("Too many shader constants given, max constants allowed: " + std::to_string(detail::MAX_CONSTANTS));
    }

    const auto absolute_path = absolute(file);

    // This is retarded, but we need to copy into fixed sized array for the hash function
    // Note: fix this shit-ness and do it properly
    std::array<ShaderMake::ShaderConstant, detail::MAX_CONSTANTS> tmp_constants = {};
    std::copy_n(constants.begin(), std::min(detail::MAX_CONSTANTS, constants.size()), tmp_constants.begin());

    nvrhi::ShaderHandle& shader = _shader[detail::shader_key(absolute_path, entry, tmp_constants)];

    // Create shader if it doesn't exist
    if (!shader) {
        const std::vector<char> shader_bytes = system::read_file_as_bytes(absolute_path);
        if (shader_bytes.empty()) {
            return std::unexpected("Failed to read shader file from disk");
        }

        // Find shader permutation
        size_t permutation_size = 0;
        const void* permutation_bytes = nullptr;

        if (!FindPermutationInBlob(
            shader_bytes.data(),
            shader_bytes.size(),
            constants.data(),
            static_cast<uint32_t>(constants.size()),
            &permutation_bytes,
            &permutation_size
        )) {
            return std::unexpected("Failed to find permutation in shader bin");
        }

        nvrhi::ShaderDesc shader_description;
        {
            shader_description.shaderType = type;

            shader_description.entryName    = entry;
            shader_description.debugName    = file.filename().string();
        }
        shader = _device->createShader(shader_description, permutation_bytes, permutation_size);
    }

    return shader;
}

void ShaderFactory::clear_cache() {
    _shader.clear();
}

std::filesystem::path ShaderFactory::full_shader_path(
    const std::filesystem::path& path,
    const std::string& entry
) const {

    // Append relative to shader folder
    std::filesystem::path full_path = _shader_folder / path;

    // Append entry name to file path
    if (!entry.empty() && entry != "main") {
        full_path += "_" + entry;
    }

    return full_path.replace_extension(".bin");
}

