#pragma once

#include <dep_nvrhi/nvrhi.hpp>
#include <module_render/util/ShaderFactory.hpp>

namespace lib::render {
namespace detail {

//! Default texture blit blending params
constexpr nvrhi::BlendState::RenderTarget DEFAULT_BLEND_STATE = nvrhi::BlendState::RenderTarget()
    .enableBlend()
    .setSrcBlend(nvrhi::BlendFactor::SrcAlpha)
    .setSrcBlendAlpha(nvrhi::BlendFactor::One)
    .setDestBlend(nvrhi::BlendFactor::OneMinusSrcAlpha)
    .setDestBlendAlpha(nvrhi::BlendFactor::OneMinusSrcAlpha)
    .setBlendOp(nvrhi::BlendOp::Add);

}

class TextureBlit {
    struct blit_instance_t {
        bool operator!() const {
            return !binding_set || !pipeline;
        }

        nvrhi::BindingSetHandle binding_set;
        nvrhi::GraphicsPipelineHandle pipeline;
    };

public:
    TextureBlit(
        nvrhi::IDevice* device,
        const std::unique_ptr<ShaderFactory>& shader_factory,
        const std::filesystem::path& rect_shader_path,
        const std::filesystem::path& blit_shader_path,
        const std::filesystem::path& blit_shader_array_path
    );

    //! Reset pipeline and binding set so we can rebind to new frame buffer texture
    void back_buffer_resizing();

    //! Blit texture from given parameters
    //! \param id blit id, must be convertable to uint32_t
    //! \param command_list command list that will execute the blit
    //! \param source_texture source texture that we want to copy
    //! \param dest_frame_buffer frame buffer we will use to write the source texture
    //! \param source_array_slice array slice/index of texture. Only used for array textures
    //! \param source_mip_level mipmap level of source texture we want to blit
    //! \param blend_state blend state to use when blitting
    //! \param blend_color blend color
    void blit(
        uint32_t id,
        nvrhi::ICommandList* command_list,
        nvrhi::ITexture* source_texture,
        nvrhi::IFramebuffer* dest_frame_buffer,
        uint32_t source_array_slice = 0,
        uint32_t source_mip_level = 0,
        const nvrhi::BlendState::RenderTarget& blend_state = detail::DEFAULT_BLEND_STATE,
        const nvrhi::Color& blend_color = nvrhi::Color(0.0)
    );

    //! Blit texture from given parameters
    //! \param id blit id, must be convertable to uint32_t
    //! \param command_list command list that will execute the blit
    //! \param source_texture source texture that we want to copy
    //! \param dest_frame_buffer frame buffer we will use to write the source texture
    //! \param source_array_slice array slice/index of texture. Only used for array textures
    //! \param source_mip_level mipmap level of source texture we want to blit
    //! \param blend_state blend state to use when blitting
    //! \param blend_color blend color
    template<typename ID>
    void blit(
        const ID id,
        nvrhi::ICommandList* command_list,
        nvrhi::ITexture* source_texture,
        nvrhi::IFramebuffer* dest_frame_buffer,
        const uint32_t source_array_slice = 0,
        const uint32_t source_mip_level = 0,
        const nvrhi::BlendState::RenderTarget& blend_state = detail::DEFAULT_BLEND_STATE,
        const nvrhi::Color& blend_color = nvrhi::Color(0.0)
    ) {
        blit(
            static_cast<uint32_t>(id),
            command_list,
            source_texture,
            dest_frame_buffer,
            source_array_slice,
            source_mip_level,
            blend_state,
            blend_color
        );
    }

private:
    nvrhi::IDevice* _device;
    nvrhi::ShaderHandle _rect_shader;

    nvrhi::ShaderHandle _blit_shader;
    nvrhi::ShaderHandle _blit_shader_array;

    nvrhi::SamplerHandle _linear_sampler;
    nvrhi::BindingLayoutHandle _blit_layout;

    std::unordered_map<uint32_t, blit_instance_t> _instances;
};

}