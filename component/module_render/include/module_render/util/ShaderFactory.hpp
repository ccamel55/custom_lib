#pragma once

#include <dep_nvrhi/nvrhi.hpp>
#include <module_core/NoCopy.hpp>

#include <filesystem>
#include <unordered_map>

namespace lib::render {
class ShaderFactory : public NoCopy {
public:
    explicit ShaderFactory(nvrhi::IDevice* device);

    //! Create shader from file on disk
    //! \param path path to shader file. must have extension type `.spv`
    //! \param type shader type
    //! \return handle to nvrhi shader
    [[nodiscard]] std::expected<nvrhi::ShaderHandle, std::string> create_shader(
        const std::filesystem::path& path,
        nvrhi::ShaderType type
    ) const;

    //! Clear in memory shader cache
    void clear_cache();

private:
    nvrhi::IDevice* _device;
    mutable std::unordered_map<std::filesystem::path, std::vector<char>> _cache;
};
}
