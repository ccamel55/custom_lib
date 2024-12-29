#pragma once

#include <dep_nvrhi/nvrhi.hpp>
#include <dep_nvrhi/shadermake.hpp>

#include <module_core/NoCopy.hpp>

#include <filesystem>
#include <unordered_map>

namespace lib::render {
namespace detail {

// Arbitrary limit set by us, increment if needed
inline constexpr size_t MAX_CONSTANTS = 8;

struct shader_key {
    std::filesystem::path path;
    std::string entry;
    std::array<ShaderMake::ShaderConstant, MAX_CONSTANTS> constants;

    bool operator==(const shader_key& other) const {
        for (size_t i = 0; i < MAX_CONSTANTS; ++i) {
            if (constants[i].name == other.constants[i].name &&
                constants[i].value == other.constants[i].value) {
                continue;
            }
            if (constants[i].name == nullptr || other.constants[i].name == nullptr ||
                constants[i].value == nullptr || other.constants[i].value == nullptr) {
                return false;
            }
            if (std::strcmp(constants[i].name, other.constants[i].name) != 0 ||
                std::strcmp(constants[i].value, other.constants[i].value) != 0) {
                return false;
            }
        }
        return path == other.path
            && entry == other.entry;
    }

    bool operator!=(const shader_key& other) const {
        return !( *this == other );
    }

    struct hash  {
        size_t operator()(const shader_key& desc) const {
            size_t hash = 0;
            nvrhi::hash_combine(hash, desc.path);
            nvrhi::hash_combine(hash, desc.entry);
            for (size_t i = 0; i < MAX_CONSTANTS; ++i) {
                nvrhi::hash_combine(hash, desc.constants[i].name);
                nvrhi::hash_combine(hash, desc.constants[i].value);
            }
            return hash;
        }
    };
};

}

class ShaderFactory : public NoCopy {
public:
    explicit ShaderFactory(nvrhi::IDevice* device, std::filesystem::path shader_folder);

    //! Create shader from file on disk
    //! \param file path to shader file. must have extension type `.spv`
    //! \param type shader type
    //! \param entry entry point for shader
    //! \param constants shader constant to select from blob
    //! \return handle to nvrhi shader
    [[nodiscard]] std::expected<nvrhi::ShaderHandle, std::string> create_shader(
        std::filesystem::path file,
        nvrhi::ShaderType type,
        const std::string& entry = "main",
        const std::vector<ShaderMake::ShaderConstant>& constants = {}
    );

    //! Clear in memory shader cache
    void clear_cache();

private:
    [[nodiscard]] std::filesystem::path full_shader_path(
        const std::filesystem::path& path,
        const std::string& entry
    ) const;

private:
    nvrhi::IDevice* _device;
    std::filesystem::path _shader_folder;

    std::unordered_map<detail::shader_key, nvrhi::ShaderHandle, detail::shader_key::hash> _shader;
};
}
