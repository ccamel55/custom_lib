#pragma once

#include <dep_nvrhi/nvrhi.hpp>

#include <module_render/types/buffer_object.hpp>

namespace lib::render {

struct bindings_t {

    //! Create bindings_t
    //! \param device device pointer
    //! \param desc binding description
    //! \returns binding_t created from description
    [[nodiscard]] static std::expected<bindings_t, std::string> create(
        nvrhi::IDevice* device,
        const nvrhi::BindingSetDesc& desc
    ) {
        bindings_t result;

        if (!nvrhi::utils::CreateBindingSetAndLayout(device, nvrhi::ShaderType::All, 0, desc, result.layout, result.set)) {
            return std::unexpected("Could not create binding set and layout");
        }

        return result;
    }

    nvrhi::BindingLayoutHandle layout;
    nvrhi::BindingSetHandle set;
};

}