#pragma once

#include <dep_nvrhi/nvrhi.hpp>

#include <module_render/types/keys.hpp>
#include <module_render/util/ShaderFactory.hpp>

#include <unordered_map>

namespace lib::render {

class TextureBlit {
public:
    TextureBlit(
        nvrhi::IDevice* device,
        const std::shared_ptr<ShaderFactory>& shader_factory
    );

    //! Reset pipeline and binding set so we can rebind to new frame buffer texture
    void back_buffer_resizing();

    //! Blit texture from given parameters
    //! \param command_list command list that will execute the blit
    //! \param source_texture source texture that we want to copy
    //! \param dest_frame_buffer frame buffer we will use to write the source texture
    //! \param source_array_slice array slice/index of texture. Only used for array textures
    //! \param source_mip_level mipmap level of source texture we want to blit
    //! \param blend_state blend state to use when blitting
    //! \param blend_color blend color
    void blit(
        nvrhi::ICommandList* command_list,
        nvrhi::ITexture* source_texture,
        nvrhi::IFramebuffer* dest_frame_buffer,
        uint32_t source_array_slice = 0,
        uint32_t source_mip_level = 0,
        const nvrhi::BlendState::RenderTarget& blend_state = DEFAULT_BLEND_STATE,
        const nvrhi::Color& blend_color = nvrhi::Color(0.0)
    );

private:
    nvrhi::IDevice* _device;
    nvrhi::ShaderHandle _rect_shader;

    nvrhi::ShaderHandle _blit_shader;
    nvrhi::ShaderHandle _blit_shader_array;

    nvrhi::SamplerHandle _linear_sampler;
    nvrhi::BindingLayoutHandle _blit_layout;

    std::unordered_map<binding_set_desc_key, nvrhi::BindingSetHandle, binding_set_desc_key::hash> _binding_set;
    std::unordered_map<pipeline_desc_key, nvrhi::GraphicsPipelineHandle, pipeline_desc_key::hash> _pipeline;
};

}