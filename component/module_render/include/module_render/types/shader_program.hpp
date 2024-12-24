#pragma once

#include <dep_nvrhi/nvrhi.hpp>

namespace lib::render {

struct shader_program_t {

    //! Create shader program
    //! \param device nvrhi device pointer
    //! \param vertex_shader vertex shader
    //! \param pixel_shader pixel shader
    //! \param vertex_attributes vertex layout descriptions
    template<size_t n>
    [[nodiscard]] static shader_program_t create(
        nvrhi::IDevice* device,
        nvrhi::ShaderHandle vertex_shader,
        nvrhi::ShaderHandle pixel_shader,
        const std::array<nvrhi::VertexAttributeDesc, n>& vertex_attributes
    ) {
        shader_program_t result;

        result.vertex_shader    = std::move(vertex_shader);
        result.pixel_shader     = std::move(pixel_shader);

        result.vertex_layout    = device->createInputLayout(
            vertex_attributes.data(),
            vertex_attributes.size(),
            result.vertex_shader
        );

        return result;
    }

    nvrhi::ShaderHandle vertex_shader;
    nvrhi::ShaderHandle pixel_shader;

    nvrhi::InputLayoutHandle vertex_layout;
};

}