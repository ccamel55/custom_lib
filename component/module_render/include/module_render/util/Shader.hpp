#pragma once

#include <dep_nvrhi/nvrhi.hpp>
#include <module_logger/Logger.hpp>

#include <filesystem>

namespace lib::render {
class ShaderFactory {
public:
    ShaderFactory(
        const std::shared_ptr<logger::Logger>& logger,
        const nvrhi::DeviceHandle& device
    );

    //! Create shader from file on disk
    //! \param path path to shader file. must have extension type `.spv`
    //! \param type shader type
    //! \return handle to nvrhi shader
    [[nodiscard]] std::expected<nvrhi::ShaderHandle, std::string> create_shader(
        const std::filesystem::path& path,
        nvrhi::ShaderType type
    ) const;

private:
    std::shared_ptr<logger::Logger> _logger;
    nvrhi::DeviceHandle _device;

};
}
